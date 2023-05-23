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
    : OpIterator(tuple_desc, "__insert__"),
      child(std::move(child)),
      bufferpool(std::move(bufferpool)),
      tid(tid),
      table_id(table_id),
      inserted(false) {}

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
  inserted = false;
  return child->Rewind();
}

void Insert::Explain(std::ostream& os, int indent) {
  os << Indent(indent) << "-> Inserting into table " << table_id << "\n";
  child->Explain(os, indent + child_indent);
}

absl::Status Insert::FetchNext() {
  if (inserted) {
    return absl::OutOfRangeError(
        "Insert already run. Use rewind to run again.");
  }
  inserted = true;

  // XXX In order to call bufferpool only once we
  // gather (materialise) them using std::vector
  int count = 0;
  std::vector<std::unique_ptr<Tuple>> tuples;
  ITERATE_RECORDS(child, record) {
    tuples.push_back(std::move(*record));
    count++;
  }
  RETURN_IF_NOT_OOR(record.status());
  RETURN_IF_ERROR(bufferpool->InsertTuples(std::move(tuples), table_id, tid));
  next_record = std::make_unique<Record>(1, RecordId(PageId(0, 0), -1));
  next_record->SetField(0, std::make_unique<IntField>(count));
  return absl::OkStatus();
}

};  // namespace komfydb::execution
