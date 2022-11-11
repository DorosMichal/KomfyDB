#ifndef __DB_FILE_H__
#define __DB_FILE_H__

#include <list>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "komfydb/common/tuple.h"
#include "komfydb/common/tuple_desc.h"
#include "komfydb/storage/page.h"
#include "komfydb/storage/page_id.h"
#include "komfydb/transaction/transaction_id.h"

namespace {

using komfydb::common::TupleDesc;

};  // namespace

namespace komfydb::storage {

class DbFile {
 public:
  virtual ~DbFile(){};

  virtual absl::StatusOr<std::unique_ptr<Page>> ReadPage(PageId id) = 0;

  // Not necessary for Lab 1
  // virtual absl::Status WritePage(Page* p?);

  // virtual absl::StatusOr<std::vector<Page*>> InsertTuple(TransactionId tid,
  //                                                     Tuple t);

  // virtual absl::StatusOr<std::vector<Page*>> DeleteTuple(TransactionId tid,
  //                                                     Tuple t);

  virtual uint32_t GetId() = 0;

  // TODO Shouldn't this return a const pointer?
  virtual TupleDesc* GetTupleDesc() = 0;

  virtual int GetNumPages() = 0;
};

};  // namespace komfydb::storage

#endif  // __DB_FILE_H__
