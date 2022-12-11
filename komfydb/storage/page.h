#ifndef __PAGE_H__
#define __PAGE_H__

#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/storage/page_id.h"
#include "komfydb/storage/record.h"
#include "komfydb/transaction/transaction_id.h"

namespace {

using komfydb::transaction::TransactionId;

};  //namespace

namespace komfydb::storage {

class Page {
 public:
  virtual ~Page(){};

  virtual PageId GetId() = 0;

  // not nessecary for lab1
  // virtual std::optional<TransactionId> DirtiedBy() = 0;

  // virtual void MarkDirty(bool dirty, TransactionId tid) = 0;

  virtual absl::StatusOr<std::vector<uint8_t>> GetPageData() = 0;

  virtual absl::StatusOr<std::unique_ptr<Page>> GetBeforeImage() = 0;

  virtual absl::Status SetBeforeImage() = 0;

  virtual std::vector<Record> GetRecords() = 0;

  virtual absl::Status AddTuple(Tuple& t) = 0;

  virtual absl::Status RemoveRecord(RecordId& id) = 0;
};

};  // namespace komfydb::storage

#endif  // __PAGE_H__
