#include "absl/status/statusor.h"

#include "komfydb/common/field.h"
#include "komfydb/common/tuple.h"

namespace komfydb::common {

Tuple::Tuple(const TupleDesc* td) : td(td) {
  fields.resize(td->Length());
}

const TupleDesc* Tuple::GetTupleDesc() {
  return td;
}

absl::StatusOr<Field*> Tuple::GetField(int i) {
  if (fields.size() <= i || i < 0) {
    return absl::InvalidArgumentError("Index out of range");
  }
  return fields[i];
}

absl::Status Tuple::SetField(int i, Field* f) {
  if (fields.size() <= i || i < 0) {
    return absl::InvalidArgumentError("Index out of range");
  }
  fields[i] = f;
  return absl::OkStatus();
}

Tuple::operator std::string() const {
  std::string res = "";
  for (int i = 0; i < fields.size() - 1; i++) {
    res += (std::string)*fields[i];
    res += " ";
  }
  res += (std::string)*fields.back();
  return res;
}

};  // namespace komfydb::common
