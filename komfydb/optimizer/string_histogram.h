#ifndef __STRING_HISTOGRAM_H__
#define __STRING_HISTOGRAM_H__

#include "komfydb/execution/op.h"
#include "komfydb/optimizer/histogram.h"
#include "komfydb/storage/record.h"
#include "komfydb/common/field.h"

namespace {
  using komfydb::storage::Record;
  using komfydb::common::Field;
}

namespace komfydb::optimizer {

// bin for every printable ASCII character
const int N_BINS = 94;
const int FIRST_PRINTABLE = 33;

class StringHistogram : public Histogram {
 public:
  static const std::string MAX_VAL;
  static const std::string MIN_VAL;

  StringHistogram(std::vector<std::unique_ptr<Record>> &records, int field_index);

  void AddValue(std::string v);

  double EstimateSelectivity(execution::Op op, Field* value);

  double AverageSelecitivty();

  void Dump();

 private:
  int number_of_values;
  int bins[94];
  int empty_count;
};

};  // namespace komfydb::optimizer

#endif
