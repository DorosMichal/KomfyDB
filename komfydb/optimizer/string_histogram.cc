#include "komfydb/optimizer/string_histogram.h"

namespace komfydb::optimizer {

StringHistogram::StringHistogram(int buckets, std::string min,
                                 std::string max) {
  // TODO
}

void StringHistogram::AddValue(std::string v) {
  // TODO
  return;
}

double StringHistogram::EstimateSelectivity(execution::Op op, std::string v) {
  // TODO
  return 1.0;
}

double StringHistogram::AverageSelecitivty() {
  // TODO
  return 1.0;
}

void StringHistogram::Dump() {
  // TODO
  return;
}

};  // namespace komfydb::optimizer
