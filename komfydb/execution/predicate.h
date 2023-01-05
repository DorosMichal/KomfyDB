#ifndef __PREDICATE_H__
#define __PREDICATE_H__

#include "komfydb/common/field.h"
#include "komfydb/common/int_field.h"
#include "komfydb/common/string_field.h"
#include "komfydb/execution/op.h"
#include "komfydb/storage/record.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "komfydb/utils/status_macros.h"

namespace {

using namespace komfydb::common;
using namespace komfydb::storage;

}  // namespace

namespace komfydb::execution {

class Predicate {
 public:
  enum Type {
    COL_COL,
    COL_CONST,
  };

  Predicate() = default;

  Predicate(int field, Op op, std::unique_ptr<Field> const_field);

  Predicate(int l_field, Op op, int r_field);

  Op GetOp();

  Field* GetConstField();

  int GetLField();

  int GetRField();

  Type GetType();

  bool Evaluate(const Record& record);

  friend std::ostream& operator<<(std::ostream& os, const Predicate& p);

 private:
  Op op;
  int l_field, r_field;
  std::unique_ptr<Field> const_field;
  Type type;
};

};  // namespace komfydb::execution

#endif  // __PREDICATE_H__
