#ifndef __HEAP_PAGE_H__
#define __HEAP_PAGE_H__

#include <stdint.h>

#include <vector>

#include "absl/synchronization/mutex.h"

#include "komfydb/common/tuple_desc.h"
#include "komfydb/storage/page.h"
#include "komfydb/storage/page_id.h"
#include "komfydb/storage/record.h"
#include "komfydb/transaction/transaction_id.h"

namespace {

using komfydb::common::TupleDesc;
using komfydb::storage::Record;

};  //namespace

namespace komfydb::storage {

class HeapPage : public Page {
  friend class HeapPageFactory;

 private:
  std::vector<uint8_t> header;
  std::vector<Record> records;
  std::vector<uint8_t> old_data;
  // Take a look on absl::MutexLock to see how to acquire it
  absl::Mutex old_data_lock;
  TransactionId last_transaction;
  TupleDesc* tuple_desc;
  PageId pid;
  int num_slots;
  bool is_dirty;

  HeapPage(PageId pid, TupleDesc* tuple_desc, std::vector<uint8_t> header,
           std::vector<Record> records, int num_slots);

 public:
  ~HeapPage() override {}

  static absl::StatusOr<std::unique_ptr<HeapPage>> Create(
      PageId id, TupleDesc* tuple_desc, std::vector<uint8_t>& data);

  PageId GetId() override;

  TransactionId GetLastTransaction() override;

  void SetDirty(bool dirty, TransactionId tid) override;

  bool IsDirty() override;

  absl::Status SetBeforeImage() override;

  absl::StatusOr<std::unique_ptr<Page>> GetBeforeImage() override;

  absl::StatusOr<std::vector<uint8_t>> GetPageData() override;

  std::vector<Record> GetRecords() override;

  absl::Status AddTuple(Tuple& t) override;

  absl::Status RemoveRecord(RecordId& id) override;
};

};  // namespace komfydb::storage

#endif  // __HEAP_PAGE_H__
