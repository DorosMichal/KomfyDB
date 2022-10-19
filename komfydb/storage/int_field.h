#ifndef __INT_FIELD_H__
#define __INT_FIELD_H__

#include "komfydb/storage/field.h"

namespace komfydb::storage {

class IntField : Field {
 private:
  int value;

 public:
  IntField(int value);

  int GetValue();

  bool Compare(const Op& op, const Field& value) const override;

  Type GetType() override;

  int HashCode() override;

  operator std::string() override;
};

};  // namespace komfydb::storage

#endif  // __INT_FIELD_H__
