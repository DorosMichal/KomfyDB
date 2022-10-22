#ifndef __INT_FIELD_H__
#define __INT_FIELD_H__

#include "komfydb/common/field.h"

namespace komfydb::common {

class IntField : Field {
 private:
  int value;

 public:
  IntField(int value);

  int GetValue();

  bool Compare(const Op& op, const Field& value) const override;

  constexpr Type GetType() override;

  // TODO(HashCode)
  // int HashCode() override;

  operator std::string() override;
};

};  // namespace komfydb::common

#endif  // __INT_FIELD_H__
