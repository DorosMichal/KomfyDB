#ifndef __DELETE_H__
#define __DELETE_H__

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

class Delete : public OpIterator {
 public:
  static absl::StatusOr<std::unique_ptr<Delete>> Create(
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
  bool deleted;

  Delete(std::unique_ptr<OpIterator> child, uint32_t table_id,
         TupleDesc tuple_desc, std::shared_ptr<BufferPool> bufferpool,
         TransactionId tid);

  absl::Status FetchNext() override;
};

};  // namespace komfydb::execution

#endif