#ifndef __JOIN_PREDICATE_H__
#define __JOIN_PREDICATE_H__

#include "komfydb/common/tuple.h"
#include "komfydb/execution/op.h"

namespace {
using komfydb::common::Tuple;
};

namespace komfydb::execution {
class JoinPredicate {
 public:
  JoinPredicate(int l_field_idx, Op op, int r_field_idx);

  bool Filter(Tuple const& l_tuple, Tuple const& r_tuple);

  int GetField1idx();

  int GetField2idx();

  Op GetOperator();

 private:
  int l_field_idx;
  Op op;
  int r_field_idx;
};
};  // namespace komfydb::execution

#endif  // __JOIN_PREDICATE_H__