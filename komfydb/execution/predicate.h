#ifndef __PREDICATE_H__
#define __PREDICATE_H__

#include <iostream>
#include <string>

#include "komfydb/common/tuple.h"
#include "komfydb/execution/op.h"
#include "komfydb/storage/field.h"

namespace {

using komfydb::common::Tuple;
using komfydb::storage::Field;

};  // namespace

namespace komfydb::execution {

class Predicate {
 private:
  int field;
  Op op;
  Field operand;

 public:
  Predicate(int field, Op op, Field operand);

  int GetField();

  Op GetOp();

  Field GetOperand();

  bool Filter(const Tuple& t);

  operator std::string() const;
};

};  // namespace komfydb::execution

#endif  // __PREDICATE_H__
