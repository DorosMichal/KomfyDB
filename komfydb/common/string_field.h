#ifndef __STRING_FIELD_H__
#define __STRING_FIELD_H__

#include <string>

#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"

#include "komfydb/common/field.h"

namespace komfydb::common {

class StringField : public Field {
 private:
  std::string value;

 public:
  StringField(const absl::string_view& s = "");

  StringField(const StringField& field);

  StringField(StringField&& field) = default;

  ~StringField() override {}

  bool Compare(const Op& op, const Field* f) const override;

  std::string GetValue() const;

  void SetValue(absl::string_view s);

  Type GetType() const override;

  std::unique_ptr<Field> CreateCopy() const override;

  bool operator==(const StringField& field) const;

  operator std::string() const override;

  template <typename H>
  friend H AbslHashValue(H h, const StringField& field) {
    return H::combine(std::move(h), field.value);
  }
};

};  // namespace komfydb::common

#endif  // __STRING_FIELD_H__
