#ifndef __INT_FIELD_H__
#define __INT_FIELD_H__

#include "komfydb/common/field.h"

namespace komfydb::common {

class IntField : public Field {
 private:
  int value;

 public:
  IntField(int value = 0);

  IntField(const IntField& f);

  IntField(IntField&& f) = default;

  ~IntField() override {}

  absl::StatusOr<bool> Compare(const Op& op, const Field* f) const override;

  Type GetType() const override;

  int GetValue() const;

  void SetValue(int i);

  std::unique_ptr<Field> CreateCopy() const override;

  bool operator==(const IntField& field) const;

  operator std::string() const override;

  template <typename H>
  friend H AbslHashValue(H h, const IntField& field) {
    return H::combine(std::move(h), field.value);
  }
};

};  // namespace komfydb::common

#endif  // __INT_FIELD_H__
