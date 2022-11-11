#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/execution/seq_scan.h"

namespace komfydb::execution {

SeqScan::SeqScan(TransactionId tid, int table_id,
                 std::shared_ptr<Catalog> catalog,
                 std::shared_ptr<BufferPool> bufferpool,
                 absl::string_view table_alias)
    : tid(tid),
      table_id(table_id),
      catalog(catalog),
      bufferpool(bufferpool),
      table_alias(table_alias) {}

SeqScan::SeqScan(TransactionId tid, int table_id,
                 std::shared_ptr<Catalog> catalog,
                 std::shared_ptr<BufferPool> bufferpool) {
  SeqScan(tid, table_id, catalog, bufferpool, common::GenerateUuidV4());
}

std::string SeqScan::GetTableName() {
    ASSIGN_OR_RETURN(std::string table_name, catalog->GetTableName(table_id);
    return table_name;
}

std::string SeqScan::GetAlias() {
  return table_alias;
}

absl::Status SeqScan::Open() {
  TableIterator iterator(tid, table_id, );
  iterator.Open();
  return absl::OkStatus();
}

void SeqScan::Close() {
  iterator.Close();
}

absl::StatusOr<bool> SeqScan::HasNext() {
  return iterator.HasNext();
}

absl::StatusOr<Record> SeqScan::Next() {
  return iterator.Next();
}

absl::StatusOr<TupleDesc*> SeqScan::GetTupleDesc() {
  return catalog->GetTupleDesc(table_id);
}

};  // namespace komfydb::execution