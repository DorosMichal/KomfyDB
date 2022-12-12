#include <iostream>

#include "komfydb/common/int_field.h"

namespace komfydb::common {

IntField::IntField(int value) : value(value) {}

IntField::IntField(const IntField& f) : value(f.value) {}

void IntField::GetValue(int& i) const {
  i = value;
}

absl::StatusOr<bool> IntField::Compare(const Op& op, const Field* f) const {
  if (f->GetType() != GetType()) {
    return absl::InvalidArgumentError("Can't compare fields of different type");
  }

  int fv;
  f->GetValue(fv);

  switch (op.value) {
    case Op::EQUALS:
    case Op::LIKE:
      return value == fv;
    case Op::NOT_EQUALS:
      return value != fv;
    case Op::GREATER_THAN:
      return value > fv;
    case Op::GREATER_THAN_OR_EQ:
      return value >= fv;
    case Op::LESS_THAN:
      return value < fv;
    case Op::LESS_THAN_OR_EQ:
      return value <= fv;
    default:
      return absl::InvalidArgumentError("Unknown operator value");
  }
}

Type IntField::GetType() const {
  return Type::INT;
}

IntField::operator std::string() const {
  return std::to_string(value);
}

};  // namespace komfydb::common
