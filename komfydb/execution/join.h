#ifndef __JOIN_H__
#define __JOIN_H__

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/execution/join_predicate.h"
#include "komfydb/execution/op_iterator.h"
#include "komfydb/storage/page_id.h"
#include "komfydb/storage/record_id.h"

namespace {

using komfydb::common::TupleDesc;
using komfydb::execution::JoinPredicate;
using komfydb::storage::RecordId;

}  // namespace

namespace komfydb::execution {

class Join : public OpIterator {
 public:
  Join(TupleDesc& tuple_desc, std::vector<std::string> fields_table_ids,
       JoinPredicate join_predicate, std::unique_ptr<OpIterator> l_child,
       std::unique_ptr<OpIterator> r_child);

  JoinPredicate GetJoinPredicate();

  absl::StatusOr<std::string> GetJoinField1Name();

  absl::StatusOr<std::string> GetJoinField2Name();

  void Close() override;

  //   virtual ~Join() = default;

 protected:
  JoinPredicate join_predicate;
  std::unique_ptr<OpIterator> l_child;
  std::unique_ptr<OpIterator> r_child;
  const RecordId joined_record_id;
};

};  // namespace komfydb::execution

#endif  // __JOIN_H__