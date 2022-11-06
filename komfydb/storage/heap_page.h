#ifndef __HEAP_PAGE_H__
#define __HEAP_PAGE_H__

#include <stdint.h>

#include <vector>

#include "absl/synchronization/mutex.h"

#include "komfydb/common/tuple.h"
#include "komfydb/common/tuple_desc.h"
#include "komfydb/storage/page.h"
#include "komfydb/storage/page_id.h"
#include "komfydb/transaction/transaction_id.h"

namespace {

using komfydb::common::Tuple;
using komfydb::common::TupleDesc;
using komfydb::transaction::TransactionId;

};  //namespace

namespace komfydb::storage {

class HeapPage : public Page {
  friend class HeapPageFactory;

 private:
  PageId pid;
  TupleDesc* td;
  std::vector<uint8_t> header;
  std::vector<Tuple> tuples;
  int num_slots;  // TODO I don't like this name ; do we even need this?
  std::vector<uint8_t> old_data;
  // Take a look on absl::MutexLock to see how to acquire it
  absl::Mutex old_data_lock;

  HeapPage(PageId pid, TupleDesc* td, std::vector<uint8_t> header,
           std::vector<Tuple> tuples, int num_slots)
      : pid(pid),
        td(td),
        header(header),
        tuples(tuples),
        num_slots(num_slots){};

 public:
  ~HeapPage() override {}

  static absl::StatusOr<std::unique_ptr<HeapPage>> Create(
      PageId id, TupleDesc* td, std::vector<uint8_t>& data);

  PageId GetId() override;

  // not nessecary for lab1
  // std::optional<TransactionId> DirtiedBy() override;

  // void MarkDirty(bool dirty, TransactionId tid) override;

  absl::Status SetBeforeImage() override;

  absl::StatusOr<std::unique_ptr<Page>> GetBeforeImage() override;

  absl::StatusOr<std::vector<uint8_t>> GetPageData() override;
};

};  // namespace komfydb::storage

#endif  // __HEAP_PAGE_H__
