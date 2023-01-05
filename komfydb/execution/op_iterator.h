#ifndef __OP_ITERATOR_H__
#define __OP_ITERATOR_H__

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/common/column_ref.h"
#include "komfydb/common/tuple_desc.h"
#include "komfydb/storage/record.h"

namespace {

using komfydb::common::ColumnRef;
using komfydb::common::TupleDesc;
using komfydb::storage::Record;

};  // namespace

namespace komfydb::execution {

#define WHILE_HAS_NEXT(child, status) \
  absl::Status status;                \
  while ((status = (child)->HasNext()).ok())

#define ITERATE_RECORDS(child, record)            \
  absl::StatusOr<std::unique_ptr<Record>> record; \
  while ((record = (child)->Next()).ok())

#define RETURN_IF_NOT_OOR(status)      \
  do {                                 \
    if (!absl::IsOutOfRange(status)) { \
      return status;                   \
    }                                  \
  } while (0);

class OpIterator {
 public:
  OpIterator() = default;

  OpIterator(TupleDesc& tuple_desc, std::string_view alias);

  OpIterator(TupleDesc& tuple_desc, std::vector<std::string> fields_table_ids);

  virtual ~OpIterator() = default;

  virtual absl::Status Open() = 0;

  virtual void Close() = 0;

  virtual absl::Status Rewind() = 0;

  TupleDesc* GetTupleDesc();

  std::vector<std::string>* GetFieldsTableAliases();

  absl::StatusOr<int> GetIndexForColumnRef(ColumnRef col);

  absl::StatusOr<std::unique_ptr<Record>> Next();

  absl::Status HasNext();

  virtual void Explain(std::ostream& os, int indent = 0) = 0;

 protected:
  TupleDesc tuple_desc;
  // fields_table_ids[i] == table id for the ith field in the returned tuples.
  std::vector<std::string> fields_table_aliases;
  std::unique_ptr<Record> next_record;
  static const int child_indent = 2;
  static const int td_indent = 4;

  std::string Indent(int indent);

  virtual absl::Status FetchNext() = 0;
};

};  // namespace komfydb::execution

#endif  // __OP_ITERATOR_H__
