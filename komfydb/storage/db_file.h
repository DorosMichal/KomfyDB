#ifndef __DB_FILE_H__
#define __DB_FILE_H__

#include <list>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/common/tuple.h"
#include "komfydb/common/tuple_desc.h"
#include "komfydb/storage/db_file_iterator.h"
#include "komfydb/storage/page.h"
#include "komfydb/storage/page_id.h"
#include "komfydb/transaction/transaction_id.h"

namespace {

using komfydb::common::Tuple;
using komfydb::common::TupleDesc;
using komfydb::transaction::TransactionId;

};  // namespace

namespace komfydb::storage {

class DbFile {
 public:
  virtual ~DbFile() = 0;

  // TODO Shouldn't this return StatusOr?
  virtual std::unique_ptr<Page> ReadPage(PageId id);

  virtual absl::Status WritePage(Page p);

  virtual absl::StatusOr<std::list<Page>> InsertTuple(TransactionId tid,
                                                      Tuple t);

  virtual absl::StatusOr<std::list<Page>> DeleteTuple(TransactionId tid,
                                                      Tuple t);

  virtual DbFileIterator Iterator(TransactionId tid);

  virtual int GetId();

  virtual TupleDesc* GetTupleDesc();
};

};  // namespace komfydb::storage

#endif  // __DB_FILE_H__
