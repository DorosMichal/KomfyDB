#include <string>

#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"

#include "komfydb/common/string_field.h"

namespace komfydb::common {

StringField::StringField(const absl::string_view& s) : value(s) {}

StringField::StringField(const StringField& s) : value(s.value) {}

void StringField::GetValue(std::string& s) const {
  s = value;
}

absl::StatusOr<bool> StringField::Compare(const Op& op, const Field* f) const {
  if (f->GetType() != GetType()) {
    return absl::InvalidArgumentError("Can't compare fields of different type");
  }

  std::string fs;
  f->GetValue(fs);
  int cmp_val = value.compare(fs);

  switch (op.value) {
    case Op::EQUALS:
      return cmp_val == 0;
    case Op::NOT_EQUALS:
      return cmp_val != 0;
    case Op::GREATER_THAN:
      return cmp_val > 0;
    case Op::GREATER_THAN_OR_EQ:
      return cmp_val >= 0;
    case Op::LESS_THAN:
      return cmp_val < 0;
    case Op::LESS_THAN_OR_EQ:
      return cmp_val <= 0;
    case Op::LIKE:
      return value.find(fs, 0) != std::string::npos;
    default:
      return absl::InvalidArgumentError("Unknown operator value");
  }
}

Type StringField::GetType() const {
  return Type::STRING;
}

StringField::operator std::string() const {
  return value;
}

bool StringField::operator==(const StringField& other) const {
  return value == other.value;
}

};  // namespace komfydb::common
