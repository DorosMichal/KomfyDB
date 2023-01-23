#include "komfydb/execution/insert.h"

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

Insert::Insert(std::unique_ptr<OpIterator> child, uint32_t table_id,
               TupleDesc tuple_desc, std::shared_ptr<BufferPool> bufferpool,
               TransactionId tid)
    : OpIterator(tuple_desc, *child->GetFieldsTableAliases()),
      child(std::move(child)),
      bufferpool(std::move(bufferpool)),
      tid(tid),
      table_id(table_id) {}

absl::StatusOr<std::unique_ptr<Insert>> Insert::Create(
    std::unique_ptr<OpIterator> child, uint32_t table_id,
    std::shared_ptr<BufferPool> bufferpool, TransactionId tid) {
  std::vector<Type> types = {Type::INT};
  std::vector<std::string> fields = {"inserted_tuples"};
  return std::unique_ptr<Insert>(new Insert(std::move(child), table_id,
                                            TupleDesc(types, fields),
                                            std::move(bufferpool), tid));
}

absl::Status Insert::Open() {
  return child->Open();
}

void Insert::Close() {
  child->Close();
}

absl::Status Insert::Rewind() {
  return child->Rewind();
}

void Insert::Explain(std::ostream& os, int indent) {
  os << Indent(indent) << "-> Inserting into table " << table_id << "\n";
  child->Explain(os, indent + child_indent);
}

absl::Status Insert::FetchNext() {
  int inserted = 0;
  // XXX In order to call bufferpool only once we
  // gather (materialise) them using std::vector
  std::vector<std::unique_ptr<Tuple>> tuples;
  ITERATE_RECORDS(child, record) {
    tuples.push_back(std::move(*record));
    inserted++;
  }
  RETURN_IF_NOT_OOR(record.status());
  if (inserted == 0) {
    return absl::OutOfRangeError("No more records in this OpIterator");
  }
  RETURN_IF_ERROR(bufferpool->InsertTuples(std::move(tuples), table_id, tid));
  next_record =
      std::make_unique<Record>(&tuple_desc, RecordId(PageId(0, 0), -1));
  RETURN_IF_ERROR(
      next_record->SetField(0, std::make_unique<IntField>(inserted)));
  return absl::OkStatus();
}

};  // namespace komfydb::execution
