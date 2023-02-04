#ifndef __STRING_HISTOGRAM_H__
#define __STRING_HISTOGRAM_H__

#include "komfydb/execution/op.h"
#include "komfydb/optimizer/histogram.h"

namespace komfydb::optimizer {

class StringHistogram : public Histogram {
 public:
  static const std::string MAX_VAL;
  static const std::string MIN_VAL;

  StringHistogram(int buckets, std::string min, std::string max);

  void AddValue(std::string v);

  double EstimateSelectivity(execution::Op op, std::string v);

  double AverageSelecitivty();

  void Dump();
};

};  // namespace komfydb::optimizer

#endif
