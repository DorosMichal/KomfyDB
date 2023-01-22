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
                 TupleDesc tuple_desc, absl::string_view table_alias,
                 int table_id)
    : OpIterator(tuple_desc),
      iterator(std::move(iterator)),
      tid(tid),
      table_alias(table_alias),
      table_id(table_id) {}

SeqScan::SeqScan(std::unique_ptr<TableIterator> iterator, TransactionId tid,
                 TupleDesc tuple_desc, int table_id)
    : OpIterator(tuple_desc),
      iterator(std::move(iterator)),
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
  ASSIGN_OR_RETURN(TupleDesc * tuple_desc, iterator->GetTupleDesc());
  return std::unique_ptr<SeqScan>(new SeqScan(
      std::move(iterator), tid, *tuple_desc, table_alias, table_id));
}

void SeqScan::Explain(std::ostream& os, int indent) {
  os << Indent(indent) << "-> SeqScan over " << table_alias
     << " (physical name: " << iterator->GetTableName() << ")\n";
  os << Indent(indent + td_indent) << "TD: " << tuple_desc << "\n";
}

absl::Status SeqScan::Open() {
  return iterator->Open();
}

void SeqScan::Close() {
  iterator->Close();
}

absl::Status SeqScan::Rewind() {
  return iterator->Rewind();
}

std::string SeqScan::GetAlias() {
  return table_alias;
}

absl::Status SeqScan::FetchNext() {
  ASSIGN_OR_RETURN(Record result, iterator->Next());
  next_record = std::make_unique<Record>(result);
  return absl::OkStatus();
}

};  // namespace komfydb::execution
