#ifndef __TUPLE_H__
#define __TUPLE_H__

#include <memory>
#include <vector>

#include "absl/status/statusor.h"

#include "komfydb/common/field.h"
#include "komfydb/common/tuple_desc.h"

namespace komfydb::common {

// This class differs from it's java counterpart. It has no logic regarding
// storage and it shouldn't have. See storage/record.h, which is a class
// representing a Tuple in memory.
class Tuple {
 protected:
  const TupleDesc* tuple_desc;

  std::vector<std::unique_ptr<Field>> fields;

  void swap(Tuple& t);

 public:
  Tuple(const TupleDesc* tuple_desc);

  Tuple(const Tuple& t);

  Tuple(Tuple&& tuple) = default;

  Tuple(Tuple& t1, Tuple&& t2, TupleDesc* joined_td);

  Tuple& operator=(const Tuple& t);

  const TupleDesc* GetTupleDesc();

  absl::StatusOr<Field*> GetField(int i) const;

  absl::Status SetField(int i, std::unique_ptr<Field> f);

  operator std::string() const;

  bool operator==(const Tuple& t) const;

  bool operator!=(const Tuple& t) const;

  // TODO(Iterator)
  // std::vector<Field> GetFields();

  // Wtf should this do? lol
  // void ResetTupleDesc(TupleDesc tuple_desc);
};

};  // namespace komfydb::common

#endif  // __TUPLE_H__
