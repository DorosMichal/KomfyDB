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

  virtual ~Field(){};

  virtual bool Compare(const Op& op, const Field& value) const = 0;

  virtual Type GetType() = 0;

  // TODO(HashCode)
  // virtual int HashCode();

  virtual operator std::string() const = 0;
};

};  // namespace komfydb::common

#endif  // __FIELD_H__
