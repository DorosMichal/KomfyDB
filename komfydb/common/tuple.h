#ifndef __TUPLE_H__
#define __TUPLE_H__

#include <memory>
#include <vector>

#include "absl/status/statusor.h"

#include "komfydb/common/field.h"
#include "komfydb/common/int_field.h"
#include "komfydb/common/string_field.h"
#include "komfydb/common/tuple_desc.h"

namespace komfydb::common {

// This class differs from it's java counterpart. It has no logic regarding
// storage and it shouldn't have. See storage/record.h, which is a class
// representing a Tuple in memory.
class Tuple {
 protected:
  std::vector<std::unique_ptr<Field>> fields;

  void swap(Tuple& t);

 public:
  Tuple(int size);

  Tuple(const Tuple& t);

  Tuple(Tuple&& tuple) = default;

  Tuple(Tuple& t1, Tuple&& t2);

  virtual ~Tuple() = default;

  Tuple& operator=(const Tuple& t);

  int Size() const;

  Field* GetField(int i) const;

  absl::StatusOr<std::unique_ptr<Field>> ReleaseField(int i);

  void SetField(int i, std::unique_ptr<Field> f);

  void SetField(int i, Field* f);

  operator std::string() const;

  friend std::ostream& operator<<(std::ostream& os, const Tuple& tuple);

  bool operator==(const Tuple& t) const;

  bool operator!=(const Tuple& t) const;

  // Don't use this for empty tuple
  template <typename H>
  friend H AbslHashValue(H h, const Tuple& tuple) {
    for (int i = 0; i < tuple.fields.size(); i++) {
      Field* field = tuple.fields[i].get();
      switch (field->GetType().GetValue()) {
        case Type::INT: {
          h = H::combine(std::move(h), *static_cast<IntField*>(field));
          break;
        }
        case Type::STRING: {
          h = H::combine(std::move(h), *static_cast<StringField*>(field));
          break;
        }
      }
    }
    return h;
  }
};

};  // namespace komfydb::common

#endif  // __TUPLE_H__
