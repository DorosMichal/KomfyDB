#ifndef __INT_HISTOGRAM_H__
#define __INT_HISTOGRAM_H__

#include "komfydb/execution/op.h"
#include "komfydb/optimizer/histogram.h"

namespace komfydb::optimizer {

class IntHistogram : Public Histogram {
 public:
  IntHistogram(int number_of_buckets, int min, int max);

  void AddValue(int v);

  double EstimateSelectivity(execution::Op op, int v);

  double AverageSelecitivty();

  void Dump();

 private:
  int number_of_buckets;
  int min;
  int max;
  int bucket_size;
  std::vector<int> buckets;
  int number_of_values;
  int min_outliers;
  int max_outliers;
};

};  // namespace komfydb::optimizer

#endif
