#ifndef __TD_ITEM_H__
#define __TD_ITEM_H__

#include <string>

#include "komfydb/common/type.h"

namespace {
using komfydb::common::Type;
};

namespace komfydb::storage {

class TDItem {
 public:
  TDItem(const Type& t, std::string& field_name);

  Type field_type;

  std::string field_name;

  operator std::string();
};

};  // namespace komfydb::storage

#endif  // __TD_ITEM_H__
