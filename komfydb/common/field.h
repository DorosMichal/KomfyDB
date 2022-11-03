#ifndef __FIELD_H__
#define __FIELD_H__

#include <iostream>
#include <string>

#include "absl/status/statusor.h"

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
  virtual ~Field(){};

  virtual absl::StatusOr<bool> Compare(const Op& op, const Field& f) const = 0;

  virtual Type GetType() const = 0;

  // I'm not sure how to solve it better. The case is that we
  // want to have to derived classes, IntField and StringField,
  // both of them implementing  'GetValue', but returning something
  // different. Also we'd like to have functions that take Field
  // as an argument and we want to be able to call these functions,
  // so this is the only idea that I had to do it.
  virtual void GetValue(int& i) const = 0;

  virtual void GetValue(std::string& s) const = 0;

  // TODO(HashCode)
  // virtual int HashCode();

  virtual operator std::string() const = 0;
};

};  // namespace komfydb::common

#endif  // __FIELD_H__
