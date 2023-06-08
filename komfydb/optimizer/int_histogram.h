#ifndef __INT_HISTOGRAM_H__
#define __INT_HISTOGRAM_H__

#include "komfydb/execution/op.h"
#include "komfydb/optimizer/histogram.h"
#include "komfydb/storage/record.h"

namespace {
using komfydb::common::Field;
using komfydb::storage::Record;

};  // namespace

namespace komfydb::optimizer {

class IntHistogram : public Histogram {
 public:
  IntHistogram(std::vector<std::unique_ptr<Record>>& records, int field_index);

  void AddValue(int v);

  double EstimateSelectivity(execution::Op op, Field* v) override;

  double AverageSelectivity() override;

  void Dump() override;

 private:
  const static int NUM_BINS = 20;
  int number_of_values;
  int ranges[NUM_BINS + 1];
  int bins[NUM_BINS];

  int GetBin(int value);

  int GetMin();

  int GetMax();
};

};  // namespace komfydb::optimizer

#endif
