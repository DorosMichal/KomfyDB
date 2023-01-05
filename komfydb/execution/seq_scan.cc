#include "komfydb/execution/seq_scan.h"

#include <memory>
#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/storage/table_iterator.h"
#include "komfydb/utils/status_macros.h"
#include "komfydb/utils/utility.h"

namespace komfydb::execution {

SeqScan::SeqScan(std::unique_ptr<TableIterator> iterator, TransactionId tid,
                 TupleDesc td, absl::string_view table_alias, int table_id)
    : iterator(std::move(iterator)),
      td(td),
      tid(tid),
      table_alias(table_alias),
      table_id(table_id) {}

SeqScan::SeqScan(std::unique_ptr<TableIterator> iterator, TransactionId tid,
                 TupleDesc td, int table_id)
    : iterator(std::move(iterator)),
      td(td),
      tid(tid),
      table_alias(common::GenerateUuidV4()),
      table_id(table_id) {}

absl::StatusOr<std::unique_ptr<SeqScan>> SeqScan::Create(
    std::unique_ptr<TableIterator> iterator, TransactionId tid, int table_id) {
  return Create(std::move(iterator), tid, common::GenerateUuidV4(), table_id);
}

absl::StatusOr<std::unique_ptr<SeqScan>> SeqScan::Create(
    std::unique_ptr<TableIterator> iterator, TransactionId tid,
    absl::string_view table_alias, int table_id) {
  ASSIGN_OR_RETURN(TupleDesc * td, iterator->GetTupleDesc());
  return std::unique_ptr<SeqScan>(
      new SeqScan(std::move(iterator), tid, *td, table_alias, table_id));
}

absl::Status SeqScan::Open() {
  return iterator->Open();
}

void SeqScan::Close() {
  iterator->Close();
}

std::string SeqScan::GetAlias() {
  return table_alias;
}

absl::StatusOr<Record> SeqScan::Next() {
  return iterator->Next();
}

TupleDesc* SeqScan::GetTupleDesc() {
  return &td;
}

};  // namespace komfydb::execution
