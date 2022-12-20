#include <hsql/sql/SelectStatement.h>
#include <readline/history.h>
#include <readline/readline.h>

#include <iostream>

#include "execution/logical_plan/logical_plan.h"
#include "glog/logging.h"
#include "hsql/SQLParser.h"
#include "hsql/util/sqlhelper.h"

#include "komfydb/common/td_item.h"
#include "komfydb/common/type.h"
#include "komfydb/database.h"
#include "komfydb/execution/order_by.h"
#include "komfydb/execution/seq_scan.h"
#include "komfydb/parser.h"
#include "komfydb/storage/heap_page.h"
#include "komfydb/storage/table_iterator.h"
#include "komfydb/utils/status_macros.h"

using namespace komfydb;

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  LOG(INFO) << "Parser testing!";

  absl::StatusOr<Database> db =
      Database::LoadSchema("komfydb/testdata/database_catalog_test.txt");
  if (!db.ok()) {
    LOG(ERROR) << "LoadSchema error: " << db.status().message();
  }

  std::shared_ptr<Catalog> catalog = db->GetCatalog();
  std::shared_ptr<BufferPool> buffer_pool = db->GetBufferPool();
  Parser parser(std::move(catalog));

  char* query;
  while ((query = readline("KomfyDB> ")) != nullptr) {
    if (!strlen(query)) {
      continue;
    }
    add_history(query);
    hsql::SQLParserResult result;
    hsql::SQLParser::parse(query, &result);

    absl::StatusOr<execution::logical_plan::LogicalPlan> lp =
        parser.ParseQuery(query);
    if (!lp.ok()) {
      LOG(ERROR) << "Parsing error: " << lp.status().message();
    } else {
      LOG(INFO) << "Parsing ok!";
      lp->Dump();
    }

    free(query);
  }
}
