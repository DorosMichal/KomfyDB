#include "komfydb/execution/predicate.h"

namespace {

using namespace komfydb::common;
using namespace komfydb::storage;

}  // namespace

Predicate::Predicate(Op op, int field, std::unique_ptr<Field> const_field)
    : op(op),
      field1(field),
      const_field(std::move(const_field)),
      type(Type::COL_CONST) {}

Predicate::Predicate(Op op, int field1, int field2)
    : op(op),
      field1(field1),
      field2(field2),
      const_field(nullptr),
      type(Type::COL_COL) {}

Op Predicate::GetOp() {
  return op;
}

Field* Predicate::GetConstField() {
  return const_field.get();
}

int Predicate::GetField1() {
  return field1;
}

int Predicate::GetField2() {
  return field2;
}

Predicate::Type Predicate::GetType() {
  return type;
}

absl::StatusOr<bool> Predicate::Evaluate(const Record& record) {
  if (type == Type::COL_COL) {
    ASSIGN_OR_RETURN(Field * f1, record.GetField(field1));
    ASSIGN_OR_RETURN(Field * f2, record.GetField(field2));
    return f1->Compare(op, f2);
  }
  if (type == Type::COL_CONST) {
    ASSIGN_OR_RETURN(Field * f, record.GetField(field1));
    return f->Compare(op, const_field.get());
  }
  return absl::InvalidArgumentError("Unknown predicate type");
}
