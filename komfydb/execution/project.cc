#include <vector>

#include "komfydb/common/td_item.h"
#include "komfydb/common/tuple_desc.h"
#include "komfydb/execution/op_iterator.h"
#include "komfydb/execution/project.h"
#include "komfydb/utils/status_macros.h"

namespace {

using namespace komfydb::common;

};  // namespace

namespace komfydb::execution {

Project::Project(std::unique_ptr<OpIterator> child,
                 std::vector<int>& out_field_idxs, TupleDesc tuple_desc)
    : OpIterator(tuple_desc, *child->GetFieldsTableAliases()),
      child(std::move(child)),
      out_field_idxs(out_field_idxs) {}

absl::StatusOr<std::unique_ptr<Project>> Project::Create(
    std::unique_ptr<OpIterator> child, std::vector<int>& out_field_idxs) {
  TupleDesc* childTD = child->GetTupleDesc();
  std::vector<TDItem> new_items;
  new_items.reserve(out_field_idxs.size());
  for (auto idx : out_field_idxs) {
    ASSIGN_OR_RETURN(TDItem item, childTD->GetItem(idx));
    new_items.push_back(item);
  }
  return std::unique_ptr<Project>(
      new Project(std::move(child), out_field_idxs, TupleDesc(new_items)));
}

absl::Status Project::Open() {
  RETURN_IF_ERROR(child->Open());
  return absl::OkStatus();
}

void Project::Close() {
  child->Close();
}

absl::Status Project::Rewind() {
  return child->Rewind();
}

absl::Status Project::FetchNext() {
  ASSIGN_OR_RETURN(std::unique_ptr<Record> record, child->Next());
  int num_of_fields = tuple_desc.Length();
  Tuple new_tuple(num_of_fields);
  for (int i = 0; i < num_of_fields; i++) {
    ASSIGN_OR_RETURN(Field * record_field, record->GetField(out_field_idxs[i]));
    if (record_field == nullptr) {
      continue;
    }
    switch (tuple_desc.GetFieldType(i).value().GetValue()) {
      case Type::INT:
        RETURN_IF_ERROR(
            new_tuple.SetField(i, std::make_unique<IntField>(
                                      *dynamic_cast<IntField*>(record_field))));
        break;
      case Type::STRING:
        RETURN_IF_ERROR(new_tuple.SetField(
            i, std::make_unique<StringField>(
                   *dynamic_cast<StringField*>(record_field))));
        break;
    }
  }
  next_record = std::make_unique<Record>(new_tuple, record->GetId());
  return absl::OkStatus();
}

void Project::Explain(std::ostream& os, int indent) {
  os << Indent(indent) << "-> Project over fields: ";
  for (auto& idx : out_field_idxs) {
    os << idx << " ";
  }
  os << "\n" << Indent(indent + td_indent) << "TD: " << tuple_desc << "\n";
  child->Explain(os, indent + child_indent);
}

};  // namespace komfydb::execution
