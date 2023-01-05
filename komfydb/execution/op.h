#ifndef __OP_H__
#define __OP_H__

#include <stdint.h>
#include <string>

#include "absl/status/statusor.h"

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
  Value value;

  Op(Value value = EQUALS) : value(value) {}

  // Change '<' to '>', '<=' to '>=' and so on.
  void Flip();

  operator std::string() const;

  static absl::StatusOr<Op> StrToOp(std::string_view op);

  friend std::ostream& operator<<(std::ostream& os, const Op& op);
};

};  // namespace komfydb::execution

#endif  // __OP_H__
