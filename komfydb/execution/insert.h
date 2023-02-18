#ifndef __INSERT_H__
#define __INSERT_H__

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/common/tuple_desc.h"
#include "komfydb/execution/op_iterator.h"
#include "komfydb/storage/buffer_pool.h"
#include "komfydb/transaction/transaction_id.h"

namespace {

using komfydb::storage::BufferPool;
using komfydb::transaction::TransactionId;

};  // namespace

namespace komfydb::execution {

class Insert : public OpIterator {
  // This OpIterator always returns a single Record with
  // a single IntField describing the number of rows inserted
 public:
  static absl::StatusOr<std::unique_ptr<Insert>> Create(
      std::unique_ptr<OpIterator> child, uint32_t table_id,
      std::shared_ptr<BufferPool> bufferpool, TransactionId tid);

  absl::Status Open() override;

  void Close() override;

  absl::Status Rewind() override;

  void Explain(std::ostream& os, int indent = 0) override;

 private:
  std::unique_ptr<OpIterator> child;
  std::shared_ptr<BufferPool> bufferpool;
  TransactionId tid;
  uint32_t table_id;
  bool inserted;

  Insert(std::unique_ptr<OpIterator> child, uint32_t table_id,
         TupleDesc tuple_desc, std::shared_ptr<BufferPool> bufferpool,
         TransactionId tid);

  absl::Status FetchNext() override;
};

};  // namespace komfydb::execution

#endif  // __INSERT_H__
