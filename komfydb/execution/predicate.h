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

class Predicate {
 public:
  enum Type {
    COL_COL,
    COL_CONST,
  };

  Predicate(Op op, int field, std::unique_ptr<Field> const_field);

  Predicate(Op op, int field1, int field2);

  Op GetOp();

  Field* GetConstField();

  int GetField1();

  int GetField2();

  Type GetType();

  absl::StatusOr<bool> Evaluate(const Record& record);

 private:
  Op op;
  int field1, field2;
  std::unique_ptr<Field> const_field;
  Type type;
};

#endif  // __PREDICATE_H__