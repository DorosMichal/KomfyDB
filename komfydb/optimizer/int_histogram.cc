#include "komfydb/optimizer/int_histogram.h"

namespace komfydb::optimizer {

IntHistogram::IntHistogram(int buckets, int min, int max) {
  // TODO
}

void IntHistogram::AddValue(int v) {
  // TODO
  return;
}

double IntHistogram::EstimageSelectivity(execution::Op op, int v) {
  // TODO
  return 1.0;
}

double IntHistogram::AverageSelecitivty() {
  // TODO
  return 1.0;
}

void IntHistogram::Dump() {
  // TODO
  return;
}

};  // namespace komfydb::optimizer
