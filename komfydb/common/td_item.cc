#include <string>

#include "absl/strings/string_view.h"

#include "komfydb/common/td_item.h"

namespace komfydb::common {

TDItem::TDItem(const Type& t, absl::string_view name)
    : field_type(t), field_name(name) {}

TDItem::operator std::string() {
  return field_name + "(" + (std::string)field_type + ")";
}

};  // namespace komfydb::common
