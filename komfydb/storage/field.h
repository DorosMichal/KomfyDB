#ifndef __FIELD_H__
#define __FIELD_H__

#include <iostream>
#include <string>

#include "komfydb/common/type.h"
#include "komfydb/execution/predicate.h"

namespace {

using komfydb::common::Type;
using komfydb::execution::Op;
using komfydb::execution::Predicate;

};  // namespace

namespace komfydb::storage {

class Field {
 public:
  virtual void Serialize(std::ostream& os) const;

  virtual bool Compare(const Op& op, const Field& value) const;

  virtual Type GetType();

  virtual int HashCode();

  virtual operator std::string();
};

};  // namespace komfydb::storage

#endif  // __FIELD_H__
