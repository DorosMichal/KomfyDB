#include "komfydb/common/field.h"
#include "komfydb/common/tuple.h"
#include "komfydb/execution/op.h"

namespace {
using komfydb::common::Field;
using komfydb::common::Tuple;
};  // namespace

namespace komfydb::execution {
class Predicate {
 public:
  Predicate(int field, Op op, Field operand);

  int getField();

  Op getOp();

  Field getOperand();

  bool filter(Tuple t);

  operator std::string() const;
};
};  // namespace komfydb::execution
