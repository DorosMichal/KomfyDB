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

using komfydb::storage::TableIterator;
using komfydb::transaction::TransactionId;

};  // namespace

namespace komfydb::execution {

class SeqScan : public OpIterator {
 public:
  static absl::StatusOr<std::unique_ptr<SeqScan>> Create(
      std::unique_ptr<TableIterator> iterator, TransactionId tid,
      absl::string_view table_alias, int table_id);

  static absl::StatusOr<std::unique_ptr<SeqScan>> Create(
      std::unique_ptr<TableIterator> iterator, TransactionId tid, int table_id);

  absl::Status Open() override;

  void Close() override;

  absl::Status Rewind() override;

  std::string GetAlias();

  void Explain(std::ostream& os, int indent = 0) override;

 private:
  SeqScan(std::unique_ptr<TableIterator> iterator, TransactionId tid,
          TupleDesc tuple_desc, absl::string_view table_alias);

  SeqScan(std::unique_ptr<TableIterator> iterator, TransactionId tid,
          TupleDesc tuple_desc);

  absl::Status FetchNext() override;

  std::unique_ptr<TableIterator> iterator;
  TransactionId tid;
  std::string table_alias;
};

};  // namespace komfydb::execution

#endif  // __SEQ_SCAN_H__
