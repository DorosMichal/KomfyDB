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
  virtual ~Page();

  virtual PageId GetId();

  virtual TransactionId* IsDirty();

  virtual void MarkDirty(bool dirty, TransactionId tid);

  virtual absl::StatusOr<std::vector<uint8_t>> GetPageData();

  virtual absl::StatusOr<std::unique_ptr<Page>> GetBeforeImage();

  virtual void SetBeforeImage();
};

};  // namespace komfydb::storage

#endif  // __PAGE_H__
