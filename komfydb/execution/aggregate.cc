#include "komfydb/execution/aggregate.h"
#include "komfydb/execution/aggregator.h"
#include "komfydb/utils/status_macros.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace {

typedef komfydb::execution::Aggregator::AggregateType AggregateType;
using komfydb::common::Type;

}  // namespace

namespace komfydb::execution {

Aggregate::Aggregate(std::unique_ptr<OpIterator> child,
                     AggregateType aggregate_type, int aggregate_field,
                     int groupby_field, TupleDesc& tuple_desc)
    : OpIterator(tuple_desc),
      child(std::move(child)),
      aggregate_type(aggregate_type),
      aggregate_field(aggregate_field),
      groupby_field(groupby_field) {}

absl::StatusOr<std::unique_ptr<Aggregate>> Create(
    std::unique_ptr<OpIterator> child, AggregateType aggregate_type,
    int aggregate_field, int groupby_field) {
  ASSIGN_OR_RETURN(Type aggregate_field_type,
                   child->GetTupleDesc()->GetFieldType(aggregate_field));
  if (aggregate_field_type == Type::STRING &&
      aggregate_type != AggregateType::COUNT) {
    return absl::InvalidArgumentError("Strings only support 'COUNT' aggregate");
  }

  std::vector<Type> types;
  std::vector<std::string> fields;
  if (groupby_field != AggregateType::NO_GROUPING) {
    ASSIGN_OR_RETURN(Type groupby_field_type,
                     child->GetTupleDesc()->GetFieldType(groupby_field));
    types = {groupby_field_type, Type::INT};
    ASSIGN_OR_RETURN(std::string aggregate_field_name,
                     child->GetTupleDesc()->GetFieldName(aggregate_field));
    fields = {aggregate_field_name,
              Aggregator::AggregateTypeToString(aggregate_type)};
  } else {
    types = {Type::INT};
    fields = {Aggregator::AggregateTypeToString(aggregate_type)};
  }
  TupleDesc tuple_desc(types, fields);

  return std::make_unique<Aggregate>(std::move(child), aggregate_type,
                                     aggregate_field, groupby_field,
                                     tuple_desc);
}

absl::Status Aggregate::Open() {
  child->Open();
}

void Aggregate::Close() {
  child->Close();
}

absl::Status Aggregate::FetchNext() {}

}  // namespace komfydb::execution