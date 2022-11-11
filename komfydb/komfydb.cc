#include <iostream>
#include <string>

#include "absl/status/statusor.h"
#include "common/permissions.h"
#include "glog/logging.h"

#include "komfydb/common/td_item.h"
#include "komfydb/common/type.h"
#include "komfydb/database.h"
#include "komfydb/execution/seq_scan.h"
#include "komfydb/storage/heap_page.h"
#include "komfydb/storage/table_iterator.h"
#include "komfydb/utils/status_macros.h"

using namespace komfydb;

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  LOG(INFO) << "Welcome to KonfyDB!";

  absl::StatusOr<Database> db =
      Database::LoadSchema("komfydb/testdata/database_catalog_test.txt");
  if (!db.ok()) {
    LOG(ERROR) << "LoadSchema error: " << db.status().message();
  }

  std::shared_ptr<Catalog> catalog = db->GetCatalog();
  std::shared_ptr<BufferPool> buffer_pool = db->GetBufferPool();

  TransactionId tid;
  std::vector<int> table_ids = catalog->GetTableIds();
  int table_id = table_ids[0];

  storage::TableIterator table_iterator(tid, table_id, catalog, buffer_pool);
  execution::SeqScan seq_scan(table_iterator, tid, "test_table", table_id);

  if (!seq_scan.Open().ok()) {
    LOG(ERROR) << "seq_scan open error? lol";
  }

  LOG(INFO) << "Opened seq_scan on table "
            << catalog->GetTableName(table_id).value();

  while (seq_scan.HasNext()) {
    Record record = std::move(seq_scan.Next().value());
    std::cout << static_cast<std::string>(record) << "\n";
  }
}
