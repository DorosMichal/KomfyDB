#ifndef __TUPLE_DESC_H__
#define __TUPLE_DESC_H__

#include <list>
#include <string>
#include <vector>

#include "absl/status/statusor.h"

#include "komfydb/common/td_item.h"

namespace komfydb::common {

class TupleDesc {
 public:
  TupleDesc(std::vector<Type> types, std::vector<std::string> fields);

  TupleDesc(std::vector<Type> types);

  TupleDesc(const TupleDesc& td1, const TupleDesc& td2);

  std::list<TDItem> GetItems();

  int Length();

  absl::StatusOr<std::string> GetFieldName(int i);

  absl::StatusOr<Type> GetFieldType(int i);

  absl::StatusOr<int> IndexForFieldName(std::string name);

  int GetSize();

  bool operator==(const TupleDesc& td) const;

  int HashCode();

  operator std::string() const;
};

};  // namespace komfydb::common

#endif  // __TUPLE_DESC_H__
