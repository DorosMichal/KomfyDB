#ifndef __TABLE_STATS_H__
#define __TABLE_STATS_H__

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"

#include "komfydb/common/field.h"
#include "komfydb/execution/op.h"
#include "komfydb/optimizer/histogram.h"

namespace {

using komfydb::common::Field;

};  // namespace

namespace komfydb::optimizer {

class TableStats {
 public:
  static absl::StatusOr<TableStats> Create(int table_id, int io_cost_per_page);

  double EstimateScanCost();

  int EstimateTableCardinality();

  double AverageSelectivity(int column, execution::Op op);

  double EstimateSelectivity(int column, execution::Op op, Field* constant);

  double EstimateSelectivity(int lcolumn, execution::Op op, int rcolumn);

 private:
  TableStats(int table_id, int io_cost_per_page);
  str::vector<*Histogram> histograms;

};

using TableStatsMap = absl::flat_hash_map<std::string, TableStats>;

};  // namespace komfydb::optimizer

#endif
