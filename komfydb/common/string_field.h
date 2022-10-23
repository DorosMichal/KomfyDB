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

  int max_size;

 public:
  StringField(const absl::string_view& s, int max_size);

  ~StringField() override { }

  void GetValue(int& i) const override { };

  void GetValue(std::string& s) const override;

  absl::StatusOr<bool> Compare(const Op& op, const Field& f) const override;

  Type GetType() const override;

  // TODO(HashCode)
  // int HashCode() override;

  operator std::string() const override;
};

};  // namespace komfydb::common

#endif  // __STRING_FIELD_H__
