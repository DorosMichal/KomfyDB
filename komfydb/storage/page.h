#ifndef __PAGE_H__
#define __PAGE_H__

#include <vector>

#include "komfydb/storage/page_id.h"
#include "komfydb/transaction/transaction_id.h"

namespace {

using komfydb::transaction::TransactionId;

};  //namespace

namespace komfydb::storage {

class Page {
 public:
  virtual ~Page() = 0;

  virtual PageId GetId();

  // not nessecary for lab1
  // virtual std::optional<TransactionId> DirtiedBy();

  // virtual void MarkDirty(bool dirty, TransactionId tid);

  virtual absl::StatusOr<std::vector<uint8_t>> GetPageData();

  virtual absl::StatusOr<std::unique_ptr<Page>> GetBeforeImage();

  virtual absl::Status SetBeforeImage();
};

};  // namespace komfydb::storage

#endif  // __PAGE_H__
