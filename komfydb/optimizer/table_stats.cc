#include "glog/logging.h"

#include "komfydb/execution/op_iterator.h"
#include "komfydb/optimizer/table_stats.h"
#include "komfydb/storage/table_iterator.h"

#include "absl/status/statusor.h"
#include "komfydb/utils/status_macros.h"

#include "komfydb/optimizer/int_histogram.h"
#include "komfydb/optimizer/string_histogram.h"
#include "table_stats.h"

namespace {
using komfydb::storage::BufferPool;
using komfydb::storage::Catalog;
using komfydb::storage::TableIterator;
using komfydb::transaction::TransactionId;

}  // namespace

namespace komfydb::optimizer {

absl::StatusOr<std::shared_ptr<TableStats>> TableStats::Create(
    int table_id, int io_cost_per_page, std::shared_ptr<Catalog> catalog,
    std::shared_ptr<BufferPool> bufferpool) {
  // read whole table
  std::unique_ptr<TableIterator> table_iterator =
      TableIterator::Create(TransactionId(), table_id, catalog, bufferpool)
          .value();
  absl::Status open_status = table_iterator->Open();
  if (!open_status.ok()) {
    LOG(INFO) << "Failed to open table iterator to read statistics from table "
                 "with id "
              << table_id;
    return open_status;
  }
  std::shared_ptr<TableStats> table_stats(
      new TableStats(table_id, io_cost_per_page));
  std::vector<std::unique_ptr<Record>> records;
  absl::StatusOr<Record> record;
  while ((record = (table_iterator)->Next()).ok()) {
    records.push_back(std::make_unique<Record>(*record));
  }
  ASSIGN_OR_RETURN(TupleDesc * tuple_desc, catalog->GetTupleDesc(table_id));
  for (int i = 0; i < tuple_desc->Length(); i++) {
    ASSIGN_OR_RETURN(Type field_type, tuple_desc->GetFieldType(i));
    if (field_type.GetValue() == Type::Value::INT) {
      table_stats->histograms.push_back(
          std::make_unique<IntHistogram>(records, i));
    } else {
      table_stats->histograms.push_back(
          std::make_unique<StringHistogram>(records, i));
    }
  }

  // get table size
  table_stats->number_of_tuples = records.size();
  table_stats->number_of_pages =
      (catalog->GetDatabaseFile(table_id)).value()->PageCount();
  table_stats->scan_cost = io_cost_per_page * table_stats->number_of_pages;

  return table_stats;
}

double TableStats::EstimateScanCost() {
  return scan_cost;
}

int TableStats::EstimateTableCardinality() {
  double selectivity = filter_information.filter_present
                           ? filter_information.compound_selectivity
                           : 1.0;
  return (int)(number_of_tuples * selectivity);
}

double TableStats::AverageSelectivity(int column, execution::Op op) {
  // TODO what for
  return 1.0;
}

double TableStats::EstimateSelectivity(int column, execution::Op op,
                                       Field* constant) {
  return histograms[column]->EstimateSelectivity(op, constant);
}

double TableStats::EstimateSelectivity(int lcolumn, execution::Op op,
                                       int rcolumn) {
  // TODO
  // it kinda needs to be higher for filter between two tables

  // we assume lcol and rcol are from one table only 
  return 1.0;
}

bool TableStats::IsFilterPresent() {
  return filter_information.filter_present;
}

void TableStats::SetFilterInfoStatus(bool filter_present) {
  filter_information.filter_present = filter_present;
}

void TableStats::SetFilterSelectivity(double selectivity) {
  filter_information.compound_selectivity = selectivity;
}

void TableStats::SetCompoundSelectivity(double multiplier) {
  filter_information.compound_selectivity *= multiplier;
}

};  // namespace komfydb::optimizer
