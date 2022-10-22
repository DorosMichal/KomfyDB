#ifndef __TD_ITEM_H__
#define __TD_ITEM_H__

#include <string>

#include "absl/strings/string_view.h"

#include "komfydb/common/type.h"

namespace komfydb::common {

class TDItem {
 public:
  TDItem(const Type& t, absl::string_view name);

  Type field_type;

  std::string field_name;

  operator std::string();
};

};  // namespace komfydb::common

#endif  // __TD_ITEM_H__
