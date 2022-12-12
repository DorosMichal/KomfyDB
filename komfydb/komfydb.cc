#include <iostream>
#include <string>

#include "absl/status/statusor.h"
#include "common/permissions.h"
#include "glog/logging.h"

#include "komfydb/common/td_item.h"
#include "komfydb/common/type.h"
#include "komfydb/database.h"
#include "komfydb/execution/order_by.h"
#include "komfydb/execution/seq_scan.h"
#include "komfydb/storage/heap_page.h"
#include "komfydb/storage/table_iterator.h"
#include "komfydb/utils/status_macros.h"

using namespace komfydb;

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  LOG(INFO) << "Welcome to KomfyDB!";

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

  std::unique_ptr<storage::TableIterator> table_iterator =
      std::make_unique<storage::TableIterator>(tid, table_id, catalog,
                                               buffer_pool);
  auto status_or_seq_scan = execution::SeqScan::Create(
      std::move(table_iterator), tid, "test_table", table_id);
  if (!status_or_seq_scan.ok()) {
    LOG(ERROR) << "Couldn't create seq_scan: "
               << status_or_seq_scan.status().message();
    return 1;
  }

  std::unique_ptr<execution::SeqScan> seq_scan =
      std::move(status_or_seq_scan.value());

  auto status_or_order_by = execution::OrderBy::Create(
      std::move(seq_scan), 0, execution::OrderBy::Order::ASCENDING);
  std::unique_ptr<execution::OrderBy> order_by =
      std::move(status_or_order_by.value());

  if (!order_by->Open().ok()) {
    LOG(ERROR) << "order_by open error";
  }

  LOG(INFO) << "Opened order_by on table "
            << catalog->GetTableName(table_id).value();

  while (order_by->HasNext()) {
    Record record = std::move(order_by->Next().value());
    std::cout << static_cast<std::string>(record) << "\n";
  }
  order_by->Close();
}
