#include "komfydb/optimizer/int_histogram.h"

namespace komfydb::optimizer {

IntHistogram::IntHistogram(int number_of_buckets)
    : number_of_buckets(number_of_buckets), min(min), max(max) {
  bucket_size = (max - min + number_of_buckets - 1) / number_of_buckets;
  buckets.reserve(number_of_buckets);
  for (int i = 0; i < number_of_buckets; i++) {
    buckets.push_back(0);
  }
  number_of_values = 0;
  min_outliers = 0;
  max_outliers = 0;
}

void IntHistogram::AddValue(int v) {
  number_of_values++;
  if (v < min) {
    min_outliers++;
  } else if (max < v) {
    max_outliers++;
  } else {
    int idx = (v - min) / bucket_size;
    buckets[idx]++;
  }
  return;
}

double IntHistogram::EstimateSelectivity(execution::Op op, int v) {
  int number_of_smaller = min_outliers;
  if (min <= v && v <= max) {
    int idx = (v - min) / bucket_size;
    for (int i = 0; i < idx; i++) {
      number_of_smaller += buckets[i];
    }
    number_of_smaller +=
        (buckets[idx] * (v - min - idx * bucket_size)) / bucket_size;
  }
  switch (op.value) {
    case execution::Op::GREATER_THAN:
    case execution::Op::GREATER_THAN_OR_EQ:
      return (double)(number_of_values - number_of_smaller) / number_of_values;
    case execution::Op::LESS_THAN:
    case execution::Op::LESS_THAN_OR_EQ:
      return (double)(number_of_smaller) / number_of_values;
    case execution::Op::EQUALS:
      return 0.05;
    case execution::Op::NOT_EQUALS:
      return 0.95;
    case execution::Op::LIKE:
      // should never happen
  }
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
