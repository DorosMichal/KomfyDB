#include "komfydb/common/type.h"

namespace komfydb::common {

Type::Type(const Value& value) {
  this->value = value;
}

int Type::GetLen() {
  if (value == Value::INT) {
    return INT_LEN;
  } else {
    // TODO: When encoded, string has 4 additional bytes for the length on the
    // disk. However: this is not a place to compute this for sure.
    return STR_LEN + 4;
  }
}

Type::Value Type::GetValue() {
  return value;
}

Type::operator std::string() const {
  if (value == Value::INT) {
    return "int";
  } else {
    return "str";
  }
}

bool Type::operator==(const Type& t) const {
  return t.value == value;
}

bool Type::operator!=(const Type& t) const {
  return t.value != value;
}

};  // namespace komfydb::common
