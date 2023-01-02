#ifndef __FILTER_H__
#define __FILTER_H__

#include "komfydb/common/tuple_desc.h"
#include "komfydb/execution/op_iterator.h"
#include "komfydb/execution/predicate.h"

namespace komfydb::execution {

class Filter : public OpIterator {
 public:
  static absl::StatusOr<std::unique_ptr<Filter>> Create(
      std::unique_ptr<OpIterator> child, Predicate predicate);

  Predicate* GetPredicate();

  absl::Status Open() override;

  void Close() override;

  bool HasNext() override;

  absl::StatusOr<Record> Next() override;

  TupleDesc* GetTupleDesc() override;

 private:
  std::unique_ptr<OpIterator> child;
  TupleDesc td;
  Predicate predicate;
  std::unique_ptr<Record> next_record;

  Filter(std::unique_ptr<OpIterator> child, Predicate predicate, TupleDesc& td);
};

}  // namespace komfydb::execution

#endif