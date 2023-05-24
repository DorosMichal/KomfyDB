#ifndef __HASH_JOIN_H__
#define __HASH_JOIN_H__

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/execution/join.h"
#include "komfydb/execution/join_predicate.h"
#include "komfydb/storage/record_id.h"
#include "komfydb/utils/utility.h"

namespace {

using komfydb::storage::RecordId;

};

namespace komfydb::execution {

using RecordVector = std::vector<std::unique_ptr<Record>>;

class HashJoin : public Join {
 public:
  static absl::StatusOr<std::unique_ptr<HashJoin>> Create(
      std::unique_ptr<OpIterator> l_child, JoinPredicate join_predicate,
      std::unique_ptr<OpIterator> r_child);

  absl::Status Open() override;

  absl::Status Rewind() override;

  void Explain(std::ostream& os, int indent = 0) override;

 private:
  HashJoin(std::unique_ptr<OpIterator> l_child, JoinPredicate join_predicate,
           std::unique_ptr<OpIterator> r_child, TupleDesc tuple_desc);

  absl::flat_hash_map<Tuple, RecordVector> map;
  absl::flat_hash_map<Tuple, RecordVector>::iterator current_vector;
  RecordVector::iterator current_match;
  std::unique_ptr<Record> r_child_next;

  void EmitRecord();
  absl::Status FetchNext() override;
};

};  // namespace komfydb::execution

#endif  // __HASH_JOIN_H__
