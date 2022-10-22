#ifndef __FIELD_H__
#define __FIELD_H__

#include <iostream>
#include <string>

#include "komfydb/common/type.h"
#include "komfydb/execution/op.h"

namespace {

using komfydb::execution::Op;

};  // namespace

namespace komfydb::common {

// TODO make this class a template class and explicitily create only
// classes with string and int (doable in C++).
class Field {
 public:
  Field(std::istream& is);

  virtual bool Compare(const Op& op, const Field& value) const;

  virtual Type GetType();

  // TODO(HashCode)
  // virtual int HashCode();

  virtual operator std::string();
};

};  // namespace komfydb::common

#endif  // __FIELD_H__
