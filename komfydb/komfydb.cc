#include <iostream>
#include <string>

#include "absl/status/statusor.h"
#include "common/permissions.h"
#include "glog/logging.h"

#include "komfydb/common/td_item.h"
#include "komfydb/common/type.h"
#include "komfydb/database.h"
#include "komfydb/execution/aggregate.h"
#include "komfydb/execution/filter.h"
#include "komfydb/execution/join.h"
#include "komfydb/execution/join_predicate.h"
#include "komfydb/execution/order_by.h"
#include "komfydb/execution/predicate.h"
#include "komfydb/execution/project.h"
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
  auto table_iterator1 =
      storage::TableIterator::Create(tid, table_ids[0], catalog, buffer_pool);
  if (!table_iterator1.ok()) {
    LOG(ERROR) << "Couldn't create table iterator: "
               << table_iterator1.status().message();
  }
  auto status_or_seq_scan1 = execution::SeqScan::Create(
      std::move(*table_iterator1), tid, "test_table", table_ids[0]);
  if (!status_or_seq_scan1.ok()) {
    LOG(ERROR) << "Couldn't create seq_scan: "
               << status_or_seq_scan1.status().message();
    return 1;
  }

  std::unique_ptr<execution::SeqScan> seq_scan1 =
      std::move(*status_or_seq_scan1);

  std::unique_ptr<IntField> filter_operand1 =
      std::make_unique<IntField>(2000000000);
  execution::Predicate filter_predicate1(0, Op::Value::GREATER_THAN_OR_EQ,
                                         std::move(filter_operand1));
  auto status_or_filter1 = execution::Filter::Create(
      std::move(seq_scan1), std::move(filter_predicate1));

  std::unique_ptr<execution::Filter> filter1 = std::move(*status_or_filter1);

  // table 2

  auto table_iterator2 =
      storage::TableIterator::Create(tid, table_ids[1], catalog, buffer_pool);
  if (!table_iterator2.ok()) {
    LOG(ERROR) << "Couldn't create table iterator: "
               << table_iterator2.status().message();
  }
  auto status_or_seq_scan2 = execution::SeqScan::Create(
      std::move(*table_iterator2), tid, "test_table", table_ids[1]);
  if (!status_or_seq_scan2.ok()) {
    LOG(ERROR) << "Couldn't create seq_scan: "
               << status_or_seq_scan2.status().message();
    return 1;
  }

  std::unique_ptr<execution::SeqScan> seq_scan2 =
      std::move(*status_or_seq_scan2);

  std::unique_ptr<IntField> filter_operand2 =
      std::make_unique<IntField>(2000000000);
  execution::Predicate filter_predicate2(0, Op::Value::GREATER_THAN_OR_EQ,
                                         std::move(filter_operand2));
  auto status_or_filter2 = execution::Filter::Create(
      std::move(seq_scan2), std::move(filter_predicate2));

  std::unique_ptr<execution::Filter> filter2 = std::move(*status_or_filter2);

  // join

  execution::JoinPredicate join_predicate(0, execution::Op::EQUALS, 0);
  auto status_or_join = execution::Join::Create(
      std::move(filter1), join_predicate, std::move(filter2));

  if (!status_or_join.ok()) {
    LOG(ERROR) << "Couldn't create join: " << status_or_join.status().message();
    return 1;
  }
  std::unique_ptr<execution::Join> join = std::move(*status_or_join);

  // aggregate

  std::vector<AggregateType> aggregate_types = {
      AggregateType::NONE, AggregateType::AVG, AggregateType::COUNT};
  std::vector<int> aggregate_fields = {3, 0, 1}, groupby_fields = {3};

  auto status_or_aggregate = execution::Aggregate::Create(
      std::move(join), aggregate_types, aggregate_fields, groupby_fields);

  std::unique_ptr<execution::Aggregate> aggregate =
      std::move(*status_or_aggregate);

  // order by

  auto status_or_order_by = execution::OrderBy::Create(
      std::move(aggregate), 0, execution::OrderBy::Order::ASCENDING);
  std::unique_ptr<execution::OrderBy> order_by = std::move(*status_or_order_by);

  // project

  // NOTE: Aggregate does projecting too
  std::vector<int> out_field_idxs = {0, 1};
  auto status_or_project =
      execution::Project::Create(std::move(order_by), out_field_idxs);
  std::unique_ptr<execution::Project> project = std::move(*status_or_project);

  project->Explain(std::cout);

  if (!project->Open().ok()) {
    LOG(ERROR) << "project open error";
    return 1;
  }

  LOG(INFO) << "Opened project\n";

  ITERATE_RECORDS(project, record) {
    std::cout << static_cast<std::string>(*(*record)) << "\n";
  }
  if (!absl::IsOutOfRange(record.status())) {
    LOG(ERROR) << record.status().message();
  }

  project->Close();
}
