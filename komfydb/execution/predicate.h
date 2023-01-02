#ifndef __PREDICATE_H__
#define __PREDICATE_H__

#include "komfydb/common/field.h"
#include "komfydb/common/int_field.h"
#include "komfydb/common/string_field.h"
#include "komfydb/execution/op.h"

namespace {

using namespace komfydb::common;

}

class Predicate {
 public:
  Predicate(Op op, int field_idx, std::unique_ptr<Field> operand);

  Op GetOp();

  Field* GetOperand();

  int GetFieldIdx();

 private:
  Op op;
  int field_idx;
  std::unique_ptr<Field> operand;
};

#endif  // __PREDICATE_H__