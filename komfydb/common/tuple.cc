#include "absl/status/statusor.h"

#include "komfydb/common/field.h"
#include "komfydb/common/int_field.h"
#include "komfydb/common/string_field.h"
#include "komfydb/common/tuple.h"

namespace {

using komfydb::common::Field;

template <typename T>
bool compare_fields(const Field* f1, const Field* f2) {
  T v1, v2;
  if ((f1 == nullptr && f2 != nullptr) || (f1 != nullptr && f2 == nullptr)) {
    return false;
  }
  if (f1 == nullptr && f2 == nullptr) {
    return true;
  }
  f1->GetValue(v1);
  f2->GetValue(v2);

  return v1 == v2;
}

};  // namespace

namespace komfydb::common {

void Tuple::swap(Tuple& t) {
  tuple_desc = t.tuple_desc;
  fields = std::move(t.fields);
}

Tuple& Tuple::operator=(const Tuple& t) {
  Tuple tmp(t);
  swap(tmp);
  return *this;
}

Tuple::Tuple(const TupleDesc* tuple_desc) : tuple_desc(tuple_desc) {
  if (tuple_desc) {
    fields.resize(tuple_desc->Length());
  }
}

Tuple::Tuple(const Tuple& t) {
  tuple_desc = t.tuple_desc;
  fields.resize(tuple_desc->Length());
  for (int i = 0; i < tuple_desc->Length(); i++) {
    if (t.fields[i] == nullptr) {
      continue;
    }
    switch (tuple_desc->GetFieldType(i).value().GetValue()) {
      case Type::INT:
        fields[i] = std::make_unique<IntField>(
            *dynamic_cast<IntField*>(t.fields[i].get()));
        break;
      case Type::STRING:
        fields[i] = std::make_unique<StringField>(
            *dynamic_cast<StringField*>(t.fields[i].get()));
        break;
    }
  }
}

Tuple::Tuple(Tuple& t1, Tuple&& t2, TupleDesc* joined_td)
    : tuple_desc(joined_td) {
  // copies the 1st Tuple, but moves the 2nd, used in nested loop join
  fields.resize(tuple_desc->Length());
  Tuple tmp_t1 = t1;
  int idx = 0;
  for (int i = 0; i < tmp_t1.tuple_desc->Length(); i++, idx++) {
    fields[idx] = std::move(tmp_t1.fields[i]);
  }
  for (int i = 0; i < t2.tuple_desc->Length(); i++, idx++) {
    fields[idx] = std::move(t2.fields[i]);
  }
}

const TupleDesc* Tuple::GetTupleDesc() {
  return tuple_desc;
}

absl::StatusOr<Field*> Tuple::GetField(int i) const {
  if (fields.size() <= i || i < 0) {
    return absl::InvalidArgumentError("Index out of range");
  }
  if (fields[i] == nullptr) {
    return absl::InvalidArgumentError("Field not set yet.");
  }
  return fields[i].get();
}

// TODO(Tuple) This is bad imho. Here we assume that f is allocated by the
// caller and what if this is not the case? We need to be careful..
absl::Status Tuple::SetField(int i, std::unique_ptr<Field> f) {
  if (fields.size() <= i || i < 0) {
    return absl::InvalidArgumentError("Index out of range");
  }
  if (fields[i].get() && fields[i]->GetType() != f->GetType()) {
    return absl::InvalidArgumentError("Fields differ in type");
  }

  fields[i] = std::move(f);
  return absl::OkStatus();
}

Tuple::operator std::string() const {
  std::string res = "";
  for (int i = 0; i < fields.size() - 1; i++) {
    res += static_cast<std::string>(*fields[i]);
    res += " ";
  }
  res += static_cast<std::string>(*fields.back());
  return res;
}

bool Tuple::operator==(const Tuple& t) const {
  if (*t.tuple_desc != *tuple_desc) {
    return false;
  }

  for (int i = 0; i < t.fields.size(); i++) {
    switch (tuple_desc->GetFieldType(i).value().GetValue()) {
      case Type::INT:
        if (!compare_fields<int>(fields[i].get(), t.fields[i].get())) {
          return false;
        }
        break;
      case Type::STRING:
        if (!compare_fields<std::string>(fields[i].get(), t.fields[i].get())) {
          return false;
        }
        break;
    }
  }

  return true;
}

bool Tuple::operator!=(const Tuple& t) const {
  return !(*this == t);
}

};  // namespace komfydb::common
