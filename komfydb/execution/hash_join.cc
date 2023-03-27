#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/utils/status_macros.h"

#include "komfydb/common/tuple_desc.h"
#include "komfydb/execution/hash_join.h"
#include "komfydb/execution/join_predicate.h"
#include "komfydb/storage/record.h"
#include "komfydb/utils/utility.h"

namespace {

using komfydb::common::Field;
using komfydb::common::TupleDesc;

}  // namespace

namespace komfydb::execution {

HashJoin::HashJoin(std::unique_ptr<OpIterator> l_child,
                   JoinPredicate join_predicate,
                   std::unique_ptr<OpIterator> r_child, TupleDesc tuple_desc)
    : Join(std::move(l_child), join_predicate, std::move(r_child), tuple_desc) {
}

absl::StatusOr<std::unique_ptr<HashJoin>> HashJoin::Create(
    std::unique_ptr<OpIterator> l_child, JoinPredicate join_predicate,
    std::unique_ptr<OpIterator> r_child) {
  switch (join_predicate.GetOperator().value) {
    // TODO: should 'LIKE' also be allowed?
    case Op::EQUALS: {
      break;
    }
    default: {
      return absl::InvalidArgumentError(
          "Hash join only works with equality conditions");
    }
  }
  TupleDesc tuple_desc(*l_child->GetTupleDesc(), *r_child->GetTupleDesc());
  return std::unique_ptr<HashJoin>(new HashJoin(
      std::move(l_child), join_predicate, std::move(r_child), tuple_desc));
}

absl::Status HashJoin::Open() {
  RETURN_IF_ERROR(l_child->Open());
  RETURN_IF_ERROR(r_child->Open());
  ITERATE_RECORDS(l_child, record) {
    Tuple key = Tuple(1);
    ASSIGN_OR_RETURN(Field * key_field,
                     (*record)->GetField(join_predicate.GetField1idx()));
    key.SetField(0, std::make_unique<Field>(*key_field));
    if (map.contains(key)) {
      map[key].push_back(std::move(*record));
    } else {
      map.insert(
          {key, std::vector<std::unique_ptr<Record>>({std::move(*record)})});
    }
  }
  RETURN_IF_NOT_OOR(record.status());
  return absl::OkStatus();
}

absl::Status HashJoin::Rewind() {
  RETURN_IF_ERROR(l_child->Rewind());
  RETURN_IF_ERROR(r_child->Rewind());
  r_child_next = nullptr;
  current_vector = nullptr;
  next_record = nullptr;
  return absl::OkStatus();
}

void HashJoin::Explain(std::ostream& os, int indent) {
  os << Indent(indent) << "-> Hash join: " << join_predicate << "\n";
  os << Indent(indent + td_indent) << "TD: " << tuple_desc << "\n";
  l_child->Explain(os, indent + child_indent);
  r_child->Explain(os, indent + child_indent);
}

absl::Status HashJoin::FetchNext() {
  if (r_child_next == nullptr) {
    ASSIGN_OR_RETURN(r_child_next, r_child->Next());
  }
  if (current_vector) {
    next_record = std::make_unique<Record>(
        Record(Tuple(*(*current_match++), std::move(*r_child_next)),
               joined_record_id));
    if (current_match == current_vector->end()) {
      current_vector = nullptr;
    }
    return absl::OkStatus();
  }
  Tuple key = Tuple(1);
  ASSIGN_OR_RETURN(Field * key_field,
                   r_child_next->GetField(join_predicate.GetField2idx()));
  key.SetField(0, std::make_unique<Field>(*key_field));
  if (map.contains(key)) {
    current_vector = &map.at(key);
    current_match = current_vector->begin();
    next_record = std::make_unique<Record>(
        Record(Tuple(*(*current_match++), std::move(*r_child_next)),
               joined_record_id));
    if (current_match == current_vector->end()) {
      current_vector = nullptr;
    }
    return absl::OkStatus();
  }
  return FetchNext();
}

}  // namespace komfydb::execution