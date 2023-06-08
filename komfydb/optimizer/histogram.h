#ifndef __HISTOGRAM_H__
#define __HISTOGRAM_H__

#include "komfydb/common/field.h"
#include "komfydb/execution/op.h"

namespace {
using komfydb::common::Field;
};

namespace komfydb::optimizer {

class Histogram {
 public:
  Histogram() = default;

  Histogram(Histogram&&) = default;

  virtual ~Histogram() = default;

  virtual double EstimateSelectivity(execution::Op op, Field* value) = 0;

  virtual double AverageSelectivity() = 0;

  virtual void Dump() = 0;

  
};

};  // namespace komfydb::optimizer

#endif