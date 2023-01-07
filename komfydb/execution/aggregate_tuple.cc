#include "komfydb/execution/aggregate_tuple.h"
#include "komfydb/common/field.h"
#include "komfydb/common/int_field.h"
#include "komfydb/common/string_field.h"
#include "komfydb/utils/status_macros.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace {

using komfydb::common::Field;
using komfydb::common::IntField;
using komfydb::common::StringField;

}  // namespace

namespace komfydb::execution {

AggregateTuple::AggregateTuple(const TupleDesc* tuple_desc)
    : Tuple(tuple_desc), group_size(1) {}

AggregateTuple::AggregateTuple(const AggregateTuple& t)
    : Tuple(t), group_size(t.group_size) {}

int AggregateTuple::GetGroupSize() {
  return group_size;
}

void AggregateTuple::IncremetGroupSize() {
  group_size++;
}

absl::Status AggregateTuple::ApplyAggregate(AggregateType aggregate_type, int i,
                                            Field* new_field) {
  ASSIGN_OR_RETURN(Type type, tuple_desc->GetFieldType(i));
  switch (type.GetValue()) {
    case Type::INT: {
      IntField* old_int_field = static_cast<IntField*>(fields[i].get());
      IntField* new_int_field = static_cast<IntField*>(new_field);
      int old_value = old_int_field->GetValue();
      switch (aggregate_type) {
        case AggregateType::AVG:
        case AggregateType::SUM: {
          int new_value = new_int_field->GetValue();
          old_int_field->SetValue(old_value + new_value);
          break;
        }
        case AggregateType::MAX: {
          int new_value = new_int_field->GetValue();
          old_int_field->SetValue(std::max(old_value, new_value));
          break;
        }
        case AggregateType::MIN: {
          int new_value = new_int_field->GetValue();
          old_int_field->SetValue(std::min(old_value, new_value));
          break;
        }
        case AggregateType::COUNT: {
          old_int_field->SetValue(old_value + 1);
          break;
        }
      }
      break;
    }
    case Type::STRING: {
      StringField* old_string_field =
          static_cast<StringField*>(fields[i].get());
      StringField* new_string_field = static_cast<StringField*>(new_field);
      switch (aggregate_type) {
        case AggregateType::NONE: {
          break;
        }
        case AggregateType::MAX: {
          std::string new_value = new_string_field->GetValue();
          ASSIGN_OR_RETURN(bool replace, old_string_field->Compare(
                                             Op::LESS_THAN, new_string_field));
          if (replace) {
            old_string_field->SetValue(new_value);
          }
          break;
        }
        case AggregateType::MIN: {
          std::string new_value = new_string_field->GetValue();
          ASSIGN_OR_RETURN(
              bool replace,
              old_string_field->Compare(Op::GREATER_THAN, new_string_field));
          if (replace) {
            old_string_field->SetValue(new_value);
          }
          break;
        }
        default: {
          return absl::InvalidArgumentError(
              absl::StrCat("StringField doesn't support ",
                           Aggregator::AggregateTypeToString(aggregate_type)));
        }
      }
      break;
    }
  }
  return absl::OkStatus();
}

absl::Status AggregateTuple::FinalizeAggregates(
    std::vector<AggregateType> aggregate_types) {
  for (int i = 0; i < tuple_desc->Length(); i++) {
    ASSIGN_OR_RETURN(Field * field, GetField(i));
    switch (aggregate_types[i]) {
      case AggregateType::AVG: {
        IntField* int_field = static_cast<IntField*>(field);
        int old_value = int_field->GetValue();
        int_field->SetValue(old_value / group_size);
        break;
      }
      default: {
        break;
      }
    }
  }
}

}  // namespace komfydb::execution