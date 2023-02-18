#include "komfydb/execution/aggregate.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "glog/logging.h"

#include "komfydb/common/field.h"
#include "komfydb/execution/aggregate_tuple.h"
#include "komfydb/execution/aggregator.h"
#include "komfydb/execution/op.h"
#include "komfydb/storage/page_id.h"
#include "komfydb/storage/record_id.h"
#include "komfydb/utils/status_macros.h"

namespace {

typedef komfydb::execution::Aggregator::AggregateType AggregateType;
using komfydb::common::Field;
using komfydb::common::IntField;
using komfydb::common::StringField;
using komfydb::common::Type;
using komfydb::execution::AggregateTuple;
using komfydb::storage::PageId;
using komfydb::storage::RecordId;

absl::Status FillGroupIdFields(Tuple& group_id,
                               std::vector<int>& groupby_fields,
                               std::vector<Type>& groupby_types,
                               Record* record) {
  for (int i = 0; i < groupby_fields.size(); i++) {
    ASSIGN_OR_RETURN(Field * field, record->GetField(groupby_fields[i]));
    RETURN_IF_ERROR(group_id.SetField(i, field->CreateCopy()));
  }
  return absl::OkStatus();
}

absl::Status UpdateGroup(AggregateTuple* group,
                         std::vector<int>& aggregate_fields,
                         std::vector<AggregateType>& aggregate_types,
                         Record* record) {
  for (int i = 0; i < aggregate_fields.size(); i++) {
    ASSIGN_OR_RETURN(Field * next_field, record->GetField(aggregate_fields[i]));
    RETURN_IF_ERROR(group->ApplyAggregate(aggregate_types[i], i, next_field));
  }
  group->IncremetGroupSize();
  return absl::OkStatus();
}

absl::Status InitializeGroup(AggregateTuple* group,
                             std::vector<int>& aggregate_fields,
                             std::vector<AggregateType>& aggregate_types,
                             Record* record) {
  for (int i = 0; i < aggregate_fields.size(); i++) {
    switch (aggregate_types[i]) {
      case AggregateType::COUNT: {
        RETURN_IF_ERROR(group->SetField(i, std::make_unique<IntField>(1)));
        break;
      }
      default: {
        ASSIGN_OR_RETURN(std::unique_ptr<Field> field,
                         record->ReleaseField(aggregate_fields[i]));
        RETURN_IF_ERROR(group->SetField(i, std::move(field)));
        break;
      }
    }
  }
  return absl::OkStatus();
}

}  // namespace

