#ifndef __TUPLE_DESC_H__
#define __TUPLE_DESC_H__

#include <string>
#include <vector>

#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"

#include "komfydb/common/td_item.h"

namespace komfydb::common {

class TupleDesc {
 public:
  TupleDesc() = default;

  TupleDesc(const std::vector<Type>& types,
            const std::vector<std::string>& fields);

  TupleDesc(const std::vector<Type>& types);

  TupleDesc(const TupleDesc& tuple_desc);

  TupleDesc(const TupleDesc& td1, const TupleDesc& td2);

  std::vector<TDItem> GetItems();

  int Length() const;

  absl::StatusOr<std::string> GetFieldName(int i) const;

  absl::StatusOr<Type> GetFieldType(int i) const;

  absl::StatusOr<int> IndexForFieldName(const absl::string_view& name) const;

  int GetSize() const;

  bool operator==(const TupleDesc& tuple_desc) const;

  bool operator!=(const TupleDesc& tuple_desc) const;

  // TODO(HashCode)
  // int HashCode();

  operator std::string() const;

 private:
  std::vector<TDItem> items;
};

};  // namespace komfydb::common

#endif  // __TUPLE_DESC_H__
