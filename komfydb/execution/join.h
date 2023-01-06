#ifndef __JOIN_H__
#define __JOIN_H__

#include <optional>
#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/execution/join_predicate.h"
#include "komfydb/execution/op_iterator.h"
#include "komfydb/storage/record_id.h"

namespace {
using komfydb::storage::RecordId;
}

namespace komfydb::execution {
class Join : public OpIterator {
 public:
  static absl::StatusOr<std::unique_ptr<Join>> Create(
      std::unique_ptr<OpIterator> l_child, JoinPredicate join_predicate,
      std::unique_ptr<OpIterator> r_child);

  JoinPredicate GetJoinPredicate();

  absl::StatusOr<std::string> GetJoinField1Name();

  absl::StatusOr<std::string> GetJoinField2Name();

  absl::Status Open() override;

  void Close() override;

  absl::Status Rewind() override;

 private:
  Join(std::unique_ptr<OpIterator> l_child, JoinPredicate join_predicate,
       std::unique_ptr<OpIterator> r_child, TupleDesc tuple_desc);

  absl::Status FetchNext() override;

  JoinPredicate join_predicate;
  std::unique_ptr<OpIterator> l_child;
  std::unique_ptr<OpIterator> r_child;
  std::unique_ptr<Record> l_child_next;
  std::unique_ptr<Record> next_record;
  const RecordId joined_record_id;
};
};  // namespace komfydb::execution

#endif  // __JOIN_H__
