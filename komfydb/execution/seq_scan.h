#ifndef __SEQ_SCAN_H__
#define __SEQ_SCAN_H__

#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/common/tuple.h"
#include "komfydb/common/tuple_desc.h"
#include "komfydb/execution/op_iterator.h"
#include "komfydb/transaction/transaction_id.h"

namespace {

using komfydb::transaction::TransactionId;

};  // namespace

namespace komfydb::execution {

class SeqScan : OpIterator {
 public:
  SeqScan(TransactionId tid, int table_id, std::string table_alias);

  SeqScan(TransactionId tid, int table_id);

  std::string GetTableName();

  std::string GetAlias();

  void Reset(int table_id, std::string table_alias);

  absl::Status Open() override;

  void Close() override;

  absl::StatusOr<bool> HasNext() override;

  absl::StatusOr<Tuple> Next() override;

  TupleDesc GetTupleDesc() override;
};

};  // namespace komfydb::execution

#endif  // __SEQ_SCAN_H__
