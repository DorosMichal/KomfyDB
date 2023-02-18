#include <hsql/sql/SelectStatement.h>
#include <readline/history.h>
#include <readline/readline.h>

#include <iostream>

#include "execution/logical_plan/logical_plan.h"
#include "execution/op_iterator.h"
#include "glog/logging.h"
#include "hsql/SQLParser.h"
#include "hsql/util/sqlhelper.h"

#include "komfydb/common/td_item.h"
#include "komfydb/common/type.h"
#include "komfydb/database.h"
#include "komfydb/execution/executor.h"
#include "komfydb/execution/order_by.h"
#include "komfydb/execution/seq_scan.h"
#include "komfydb/parser.h"
#include "komfydb/storage/heap_file.h"
#include "komfydb/storage/heap_page.h"
#include "komfydb/storage/table_iterator.h"
#include "komfydb/utils/status_macros.h"
#include "optimizer/table_stats.h"
#include "transaction/transaction_id.h"
#include "utils/status_macros.h"

using namespace komfydb;

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  LOG(INFO) << "Parser testing!";

  const std::string catalog_directory = "komfydb/testdata/";
  absl::StatusOr<Database> db =
      Database::LoadSchema(catalog_directory + "database_catalog_test.txt");
  if (!db.ok()) {
    LOG(ERROR) << "LoadSchema error: " << db.status().message();
  }

  std::shared_ptr<Catalog> catalog = db->GetCatalog();
  std::shared_ptr<BufferPool> buffer_pool = db->GetBufferPool();

  // TODO: Shouldnt table stats map be stored in database the same as catalog
  // and buffer pool?
  optimizer::TableStatsMap table_stats_map;
  Parser parser(catalog, buffer_pool, table_stats_map);
  execution::Executor executor;

  char* input;
  while ((input = readline("KomfyDB> ")) != nullptr) {
    if (!strlen(input)) {
      continue;
    }
    add_history(input);
    std::string query_str = input;
    free(input);

    hsql::SQLParserResult result;
    hsql::SQLParser::parse(query_str, &result);

    uint64_t limit = 0;
    absl::StatusOr<Query> query =
        parser.ParseQuery(query_str, TransactionId(), &limit, false);
    if (!query.ok()) {
      LOG(ERROR) << "Parsing error: " << query.status().message();
      continue;
    }

    switch (query->type) {
      case Query::ITERATOR: {
        query->iterator->Explain(std::cout);
        absl::Status status =
            executor.PrettyExecute(std::move(query->iterator), limit);
        if (!status.ok()) {
          std::cout << "Executor error: " << status.message() << std::endl;
        }
        break;
      }
      case Query::CREATE_TABLE: {
        if (catalog->GetTableId(query->table_name).ok()) {
          std::cout << "Table " << query->table_name << " already exists."
                    << std::endl;
          continue;
        }
        absl::StatusOr<std::unique_ptr<DbFile>> new_file =
            HeapFile::Create(catalog_directory + query->table_name + ".dat",
                             query->tuple_desc, Permissions::READ_WRITE);
        if (!new_file.status().ok()) {
          std::cout << "Cannot create table: " << new_file.status().message()
                    << std::endl;
          continue;
        }

        catalog->AddTable(std::move(*new_file), query->table_name,
                          query->primary_key);
        break;
      }
    }
  }
}
