#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/utils/status_macros.h"

#include "komfydb/common/tuple_desc.h"
#include "komfydb/execution/join.h"
#include "komfydb/execution/join_predicate.h"
#include "komfydb/storage/record.h"
#include "komfydb/utils/utility.h"

namespace {

using komfydb::common::JoinVectors;
using komfydb::common::TupleDesc;
using komfydb::storage::PageId;
using komfydb::storage::RecordId;

}  // namespace

namespace komfydb::execution {

Join::Join(std::unique_ptr<OpIterator> l_child, JoinPredicate join_predicate,
           std::unique_ptr<OpIterator> r_child, TupleDesc tuple_desc,
           JoinType type)
    : OpIterator(tuple_desc, JoinVectors(*l_child->GetFieldsTableAliases(),
                                         *r_child->GetFieldsTableAliases())),
      type(type),
      join_predicate(join_predicate),
      l_child(std::move(l_child)),
      r_child(std::move(r_child)),
      l_child_next(nullptr),
      joined_record_id(RecordId(PageId(0, 0), -1)) {}

absl::StatusOr<std::unique_ptr<Join>> Join::Create(
    std::unique_ptr<OpIterator> l_child, JoinPredicate join_predicate,
    std::unique_ptr<OpIterator> r_child, JoinType type) {
  TupleDesc tuple_desc(*l_child->GetTupleDesc(), *r_child->GetTupleDesc());
  return std::unique_ptr<Join>(new Join(std::move(l_child), join_predicate,
                                        std::move(r_child), tuple_desc, type));
}

JoinPredicate Join::GetJoinPredicate() {
  return join_predicate;
}

absl::StatusOr<std::string> Join::GetJoinField1Name() {
  int field_idx = join_predicate.GetField1idx();
  TupleDesc* tuple_desc = l_child->GetTupleDesc();
  return tuple_desc->GetFieldName(field_idx);
}

absl::StatusOr<std::string> Join::GetJoinField2Name() {
  int field_idx = join_predicate.GetField2idx();
  TupleDesc* tuple_desc = r_child->GetTupleDesc();
  return tuple_desc->GetFieldName(field_idx);
}

absl::Status Join::Open() {
  RETURN_IF_ERROR(l_child->Open());
  RETURN_IF_ERROR(r_child->Open());
  return absl::OkStatus();
}

void Join::Close() {
  l_child->Close();
  r_child->Close();
}

absl::Status Join::Rewind() {
  RETURN_IF_ERROR(l_child->Rewind());
  RETURN_IF_ERROR(r_child->Rewind());
  l_child_next = {};
  next_record = {};
  return absl::OkStatus();
}

absl::Status Join::FetchNext() {
  switch (type) {
    case JoinType::NESTED_LOOPS: {
      return FetchNextNestedLoops();
    }
    case JoinType::HASH: {
      return FetchNextHash();
    }
  }
}

absl::Status Join::FetchNextNestedLoops() {
  if (next_record)
    return absl::OkStatus();

  absl::Status status;

  while (!next_record) {
    if (!l_child_next) {
      if ((status = l_child->HasNext()).ok()) {
        l_child_next = *(l_child->Next());
      } else {
        return status;
      }
    }
    while ((status = r_child->HasNext()).ok()) {
      std::unique_ptr<Record> potential_match = *(r_child->Next());
      if (join_predicate.Filter(*l_child_next, *potential_match)) {
        next_record = std::make_unique<Record>(
            Record(Tuple(*l_child_next, std::move(*potential_match)),
                   joined_record_id));
        return absl::OkStatus();
      }
    }
    RETURN_IF_NOT_OOR(status);
    l_child_next = {};
    RETURN_IF_ERROR(r_child->Rewind());
    continue;
  }

  return absl::OkStatus();
}

absl::Status FetchNextHash() {}

void Join::Explain(std::ostream& os, int indent) {
  os << Indent(indent) << "-> Nested loops join: " << join_predicate << "\n";
  os << Indent(indent + td_indent) << "TD: " << tuple_desc << "\n";
  l_child->Explain(os, indent + child_indent);
  r_child->Explain(os, indent + child_indent);
}

}  // namespace komfydb::execution
