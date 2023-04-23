#include "komfydb/optimizer/int_histogram.h"
#include "komfydb/storage/record.h"
#include "komfydb/common/field.h"
#include "komfydb/common/int_field.h"

namespace {
  using komfydb::storage::Record;
  using komfydb::common::Field;

}; // namespace

namespace komfydb::optimizer {

IntHistogram::IntHistogram(std::vector<std::unique_ptr<Record>> &records, int field_index){
  std::sort(records.begin(), records.end(),
            [field_index](const std::unique_ptr<Record>& a,
                   const std::unique_ptr<Record>& b) {
              absl::StatusOr<Field*> fa = a->GetField(field_index);
              absl::StatusOr<Field*> fb = b->GetField(field_index);
              assert(fa.ok());
              assert(fb.ok());
              Op::Value comp = Op::Value::LESS_THAN;
              absl::StatusOr<bool> result = (*fa)->Compare(Op(comp), *fb);
              assert(result.ok());
              return *result;
            });

  // bins should be roughly the same size
  number_of_values = records.size();
  int jump = number_of_values / N_BINS;
  for(int i = 0, j = 0; i < N_BINS; i++, j += jump){
    IntField *field = *records[j]->GetField(field_index);
    ranges[i] = field->GetValue();
    bins[i] = jump;
  }

  // set max as closing range
  IntField *field = *records[number_of_values-1]->GetField(field_index);
  ranges[N_BINS] = field->GetValue();
  // add remaining elements
  bins[N_BINS - 1] += number_of_values % N_BINS;

}

inline int IntHistogram::GetMin(){
  return ranges[0];
}

inline int IntHistogram::GetMax(){
  return ranges[N_BINS];
}

int IntHistogram::GetBin(int v){
  // we need min <= v < max
  return std::upper_bound(std::begin(ranges) + 1, std::end(ranges), v) - ranges - 1;
}

void IntHistogram::AddValue(int v) {
  if (v <= GetMin()){
    ranges[0] = v;
    bins[0]++;
  } else if(GetMax() <= v){
    ranges[N_BINS] = v;
    bins[N_BINS - 1]++;
  } else {
    int bin = std::upper_bound(ranges, ranges + N_BINS, v) - ranges - 1;
    bins[bin]++;
  }
  number_of_values++;
}

double IntHistogram::EstimateSelectivity(execution::Op op, int v) {
  assert(number_of_values > 0);
  
  int number_of_smaller = 0;
  if (v < GetMin()){
    number_of_smaller = 0;
  } else if (GetMax() <= v) {
    number_of_smaller = number_of_values;
  } else {
    int bin = GetBin(v);
    for(int i = 0; i < bin; i++){
      number_of_smaller += counter[i];
    }
    number_of_smaller += (v - ranges[bin]) * counter[bin] / (ranges[bin + 1] - ranges[bin]);
  }

  switch (op.value) {
    case execution::Op::GREATER_THAN:
    case execution::Op::GREATER_THAN_OR_EQ:
      return (double)(number_of_values - number_of_smaller) / number_of_values;
    case execution::Op::LESS_THAN:
    case execution::Op::LESS_THAN_OR_EQ:
      return (double)(number_of_smaller) / number_of_values;
    case execution::Op::EQUALS:
      return 0.05;
    case execution::Op::NOT_EQUALS:
      return 0.95;
    case execution::Op::LIKE:
      // should never happen
      assert(false);
  }
}

double IntHistogram::AverageSelectivity() {
  // TODO
  // what for, whats the semantics?
  return 1.0;
}

void IntHistogram::Dump() {
  for(int i = 0; i < N_BINS; i++){
    std::cout << '[' << ranges[i] << ',' << ranges[i+1] << ") \t->\t" << bins[i] << '\n';
  }
}

};  // namespace komfydb::optimizer
