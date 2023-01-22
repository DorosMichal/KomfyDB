#include "komfydb/execution/join_predicate.h"

namespace {
using komfydb::common::Field;
};

namespace komfydb::execution {
JoinPredicate::JoinPredicate(int l_field_idx, Op op, int r_field_idx)
    : l_field_idx(l_field_idx), op(op), r_field_idx(r_field_idx){};

bool JoinPredicate::Filter(Tuple const& l_tuple, Tuple const& r_tuple) {
  // we assume that if we got to this place all the values are correct
  Field* l_field = l_tuple.GetField(l_field_idx).value();
  Field* r_field = r_tuple.GetField(r_field_idx).value();
  return l_field->Compare(op, r_field).value();
}

int JoinPredicate::GetField1idx() {
  return l_field_idx;
}

int JoinPredicate::GetField2idx() {
  return r_field_idx;
}

Op JoinPredicate::GetOperator() {
  return op;
}

std::ostream& operator<<(std::ostream& os, const JoinPredicate& jp) {
  os << "[col " << jp.l_field_idx << "] " << jp.op << " [col " << jp.r_field_idx
     << "]";
  return os;
}

};  // namespace komfydb::execution
