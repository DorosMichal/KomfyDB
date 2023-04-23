#include "komfydb/optimizer/string_histogram.h"
#include "komfydb/storage/record.h"
#include "komfydb/common/string_field.h"

namespace {
  using komfydb::storage::Record;

}; // namespace


namespace komfydb::optimizer {

StringHistogram::StringHistogram(std::vector<std::unique_ptr<Record>> &records, int field_index) {
  number_of_values = records.size();
  for(int i = 0; i < number_of_values; i++){
    StringField *field = *records[i]->GetField(field_index);
    std::string value = field->GetValue();
    if(value.empty()){
      empty_count++;
    } else {
      bins[value[0] - FIRST_PRINTABLE]++;
    }
  }
  
}

void StringHistogram::AddValue(std::string v) {
  number_of_values++;
  if(v.empty()){
    empty_count++;
  } else {
    bins[v[0] - FIRST_PRINTABLE]++;
  }
}

double StringHistogram::EstimateSelectivity(execution::Op op, std::string v) {
  int number_of_smaller_equal = empty_count;

  if(!v.empty()){
    int first_letter = v[0] - FIRST_PRINTABLE;

    for(int i = 0; i < first_letter; i++){
      number_of_smaller_equal += bins[i];
    }
    // estimate possition in bucket, we simply use the next letter
    if(v.size() > 1){
      int second_letter = v[1] - FIRST_PRINTABLE;
      number_of_smaller_equal += second_letter * bins[first_letter] / N_BINS;
    }
  }

  switch (op.value) {
    case execution::Op::GREATER_THAN:
    case execution::Op::GREATER_THAN_OR_EQ:
      return (double)(number_of_values - number_of_smaller_equal) / number_of_values;
    case execution::Op::LESS_THAN:
    case execution::Op::LESS_THAN_OR_EQ:
      return (double)(number_of_smaller_equal) / number_of_values;
    case execution::Op::EQUALS:
      return 0.05;
    case execution::Op::NOT_EQUALS:
      return 0.95;
    case execution::Op::LIKE:
      // No idea how to handle this   
      return 0.1;
  }
}

double StringHistogram::AverageSelecitivty() {
  // TODO
  // what for, whats the semantics?
  return 1.0;
}

void StringHistogram::Dump() {
  for(int i = 0; i < N_BINS; i++){
    std::cout << (char)(i + FIRST_PRINTABLE) << "\t->\t" << bins[i] << '\n';
  }
}

};  // namespace komfydb::optimizer
