#include <iostream>
#include <string>

#include "absl/status/statusor.h"
#include "common/permissions.h"
#include "glog/logging.h"

#include "komfydb/common/td_item.h"
#include "komfydb/common/type.h"
#include "komfydb/database.h"
#include "komfydb/execution/join.h"
#include "komfydb/execution/join_predicate.h"
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

  // table 1
  std::unique_ptr<storage::TableIterator> table_iterator1 =
      std::make_unique<storage::TableIterator>(tid, table_ids[0], catalog,
                                               buffer_pool);
  auto status_or_seq_scan1 = execution::SeqScan::Create(
      std::move(table_iterator1), tid, "test_table", table_ids[0]);
  if (!status_or_seq_scan1.ok()) {
    LOG(ERROR) << "Couldn't create seq_scan: "
               << status_or_seq_scan1.status().message();
    return 1;
  }

  std::unique_ptr<execution::SeqScan> seq_scan1 =
      std::move(status_or_seq_scan1.value());

  // table 2

  std::unique_ptr<storage::TableIterator> table_iterator2 =
      std::make_unique<storage::TableIterator>(tid, table_ids[1], catalog,
                                               buffer_pool);
  auto status_or_seq_scan2 = execution::SeqScan::Create(
      std::move(table_iterator2), tid, "test_table", table_ids[1]);
  if (!status_or_seq_scan2.ok()) {
    LOG(ERROR) << "Couldn't create seq_scan: "
               << status_or_seq_scan2.status().message();
    return 1;
  }

  std::unique_ptr<execution::SeqScan> seq_scan2 =
      std::move(status_or_seq_scan2.value());

  // join

  execution::JoinPredicate join_predicate(0, execution::Op::EQUALS, 0);
  auto status_or_join = execution::Join::Create(
      std::move(seq_scan1), join_predicate, std::move(seq_scan2));

  if (!status_or_join.ok()) {
    LOG(ERROR) << "Couldn't create join: " << status_or_join.status().message();
    return 1;
  }
  std::unique_ptr<execution::Join> join = std::move(status_or_join.value());

  auto status_or_order_by = execution::OrderBy::Create(
      std::move(join), 0, execution::OrderBy::Order::ASCENDING);
  std::unique_ptr<execution::OrderBy> order_by =
      std::move(status_or_order_by.value());

  if (!order_by->Open().ok()) {
    LOG(ERROR) << "order_by open error";
  }

  LOG(INFO) << "Opened order_by\n";
  LOG(ERROR) << "udalo sie\n";

  ITERATE_RECORDS(order_by, record) {
    std::cout << static_cast<std::string>(*(record.value())) << "\n";
  }
  if (!absl::IsOutOfRange(record.status())) {
    LOG(ERROR) << record.status().message();
  }
  order_by->Close();
}
