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
  virtual ~OpIterator() {}

  virtual absl::Status Open() = 0;

  virtual void Close() = 0;

  virtual bool HasNext() = 0;

  virtual absl::StatusOr<Record> Next() = 0;

  virtual TupleDesc* GetTupleDesc() = 0;
};

};  // namespace komfydb::execution

#endif  // __OP_ITERATOR_H__
