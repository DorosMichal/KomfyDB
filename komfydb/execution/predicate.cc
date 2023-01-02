#include "komfydb/execution/predicate.h"

namespace {

using namespace komfydb::common;

}

Predicate::Predicate(Op op, int field_idx, std::unique_ptr<Field> operand)
    : op(op), field_idx(field_idx), operand(std::move(operand)) {}

Op Predicate::GetOp() {
  return op;
}

Field* Predicate::GetOperand() {
  return operand.get();
}

int Predicate::GetFieldIdx() {
  return field_idx;
}