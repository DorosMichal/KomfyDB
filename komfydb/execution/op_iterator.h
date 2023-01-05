#ifndef __OP_ITERATOR_H__
#define __OP_ITERATOR_H__

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/common/tuple_desc.h"
#include "komfydb/storage/record.h"

namespace {

using komfydb::common::TupleDesc;
using komfydb::storage::Record;

};  // namespace

namespace komfydb::execution {

class OpIterator {
 public:
  OpIterator(TupleDesc& td);

  virtual ~OpIterator() {}

  virtual absl::Status Open() = 0;

  virtual void Close() = 0;

  virtual TupleDesc* GetTupleDesc();

  virtual absl::StatusOr<std::unique_ptr<Record>> Next();

  virtual bool HasNext();

 protected:
  TupleDesc td;
  std::unique_ptr<Record> next_record;

  virtual absl::StatusOr<std::unique_ptr<Record>> FetchNext() = 0;
};

};  // namespace komfydb::execution

#endif  // __OP_ITERATOR_H__
