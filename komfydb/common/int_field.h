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

  void GetValue(int& i) const override;

  void GetValue(std::string& s) const override{};

  // TODO(HashCode)
  // int HashCode() override;

  operator std::string() const override;
};

};  // namespace komfydb::common

#endif  // __INT_FIELD_H__
