#include "komfydb/execution/delete.h"

#include "komfydb/common/int_field.h"
#include "komfydb/common/type.h"
#include "komfydb/storage/buffer_pool.h"
#include "komfydb/storage/page_id.h"
#include "komfydb/storage/record.h"
#include "komfydb/storage/record_id.h"
#include "komfydb/utils/status_macros.h"

namespace {

using komfydb::common::IntField;
using komfydb::storage::BufferPool;
using komfydb::storage::PageId;
using komfydb::storage::Record;
using komfydb::storage::RecordId;
using komfydb::transaction::TransactionId;

};  // namespace

namespace komfydb::execution {

Delete::Delete(std::unique_ptr<OpIterator> child, uint32_t table_id,
               TupleDesc tuple_desc, std::shared_ptr<BufferPool> bufferpool,
               TransactionId tid)
    : OpIterator(tuple_desc, "__delete__"),
      child(std::move(child)),
      bufferpool(std::move(bufferpool)),
      tid(tid),
      table_id(table_id),
      deleted(false) {}

absl::StatusOr<std::unique_ptr<Delete>> Delete::Create(
    std::unique_ptr<OpIterator> child, uint32_t table_id,
    std::shared_ptr<BufferPool> bufferpool, TransactionId tid) {
  std::vector<Type> types = {Type::INT};
  std::vector<std::string> fields = {"deleted_tuples"};
  std::vector<std::string>* child_aliases = child->GetFieldsTableAliases();
  for (std::string alias : *child_aliases) {
    if (alias != (*child_aliases)[0]) {
      return absl::InvalidArgumentError(
          "Cannot delete tulples with fields from different tables.");
    }
  }
  return std::unique_ptr<Delete>(new Delete(std::move(child), table_id,
                                            TupleDesc(types, fields),
                                            std::move(bufferpool), tid));
}

absl::Status Delete::Open() {
  return child->Open();
}

void Delete::Close() {
  child->Close();
}

absl::Status Delete::Rewind() {
  return child->Rewind();
}

void Delete::Explain(std::ostream& os, int indent) {
  os << Indent(indent) << "-> Deleting from table " << table_id << "\n";
  child->Explain(os, indent + child_indent);
}

absl::Status Delete::FetchNext() {
  if (deleted) {
    return absl::OutOfRangeError(
        "Delete already run. Use rewind to run again.");
  }
  deleted = true;

  // XXX In order to call bufferpool only once we
  // gather (materialise) records using std::vector
  int count = 0;
  std::vector<RecordId> ids;
  ITERATE_RECORDS(child, record) {
    ids.push_back((*record)->GetId());
    count++;
  }
  RETURN_IF_NOT_OOR(record.status());
  RETURN_IF_ERROR(bufferpool->RemoveTuples(ids, table_id, tid));
  next_record = std::make_unique<Record>(1, RecordId(PageId(0, 0), -1));
  next_record->SetField(0, std::make_unique<IntField>(count));
  return absl::OkStatus();
}

};  // namespace komfydb::execution
