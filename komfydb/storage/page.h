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

  virtual TransactionId GetLastTransaction() = 0;

  virtual bool IsDirty() = 0;

  virtual void SetDirty(bool dirty, TransactionId tid) = 0;

  virtual std::vector<uint8_t> GetPageData() = 0;

  virtual absl::StatusOr<std::unique_ptr<Page>> GetBeforeImage() = 0;

  virtual void SetBeforeImage() = 0;

  virtual std::vector<Record> GetRecords() = 0;

  virtual absl::Status AddTuples(std::unique_ptr<Tuple> tuples[], int num) = 0;

  virtual absl::Status RemoveRecord(RecordId& id) = 0;

  virtual int GetFreeSpace() = 0;
};

};  // namespace komfydb::storage

#endif  // __PAGE_H__
