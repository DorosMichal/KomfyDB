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

using komfydb::storage::BufferPool;
using komfydb::storage::Catalog;
using komfydb::storage::Record;
using komfydb::storage::TableIterator;
using komfydb::transaction::TransactionId;

};  // namespace

namespace komfydb::execution {

class SeqScan : OpIterator {
 public:
  SeqScan(TransactionId tid, int table_id, std::shared_ptr<Catalog> catalog,
          std::shared_ptr<BufferPool> bufferpool,
          absl::string_view table_alias);

  SeqScan(TransactionId tid, int table_id, std::shared_ptr<Catalog> catalog,
          std::shared_ptr<BufferPool> bufferpool);

  std::string GetTableName();

  std::string GetAlias();

  void Reset(int table_id, std::string table_alias);

  absl::Status Open() override;

  void Close() override;

  absl::StatusOr<bool> HasNext() override;

  absl::StatusOr<Record> Next() override;

  absl::StatusOr<TupleDesc*> GetTupleDesc() override;

 private:
  int table_id;
  std::string table_alias;
  TransactionId tid;
  TableIterator iterator;
  std::shared_ptr<Catalog> catalog;
  std::shared_ptr<BufferPool> bufferpool;
};

};  // namespace komfydb::execution

#endif  // __SEQ_SCAN_H__