namespace komfydb::execution {

Aggregate::Aggregate(std::unique_ptr<OpIterator> child,
                     std::vector<Aggregator::AggregateType>& aggregate_types,
                     std::vector<int>& aggregate_fields,
                     std::vector<int>& groupby_fields, TupleDesc& tuple_desc,
                     std::vector<std::string> aliases)
    : OpIterator(tuple_desc, aliases),
      child(std::move(child)),
      aggregate_types(aggregate_types),
      aggregate_fields(aggregate_fields),
      groupby_fields(groupby_fields) {}

absl::StatusOr<std::unique_ptr<Aggregate>> Aggregate::Create(
    std::unique_ptr<OpIterator> child,
    std::vector<Aggregator::AggregateType>& aggregate_types,
    std::vector<int>& aggregate_fields, std::vector<int>& groupby_fields) {
  TupleDesc* child_tuple_desc = child->GetTupleDesc();
  std::vector<std::string>* child_aliases = child->GetFieldsTableAliases();
  std::vector<Type> types;
  std::vector<std::string> fields;
  std::vector<std::string> aliases;
  for (int i = 0; i < aggregate_types.size(); i++) {
    AggregateType aggregate_type = aggregate_types[i];
    Type type(Type::INT);
    std::string name, alias;
    if (aggregate_type == AggregateType::NONE) {
      ASSIGN_OR_RETURN(type,
                       child_tuple_desc->GetFieldType(aggregate_fields[i]));
      ASSIGN_OR_RETURN(name,
                       child_tuple_desc->GetFieldName(aggregate_fields[i]));
      alias = (*child_aliases)[aggregate_fields[i]];
    } else {
      ASSIGN_OR_RETURN(std::string field_name,
                       child_tuple_desc->GetFieldName(aggregate_fields[i]));
      name = absl::StrCat(Aggregator::AggregateTypeToString(aggregate_type),
                          "__", field_name);
      // This field comes from aggregation, so it does not come from any
      // table. Let the alias for this be "__aggregate__".
      alias = "__aggregate__";
    }
    types.push_back(type);
    fields.push_back(name);
    aliases.push_back(alias);
  }
  TupleDesc tuple_desc(types, fields);

  return std::unique_ptr<Aggregate>(
      new Aggregate(std::move(child), aggregate_types, aggregate_fields,
                    groupby_fields, tuple_desc, aliases));
}

absl::Status Aggregate::PrepareWithGrouping() {
  TupleDesc* child_tuple_desc = child->GetTupleDesc();
  std::vector<Type> groupby_types;
  for (int i = 0; i < groupby_fields.size(); i++) {
    ASSIGN_OR_RETURN(Type type,
                     child_tuple_desc->GetFieldType(groupby_fields[i]));
    groupby_types.push_back(type);
  }
  absl::flat_hash_map<Tuple, AggregateTuple> map;
  TupleDesc groupby_tuple_desc(groupby_types);
  Tuple group_id(groupby_tuple_desc.Length());

  ITERATE_RECORDS(child, rec) {
    std::unique_ptr<Record> record = std::move(*rec);
    RETURN_IF_ERROR(FillGroupIdFields(group_id, groupby_fields, groupby_types,
                                      record.get()));
    if (map.contains(group_id)) {
      RETURN_IF_ERROR(UpdateGroup(&map.at(group_id), aggregate_fields,
                                  aggregate_types, record.get()));
    } else {
      AggregateTuple new_group(tuple_desc.Length());
      RETURN_IF_ERROR(InitializeGroup(&new_group, aggregate_fields,
                                      aggregate_types, record.get()));
      map.insert({group_id, new_group});
    }
  }
  RETURN_IF_NOT_OOR(rec.status());
  for (auto it = map.begin(); it != map.end(); it++) {
    RETURN_IF_ERROR(it->second.FinalizeAggregates(aggregate_types));
    out_tuples.push_back(it->second);
  }
  out_tuples_it = out_tuples.begin();
  return absl::OkStatus();
}

absl::Status Aggregate::PrepareNoGrouping() {
  AggregateTuple result(tuple_desc.Length());
  bool initialized = false;
  ITERATE_RECORDS(child, rec) {
    std::unique_ptr<Record> record = std::move(*rec);
    if (initialized) {
      RETURN_IF_ERROR(UpdateGroup(&result, aggregate_fields, aggregate_types,
                                  record.get()));
    } else {
      RETURN_IF_ERROR(InitializeGroup(&result, aggregate_fields,
                                      aggregate_types, record.get()));
      initialized = true;
    }
  }
  RETURN_IF_NOT_OOR(rec.status());
  out_tuples.push_back(result);
  out_tuples_it = out_tuples.begin();
  return absl::OkStatus();
}

absl::Status Aggregate::Prepare() {
  if (groupby_fields == Aggregator::NO_GROUPING) {
    return PrepareNoGrouping();
  }
  return PrepareWithGrouping();
}

absl::Status Aggregate::Open() {
  RETURN_IF_ERROR(child->Open());
  return Prepare();
}

void Aggregate::Close() {
  child->Close();
}

absl::Status Aggregate::FetchNext() {
  if (out_tuples_it == out_tuples.end()) {
    return absl::OutOfRangeError("No more records in this OpIterator");
  }
  next_record = std::make_unique<Record>(std::move(*out_tuples_it++),
                                         RecordId(PageId(0, 0), -1));
  return absl::OkStatus();
}

absl::Status Aggregate::Rewind() {
  RETURN_IF_ERROR(child->Rewind());
  out_tuples.clear();
  next_record = {};
  return Prepare();
}

void Aggregate::Explain(std::ostream& os, int indent) {
  os << Indent(indent) << "-> Aggregate: ";
  for (int i = 0; i < aggregate_fields.size(); i++) {
    if (i != 0) {
      os << ", ";
    }
    if (aggregate_types[i] == Aggregator::NONE) {
      os << "[col " << aggregate_fields[i] << "]";
    } else {
      os << Aggregator::AggregateTypeToString(aggregate_types[i]) << "([col "
         << aggregate_fields[i] << "])";
    }
  }
  os << "\n";
  child->Explain(os, indent + child_indent);
}

};  // namespace komfydb::execution
