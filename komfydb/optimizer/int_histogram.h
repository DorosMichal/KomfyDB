#ifndef __INT_HISTOGRAM_H__
#define __INT_HISTOGRAM_H__

#include "komfydb/execution/op.h"
#include "komfydb/optimizer/histogram.h"
#include "komfydb/storage/record.h"

namespace {
  using komfydb::storage::Record;
  using komfydb::common::Field;
};


namespace komfydb::optimizer {

const int N_BINS = 20;

class IntHistogram : public Histogram {
 public:
  IntHistogram(std::vector<std::unique_ptr<Record>> &records, int field_index);

  void AddValue(int v);

  double EstimateSelectivity(execution::Op op, Field* v);

  double AverageSelecitivty();

  void Dump();

 private:
  int number_of_values;
  int ranges[N_BINS + 1];
  int bins[N_BINS];

  int GetBin(int value);

  int GetMin();

  int GetMax();


};

};  // namespace komfydb::optimizer

#endif
