#ifndef __PREDICATE_H__
#define __PREDICATE_H__

#include <iostream>
#include <string>

#include "komfydb/storage/field.h"
#include "komfydb/storage/tuple.h"

namespace {

using komfydb::storage::Field;
using komfydb::storage::Tuple;

};  // namespace

namespace komfydb::execution {

class Op {
 public:
  enum Value : uint8_t {
    EQUALS,
    GREATER_THAN,
    LESS_THAN,
    LESS_THAN_OR_EQ,
    GREATER_THAN_OR_EQ,
    LIKE,
    NOT_EQUALS,
  };

  operator std::string() const;

 private:
  Value value;
};

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
