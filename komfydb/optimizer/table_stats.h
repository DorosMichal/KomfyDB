#ifndef __TABLE_STATS_H__
#define __TABLE_STATS_H__

#include "absl/status/statusor.h"

#include "komfydb/common/field.h"
#include "komfydb/execution/op.h"

namespace {

using komfydb::common::Field;

};

namespace komfydb::optimizer {

class TableStats {
 public:
  static absl::StatusOr<TableStats> Create(int table_id, int io_cost_per_page);

  double EstimateScanCost();

  int EstimateTableCardinality();

  double AverageSelectivity(int column, execution::Op op);

  double EstimateSelectivity(int column, execution::Op op, Field* constant);

 private:
  TableStats(int table_id, int io_cost_per_page);
};

};  // namespace komfydb::optimizer

#endif
