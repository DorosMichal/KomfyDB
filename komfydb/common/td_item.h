#ifndef __TD_ITEM_H__
#define __TD_ITEM_H__

#include <string>

#include "komfydb/common/type.h"

namespace komfydb::common {

class TDItem {
 public:
  TDItem(const Type& t, std::string& field_name);

  Type field_type;

  std::string field_name;

  operator std::string();
};

};  // namespace komfydb::common

#endif  // __TD_ITEM_H__
