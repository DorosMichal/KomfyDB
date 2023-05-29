#ifndef __HISTOGRAM_H__
#define __HISTOGRAM_H__

#include "komfydb/execution/op.h"
#include "komfydb/common/field.h"

namespace {
  using komfydb::common::Field;
};

namespace komfydb::optimizer {

class Histogram {
 public:
  virtual double EstimateSelectivity(execution::Op op, Field* value);

  virtual double AverageSelecitivty();

  virtual void Dump();
};

};  // namespace komfydb::optimizer

#endif