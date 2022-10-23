#include "absl/status/statusor.h"

#include "komfydb/common/field.h"
#include "komfydb/common/tuple.h"

namespace komfydb::common {

Tuple::Tuple(const TupleDesc* td) : td(td) {
  fields.resize(td->Length());
}

Tuple::~Tuple() {
  for (auto f: fields) {
    delete f;
  }
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

// TODO(Tuple) This is bad imho. Here we assume that f is allocated by the 
// caller and what if this is not the case? We need to be careful..
absl::Status Tuple::SetField(int i, Field* f) {
  if (fields.size() <= i || i < 0) {
    return absl::InvalidArgumentError("Index out of range");
  }
  if (fields[i]->GetType() != f->GetType()) {
    return absl::InvalidArgumentError("Fields differ in type");
  }
  delete fields[i];
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
