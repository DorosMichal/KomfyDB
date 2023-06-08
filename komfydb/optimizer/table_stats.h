#ifndef __TABLE_STATS_H__
#define __TABLE_STATS_H__

#include <memory>
#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"

#include "komfydb/common/field.h"
#include "komfydb/execution/op.h"
#include "komfydb/optimizer/histogram.h"
#include "komfydb/storage/table_iterator.h"

namespace {

using komfydb::common::Field;
using komfydb::storage::BufferPool;
using komfydb::storage::Catalog;

struct FilterInformation {
  double compound_selectivity;
  bool filter_present;
};

};  // namespace

namespace komfydb::optimizer {

class TableStats {
 public:
  static absl::StatusOr<std::shared_ptr<TableStats>> Create(
      int table_id, int io_cost_per_page, std::shared_ptr<Catalog> catalog,
      std::shared_ptr<BufferPool> bufferpool);

  double EstimateScanCost();

  int EstimateTableCardinality();

  double AverageSelectivity(int column, execution::Op op);

  double EstimateSelectivity(int column, execution::Op op, Field* constant);

  // TODO: this function is on wrong level, we need information about two tables to process col:col filter in any reasonable way
  // it needs to be moved higher
  double EstimateSelectivity(int lcolumn, execution::Op op, int rcolumn);

  bool IsFilterPresent();

  void SetFilterInfoStatus(bool filter_present);

  void SetFilterSelectivity(double selectivity);

  void SetCompoundSelectivity(double multiplier);

 private:
  TableStats(int table_id, int io_cost_per_page)
      : table_id(table_id), io_cost_per_page(io_cost_per_page) {}
  int table_id;
  int io_cost_per_page;
  int number_of_tuples;
  int number_of_pages;
  int scan_cost;
  std::vector<std::unique_ptr<Histogram>> histograms;
  // we need additional info from processing filters
  // this information is per query (lets assume per transaction) so (in future) we'll keep hash table indexed with transaction_id
  // for now we don't really pass transaction_id anywhere so the structure will be set over and over with every query
  FilterInformation filter_information;
};

using TableStatsMap =
    absl::flat_hash_map<std::string, std::shared_ptr<TableStats>>;

};  // namespace komfydb::optimizer

#endif
