#ifndef __SCAN_NODE_H__
#define __SCAN_NODE_H__

#include <string>

namespace komfydb::execution::logical_plan {

class ScanNode {
 public:
  // Table id in the catalog.
  int id;
  // Table alias in the query.
  std::string alias;

  ScanNode(int id, std::string_view alias) : id(id), alias(alias) {}

  operator std::string() const { return std::to_string(id) + ", " + alias; }
};

};  // namespace komfydb::execution::logical_plan

#endif  // __SCAN_NODE_H__
