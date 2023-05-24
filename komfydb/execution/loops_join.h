#ifndef __LOOPS_JOIN_H__
#define __LOOPS_JOIN_H__

#include <optional>
#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/execution/join.h"
#include "komfydb/execution/join_predicate.h"
#include "komfydb/execution/op_iterator.h"

namespace komfydb::execution {

class LoopsJoin : public Join {
 public:
  static absl::StatusOr<std::unique_ptr<LoopsJoin>> Create(
      std::unique_ptr<OpIterator> l_child, JoinPredicate join_predicate,
      std::unique_ptr<OpIterator> r_child);

  JoinPredicate GetJoinPredicate();

  absl::StatusOr<std::string> GetJoinField1Name();

  absl::StatusOr<std::string> GetJoinField2Name();

  absl::Status Open() override;

  absl::Status Rewind() override;

  void Explain(std::ostream& os, int indent = 0) override;

 private:
  LoopsJoin(std::unique_ptr<OpIterator> l_child, JoinPredicate join_predicate,
            std::unique_ptr<OpIterator> r_child, TupleDesc tuple_desc);

  absl::Status FetchNext() override;

  std::unique_ptr<Record> l_child_next;
};

};  // namespace komfydb::execution

#endif  // __LOOPS_JOIN_H__
