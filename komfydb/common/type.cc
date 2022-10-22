#include "komfydb/common/type.h"

namespace komfydb::common {

Type::Type(const Value& value) {
  this->value = value;
}

int Type::GetLen() {
  if (value == Value::INT) {
    return INT_LEN;
  } else {
    // TODO check why this +4 is necessary?
    return STR_LEN + 4;
  }
}

Type::Value Type::GetValue() {
  return value;
}

Type::operator std::string() {
  if (value == Value::INT) {
    return "int";
  } else {
    return "str";
  }
}

};  // namespace komfydb::common
