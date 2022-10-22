#include <string>

#include "absl/strings/string_view.h"

#include "komfydb/common/td_item.h"

namespace komfydb::common {

TDItem::TDItem(const Type& t, absl::string_view name)
    : field_type(t), field_name(name) {}

TDItem::operator std::string() const {
  return field_name + "(" + (std::string)field_type + ")";
}

bool TDItem::operator==(const TDItem& tdi) const {
  return field_type == tdi.field_type && field_name == tdi.field_name;
}

bool TDItem::operator!=(const TDItem& tdi) const {
  return !(*this == tdi);
}

};  // namespace komfydb::common
