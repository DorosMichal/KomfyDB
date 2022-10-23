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
  const TupleDesc* td;

  std::vector<Field*> fields;

 public:
  Tuple(const TupleDesc* td);

  ~Tuple();

  const TupleDesc* GetTupleDesc();

  absl::StatusOr<Field*> GetField(int i);

  absl::Status SetField(int i, Field* f);

  operator std::string() const;

  // TODO(Iterator)
  // std::vector<Field> GetFields();

  // Wtf should this do? lol
  // void ResetTupleDesc(TupleDesc td);
};

};  // namespace komfydb::common

#endif  // __TUPLE_H__
