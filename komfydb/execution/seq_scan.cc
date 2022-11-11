#include "komfydb/execution/seq_scan.h"

#include <memory>
#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/storage/table_iterator.h"
#include "komfydb/utils/utility.h"

namespace komfydb::execution {

SeqScan::SeqScan(TableIterator iterator, TransactionId tid,
                 absl::string_view table_alias, int table_id)
    : iterator(std::move(iterator)),
      tid(tid),
      table_alias(table_alias),
      table_id(table_id) {}

SeqScan::SeqScan(TableIterator iterator, TransactionId tid, int table_id)
    : iterator(std::move(iterator)),
      tid(tid),
      table_alias(common::GenerateUuidV4()),
      table_id(table_id) {}

absl::Status SeqScan::Open() {
  return iterator.Open();
}

void SeqScan::Close() {
  return iterator.Close();
}

std::string SeqScan::GetAlias() {
  return table_alias;
}

bool SeqScan::HasNext() {
  return iterator.HasNext();
}

absl::StatusOr<Record> SeqScan::Next() {
  return iterator.Next();
}

absl::StatusOr<TupleDesc*> SeqScan::GetTupleDesc() {
  return iterator.GetTupleDesc();
}

};  // namespace komfydb::execution
