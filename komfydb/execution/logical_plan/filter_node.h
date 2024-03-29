#ifndef __FILTER_NODE_H__
#define __FILTER_NODE_H__

#include "komfydb/common/column_ref.h"
#include "komfydb/common/field.h"
#include "komfydb/execution/op.h"
#include "komfydb/execution/predicate.h"

namespace {

using komfydb::common::ColumnRef;
using komfydb::common::Field;

};  // namespace

namespace komfydb::execution::logical_plan {

class FilterNode {
 public:
  enum FilterType {
    TWO_COLUMNS,
    COLUMN_CONSTANT,
  };

  ColumnRef lcol;
  ColumnRef rcol;
  std::unique_ptr<Field> constant;
  Op op;
  FilterType type;

  FilterNode(ColumnRef lcol, Op op, ColumnRef rcol)
      : lcol(lcol), rcol(rcol), op(op), type(TWO_COLUMNS) {}

  FilterNode(ColumnRef lcol, Op op, std::unique_ptr<Field> constant)
      : lcol(lcol),
        rcol(ColumnRef("", "")),
        constant(std::move(constant)),
        op(op),
        type(COLUMN_CONSTANT) {}

  absl::StatusOr<Predicate> GetPredicate(TupleDesc& tuple_desc) {
    ASSIGN_OR_RETURN(int l_field, tuple_desc.IndexForFieldName(lcol.column));
    switch (type) {
      case TWO_COLUMNS: {
        ASSIGN_OR_RETURN(int r_field,
                         tuple_desc.IndexForFieldName(rcol.column));
        return Predicate(l_field, op, r_field);
      }
      case COLUMN_CONSTANT: {
        return Predicate(l_field, op, constant->CreateCopy());
      }
    }
  }

  operator std::string() const {
    switch (type) {
      case TWO_COLUMNS:
        return static_cast<std::string>(lcol) + " " +
               static_cast<std::string>(op) + " " +
               static_cast<std::string>(rcol);
      case COLUMN_CONSTANT:
        return static_cast<std::string>(lcol) + " " +
               static_cast<std::string>(op) + " " +
               static_cast<std::string>(*constant);
    }
  }
};

};  // namespace komfydb::execution::logical_plan

#endif  // __FILTER_NODE_H__
