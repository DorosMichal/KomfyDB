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

#define WHILE_HAS_NEXT(child, status) \
  absl::Status status;                \
  while ((status = (child)->HasNext()).ok())

#define ITERATE_RECORDS(child, record)            \
  absl::StatusOr<std::unique_ptr<Record>> record; \
  while ((record = std::move((child)->Next())).ok())

#define RETURN_IF_NOT_OOR(status)      \
  do {                                 \
    if (!absl::IsOutOfRange(status)) { \
      return status;                   \
    }                                  \
  } while (0);

class OpIterator {
 public:
  OpIterator(TupleDesc& td);

  virtual ~OpIterator() {}

  virtual absl::Status Open() = 0;

  virtual void Close() = 0;

  TupleDesc* GetTupleDesc();

  absl::StatusOr<std::unique_ptr<Record>> Next();

  absl::Status HasNext();

 protected:
  TupleDesc td;
  std::unique_ptr<Record> next_record;

  virtual absl::Status FetchNext() = 0;
};

};  // namespace komfydb::execution

#endif  // __OP_ITERATOR_H__
