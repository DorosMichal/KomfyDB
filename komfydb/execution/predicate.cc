#include "komfydb/execution/predicate.h"

namespace {

using namespace komfydb::common;
using namespace komfydb::storage;

}  // namespace

namespace komfydb::execution {

Predicate::Predicate(int field, Op op, std::unique_ptr<Field> const_field)
    : op(op),
      l_field(field),
      const_field(std::move(const_field)),
      type(Type::COL_CONST) {}

Predicate::Predicate(int l_field, Op op, int r_field)
    : op(op),
      l_field(l_field),
      r_field(r_field),
      const_field(nullptr),
      type(Type::COL_COL) {}

Op Predicate::GetOp() {
  return op;
}

Field* Predicate::GetConstField() {
  return const_field.get();
}

int Predicate::GetLField() {
  return l_field;
}

int Predicate::GetRField() {
  return r_field;
}

Predicate::Type Predicate::GetType() {
  return type;
}

bool Predicate::Evaluate(const Record& record) {
  /* We assume that GetField() or Compare() cannot fail here */
  switch (type) {
    case Type::COL_COL: {
      return *(
          (*record.GetField(l_field))->Compare(op, *record.GetField(r_field)));
    }
    case Type::COL_CONST: {
      return *((*record.GetField(l_field))->Compare(op, const_field.get()));
    }
  }
}

std::ostream& operator<<(std::ostream& os, const Predicate& p) {
  switch (p.type) {
    case Predicate::Type::COL_COL: {
      os << "[col " << p.l_field << "] " << p.op << "[col " << p.r_field << "]";
    }
    case Predicate::Type::COL_CONST: {
      os << "[col " << p.l_field << "] " << p.op << " " << p.const_field.get();
    }
  }
  return os;
}

};  // namespace komfydb::execution
