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
  virtual absl::Status Open();

  virtual void Close();

  virtual absl::StatusOr<bool> HasNext();

  virtual absl::StatusOr<Record> Next();

  virtual absl::StatusOr<TupleDesc*> GetTupleDesc();
};

};  // namespace komfydb::execution

#endif  // __OP_ITERATOR_H__
