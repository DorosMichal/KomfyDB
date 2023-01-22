#include <iostream>

#include "komfydb/common/int_field.h"

namespace komfydb::common {

IntField::IntField(int value) : value(value) {}

IntField::IntField(const IntField& f) : value(f.value) {}

int IntField::GetValue() const {
  return value;
}

void IntField::SetValue(int i) {
  value = i;
}

absl::StatusOr<bool> IntField::Compare(const Op& op, const Field* f) const {
  if (f->GetType() != GetType()) {
    return absl::InvalidArgumentError("Can't compare fields of different type");
  }

  int fv = static_cast<const IntField*>(f)->GetValue();

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

std::unique_ptr<Field> IntField::CreateCopy() const {
  return std::make_unique<IntField>(value);
}

bool IntField::operator==(const IntField& field) const {
  return value == field.value;
}

IntField::operator std::string() const {
  return absl::StrCat("IntField(", std::to_string(value), ")");
}

};  // namespace komfydb::common
