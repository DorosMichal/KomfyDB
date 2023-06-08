#ifndef __STRING_HISTOGRAM_H__
#define __STRING_HISTOGRAM_H__

#include "komfydb/common/field.h"
#include "komfydb/execution/op.h"
#include "komfydb/optimizer/histogram.h"
#include "komfydb/storage/record.h"

namespace {
using komfydb::common::Field;
using komfydb::storage::Record;
}  // namespace

namespace komfydb::optimizer {

class StringHistogram : public Histogram {
 public:
  static const std::string MAX_VAL;
  static const std::string MIN_VAL;

  StringHistogram(std::vector<std::unique_ptr<Record>>& records,
                  int field_index);

  void AddValue(std::string v);

  double EstimateSelectivity(execution::Op op, Field* value) override;

  double AverageSelectivity() override;

  void Dump() override;

 private:
 // bin for every printable ASCII character
  const static int N_BINS = 94;
  const static int FIRST_PRINTABLE = 33;

  int number_of_values;
  int bins[N_BINS];
  int empty_count;
};

};  // namespace komfydb::optimizer

#endif
