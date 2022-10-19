#ifndef __HEAP_PAGE_H__
#define __HEAP_PAGE_H__

#include <stdint.h>

#include <vector>

#include "absl/synchronization/mutex.h"

#include "komfydb/common/tuple.h"
#include "komfydb/common/tuple_desc.h"
#include "komfydb/storage/page.h"
#include "komfydb/storage/page_id.h"
#include "komfydb/storage/heap_page_id.h"
#include "komfydb/transaction/transaction_id.h"

namespace {

using komfydb::transaction::TransactionId;
using komfydb::common::TupleDesc;
using komfydb::common::Tuple;

};  //namespace

namespace komfydb::storage {

class HeapPage : Page {
  friend class HeapPageFactory;

 private:
  HeapPageId pid;
  TupleDesc td;
  std::vector<uint8_t> header;
  std::vector<Tuple> tuples;
  int num_slots;  // TODO I don't like this name
  std::vector<uint8_t> old_data;
  // Take a look on absl::MutexLock to see how to aquire it
  absl::Mutex old_data_lock;  

  HeapPage() = default;

 public:
  PageId GetId() override;

  TransactionId IsDirty() override;

  void MarkDirty(bool dirty, TransactionId tid) override;

  std::vector<bool> GetPageData() override;

  Page GetBeforeImage() override;

  void SetBeforeImage() override;
};

};

#endif  // __HEAP_PAGE_H__
