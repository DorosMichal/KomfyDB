#include "komfydb/execution/op.h"

#include "absl/container/flat_hash_map.h"

namespace komfydb::execution {

void Op::Flip() {
  switch (value) {
    case GREATER_THAN:
      value = LESS_THAN;
      break;
    case GREATER_THAN_OR_EQ:
      value = LESS_THAN_OR_EQ;
      break;
    case LESS_THAN:
      value = GREATER_THAN;
      break;
    case LESS_THAN_OR_EQ:
      value = GREATER_THAN_OR_EQ;
    case EQUALS:
    case NOT_EQUALS:
    case LIKE:
      break;
  }
}

absl::StatusOr<Op> Op::StrToOp(std::string_view op) {
  static const absl::flat_hash_map<std::string, Op::Value> str_to_op = {
      {"=", Op::EQUALS},       {"==", Op::EQUALS},
      {">", Op::GREATER_THAN}, {">=", Op::GREATER_THAN_OR_EQ},
      {"<", Op::LESS_THAN},    {"<=", Op::LESS_THAN_OR_EQ},
      {"LIKE", Op::LIKE},      {"~", Op::LIKE},
      {"!=", Op::NOT_EQUALS},  {"<>", Op::NOT_EQUALS},
  };

  auto it = str_to_op.find(op);
  if (it == str_to_op.end()) {
    return absl::InvalidArgumentError(
        absl::StrCat(op, " is not a valid operator\n."));
  }
  return Op(it->second);
}

Op::operator std::string() const {
  static const absl::flat_hash_map<Op::Value, std::string> op_to_str = {
      {EQUALS, "="},           {GREATER_THAN, ">"},        {LESS_THAN, "<"},
      {LESS_THAN_OR_EQ, "<="}, {GREATER_THAN_OR_EQ, ">="}, {LIKE, "~"},
      {NOT_EQUALS, "!="},
  };

  assert(op_to_str.contains(value));
  return op_to_str.find(value)->second;
}

};  // namespace komfydb::execution
