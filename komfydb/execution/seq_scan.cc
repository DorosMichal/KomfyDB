#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/common/tuple.h"
#include "komfydb/common/tuple_desc.h"
#include "komfydb/execution/op_iterator.h"
#include "komfydb/transaction/transaction_id.h"

namespace komfydb::execution {

SeqScan::SeqScan(TransactionId tid, int table_id, absl::string_view table_alias)
    : tid(tid), table_id(table_id), table_alias(table_alias) {}

SeqScan::SeqScan(TransactionId tid, int table_id) {
  SeqScan(tid, table_id, common::GenerateUuidV4());
}

std::string SeqScan::GetTableName() {
    ASSIGN_OR_RETURN(std::string table_name, catalog->GetTableName(table_id);
    return table_name;
}

std::string SeqScan::GetAlias() {
  return table_alias;
}

absl::Status SeqScan::Open() {
  TableIterator iterator(table_id);
  iterator.open();
  return absl::OkStatus();
}

void SeqScan::Close() {
  iterator.close();
}

absl::StatusOr<bool> SeqScan::HasNext() {
  return iterator.has_next();
}

absl::StatusOr<Tuple> SeqScan::Next() {
  return iterator.next();
}

absl::StatusOr<TupleDesc*> SeqScan::GetTupleDesc() {
  return catalog->GetTupleDesc(table_id);
}
};
}
;  // namespace komfydb::execution