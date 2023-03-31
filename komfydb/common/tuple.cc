#include "absl/status/statusor.h"

#include "komfydb/common/field.h"
#include "komfydb/common/int_field.h"
#include "komfydb/common/string_field.h"
#include "komfydb/common/tuple.h"

namespace {

using komfydb::common::Field;
using komfydb::common::IntField;
using komfydb::common::StringField;

bool compare_fields(const IntField* f1, const IntField* f2) {
  if (f1 == f2) {
    return true;
  }
  if (f1 == nullptr || f2 == nullptr) {
    return false;
  }
  return f1->GetValue() == f2->GetValue();
}

bool compare_fields(const StringField* f1, const StringField* f2) {
  if (f1 == f2) {
    return true;
  }
  if (f1 == nullptr || f2 == nullptr) {
    return false;
  }
  return f1->GetValue() == f2->GetValue();
}

};  // namespace

namespace komfydb::common {

void Tuple::swap(Tuple& t) {
  fields = std::move(t.fields);
}

Tuple& Tuple::operator=(const Tuple& t) {
  Tuple tmp(t);
  swap(tmp);
  return *this;
}

Tuple::Tuple(int size) {
  fields.resize(size);
}

Tuple::Tuple(const Tuple& t) {
  fields.resize(t.Size());
  for (int i = 0; i < t.Size(); i++) {
    if (t.fields[i] == nullptr) {
      fields[i] = nullptr;
      continue;
    }
    switch (t.fields[i]->GetType().GetValue()) {
      case Type::INT:
        fields[i] = std::make_unique<IntField>(
            *static_cast<IntField*>(t.fields[i].get()));
        break;
      case Type::STRING:
        fields[i] = std::make_unique<StringField>(
            *static_cast<StringField*>(t.fields[i].get()));
        break;
    }
  }
}

Tuple::Tuple(Tuple& t1, Tuple&& t2) {
  // copies the 1st Tuple, but moves the 2nd, used in nested loop join
  fields.resize(t1.Size() + t2.Size());
  Tuple tmp_t1 = t1;
  int idx = 0;
  for (int i = 0; i < tmp_t1.Size(); i++, idx++) {
    fields[idx] = std::move(tmp_t1.fields[i]);
  }
  for (int i = 0; i < t2.Size(); i++, idx++) {
    fields[idx] = std::move(t2.fields[i]);
  }
}

Tuple::Tuple(Tuple& t1, Tuple& t2) {
  // copies both Tuples, used in hash join
  fields.resize(t1.Size() + t2.Size());
  Tuple tmp_t1 = t1, tmp_t2 = t2;
  int idx = 0;
  for (int i = 0; i < tmp_t1.Size(); i++, idx++) {
    fields[idx] = std::move(tmp_t1.fields[i]);
  }
  for (int i = 0; i < tmp_t2.Size(); i++, idx++) {
    fields[idx] = std::move(tmp_t2.fields[i]);
  }
}

int Tuple::Size() const {
  return fields.size();
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

absl::StatusOr<std::unique_ptr<Field>> Tuple::ReleaseField(int i) {
  if (fields.size() <= i || i < 0) {
    return absl::InvalidArgumentError("Index out of range");
  }
  if (fields[i] == nullptr) {
    return absl::InvalidArgumentError("Field not set yet.");
  }
  return std::move(fields[i]);
}

absl::Status Tuple::SetField(int i, std::unique_ptr<Field> f) {
  if (fields.size() <= i || i < 0) {
    return absl::InvalidArgumentError("Index out of range");
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

std::ostream& operator<<(std::ostream& os, const Tuple& tuple) {
  os << static_cast<std::string>(tuple);
  return os;
}

bool Tuple::operator==(const Tuple& t) const {
  if (Size() != t.Size()) {
    return false;
  }

  for (int i = 0; i < t.fields.size(); i++) {
    if (fields[i]->GetType() != t.fields[i]->GetType()) {
      return false;
    }
    switch (fields[i]->GetType().GetValue()) {
      case Type::INT: {
        if (!compare_fields(static_cast<const IntField*>(fields[i].get()),
                            static_cast<const IntField*>(t.fields[i].get()))) {
          return false;
        }
        break;
      }
      case Type::STRING: {
        if (!compare_fields(
                static_cast<const StringField*>(fields[i].get()),
                static_cast<const StringField*>(t.fields[i].get()))) {
          return false;
        }
        break;
      }
    }
  }

  return true;
}

bool Tuple::operator!=(const Tuple& t) const {
  return !(*this == t);
}

};  // namespace komfydb::common
