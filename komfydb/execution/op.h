#ifndef __OP_H__
#define __OP_H__

#include <stdint.h>
#include <string>

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

};  // namespace komfydb::execution

#endif  // __OP_H__
