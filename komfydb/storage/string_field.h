#ifndef __STRING_FIELD_H__
#define __STRING_FIELD_H__

#include "absl/strings/string_view.h"

#include "komfydb/storage/field.h"

namespace komfydb::storage {

class StringField : Field {
 private:
  std::string value;
  int max_size;

 public:
  StringField(const absl::string_view& s, int max_size);

  std::string GetValue();

  bool Compare(const Op& op, const Field& value) const override;

  Type GetType() override;

  int HashCode() override;

  operator std::string() override;
};

};  // namespace komfydb::storage

#endif  // __STRING_FIELD_H__
