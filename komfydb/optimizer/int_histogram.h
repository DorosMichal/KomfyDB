#ifndef __INT_HISTOGRAM_H__
#define __INT_HISTOGRAM_H__

#include "komfydb/execution/op.h"

namespace komfydb::optimizer {

class IntHistogram {
 public:
  IntHistogram(int buckets, int min, int max);

  void AddValue(int v);

  double EstimageSelectivity(execution::Op op, int v);

  double AverageSelecitivty();

  void Dump();
};

};  // namespace komfydb::optimizer

#endif
