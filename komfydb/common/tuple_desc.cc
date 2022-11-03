#include <string>
#include <vector>

#include "absl/status/statusor.h"

#include "komfydb/common/tuple_desc.h"
#include "komfydb/common/type.h"
#include "komfydb/utils/utility.h"

namespace komfydb::common {

TupleDesc::TupleDesc(std::vector<Type>& types,
                     std::vector<std::string>& fields) {
  int sz = types.size();
  items.reserve(sz);

  // XXX This can throw a segfault if fields.sz != type.sz. This shouldn't
  // ever happen so probably we don't need to worry about it (and if this
  // happens then there's a bug anyway).
  for (int i = 0; i < sz; i++) {
    items.push_back(TDItem(types[i], fields[i]));
  }
}

TupleDesc::TupleDesc(std::vector<Type>& types) {
  int sz = types.size();
  items.reserve(sz);

  for (int i = 0; i < sz; i++) {
    items.push_back(TDItem(types[i], ""));
  }
}

TupleDesc::TupleDesc(const TupleDesc& td1, const TupleDesc& td2) {
  JoinVectors(td1.items, td2.items, items);
}

std::vector<TDItem> TupleDesc::GetItems() {
  return std::vector<TDItem>(items);
}

int TupleDesc::Length() const {
  return items.size();
}

absl::StatusOr<std::string> TupleDesc::GetFieldName(int i) const {
  if (i >= items.size() || i < 0) {
    return absl::InvalidArgumentError("Index out of range");
  }

  return items[i].field_name;
}

absl::StatusOr<Type> TupleDesc::GetFieldType(int i) const {
  if (i >= items.size() || i < 0) {
    return absl::InvalidArgumentError("Index out of range");
  }

  return items[i].field_type;
}

absl::StatusOr<int> TupleDesc::IndexForFieldName(
    const absl::string_view& name) const {
  for (int i = 0; i < items.size(); i++) {
    if (items[i].field_name == name) {
      return i;
    }
  }

  return absl::InvalidArgumentError("No field with given name");
}

int TupleDesc::GetSize() const {
  // TODO: maybe this can be faster? Just preprocess it and store in variable?
  // Is it worth?
  int sz = 0;
  for (auto item : items) {
    sz += item.field_type.GetLen();
  }

  return sz;
}

bool TupleDesc::operator==(const TupleDesc& td) const {
  if (td.items.size() != items.size()) {
    return false;
  }

  for (int i = 0; i < items.size(); i++) {
    if (td.items[i] != items[i]) {
      return false;
    }
  }

  return true;
}

TupleDesc::operator std::string() const {
  std::string res = "";
  for (int i = 0; i < items.size() - 1; i++) {
    res += (std::string)items[i] + ", ";
  }
  res += items.back();
  return res;
}

};  // namespace komfydb::common
