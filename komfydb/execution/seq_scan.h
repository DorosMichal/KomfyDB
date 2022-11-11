#ifndef __SEQ_SCAN_H__
#define __SEQ_SCAN_H__

#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/common/tuple_desc.h"
#include "komfydb/execution/op_iterator.h"
#include "komfydb/storage/buffer_pool.h"
#include "komfydb/storage/catalog.h"
#include "komfydb/storage/record.h"
#include "komfydb/storage/table_iterator.h"
#include "komfydb/transaction/transaction_id.h"

namespace {

using komfydb::storage::Record;
using komfydb::storage::TableIterator;
using komfydb::transaction::TransactionId;

};  // namespace

namespace komfydb::execution {

class SeqScan : public OpIterator {
 public:
  SeqScan(TableIterator iterator, TransactionId tid,
          absl::string_view table_alias, int table_id);

  SeqScan(TableIterator iterator, TransactionId tid, int table_id);

  absl::Status Open() override;

  void Close() override;

  std::string GetAlias();

  bool HasNext() override;

  absl::StatusOr<Record> Next() override;

  absl::StatusOr<TupleDesc*> GetTupleDesc() override;

 private:
  TableIterator iterator;
  TransactionId tid;
  std::string table_alias;
  int table_id;
};

};  // namespace komfydb::execution

#endif  // __SEQ_SCAN_H__
