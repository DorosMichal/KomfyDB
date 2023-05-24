#include "komfydb/execution/join.h"

namespace {

using komfydb::common::TupleDesc;
using komfydb::execution::JoinPredicate;
using komfydb::storage::PageId;
using komfydb::storage::RecordId;

}  // namespace

namespace komfydb::execution {

Join::Join(TupleDesc& tuple_desc, std::vector<std::string> fields_table_ids,
           JoinPredicate join_predicate, std::unique_ptr<OpIterator> l_child,
           std::unique_ptr<OpIterator> r_child)
    : OpIterator(tuple_desc, fields_table_ids),
      join_predicate(join_predicate),
      l_child(std::move(l_child)),
      r_child(std::move(r_child)),
      joined_record_id(RecordId(PageId(0, 0), -1)) {}

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

void Join::Close() {
  l_child->Close();
  r_child->Close();
}

};  // namespace komfydb::execution