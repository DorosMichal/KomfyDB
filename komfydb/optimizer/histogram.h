#ifndef __HISTOGRAM_H__
#define __HISTOGRAM_H__

#include "komfydb/execution/op.h"

namespace komfydb::optimizer {

class Histogram {
 public:
  virtual double EstimateSelectivity(execution::Op op, int v);

  virtual double AverageSelecitivty();

  virtual void Dump();
};

};  // namespace komfydb::optimizer

#endif