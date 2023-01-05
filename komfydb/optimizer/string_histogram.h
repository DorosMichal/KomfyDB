#ifndef __STRING_HISTOGRAM_H__
#define __STRING_HISTOGRAM_H__

#include "komfydb/execution/op.h"

namespace komfydb::optimizer {

class StringHistogram {
 public:
  static const std::string MAX_VAL;
  static const std::string MIN_VAL;

  StringHistogram(int buckets, std::string min, std::string max);

  void AddValue(std::string v);

  double EstimageSelectivity(execution::Op op, std::string v);

  double AverageSelecitivty();

  void Dump();
};

};  // namespace komfydb::optimizer

#endif
