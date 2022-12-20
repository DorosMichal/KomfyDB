#ifndef __SELECT_NODE_H__
#define __SELECT_NODE_H__

#include "komfydb/common/column_ref.h"

namespace {

using komfydb::common::ColumnRef;

};

namespace komfydb::execution::logical_plan {

class SelectNode {
 public:
  ColumnRef ref;

  SelectNode(ColumnRef ref) : ref(ref) {}

  operator std::string() const { return static_cast<std::string>(ref); }
};

};  // namespace komfydb::execution::logical_plan

#endif  // __SELECT_LIST_NODE_H__
