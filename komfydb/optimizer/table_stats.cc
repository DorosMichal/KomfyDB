#include "komfydb/optimizer/table_stats.h"

#include "absl/status/statusor.h"

namespace komfydb::optimizer {

absl::StatusOr<TableStats> TableStats::Create(int table_id,
                                              int io_cost_per_page) {
  return absl::UnimplementedError("TODO");
}

double TableStats::EstimateScanCost() {
  // TODO
  return 1.0;
}

int TableStats::EstimateTableCardinality() {
  // TODO
  return 1000;
}

double TableStats::AverageSelectivity(int column, execution::Op op) {
  // TODO
  return 1.0;
}

double TableStats::EstimateSelectivity(int column, execution::Op op,
                                       Field* constant) {
  // TODO
  return 1.0;
}

double TableStats::EstimateSelectivity(int lcolumn, execution::Op op,
                                       int rcolumn) {
  // TODO
  return 1.0;
}

};  // namespace komfydb::optimizer
