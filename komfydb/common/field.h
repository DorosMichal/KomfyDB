#ifndef __FIELD_H__
#define __FIELD_H__

#include <iostream>
#include <string>

#include "absl/status/statusor.h"

#include "komfydb/common/type.h"
#include "komfydb/execution/aggregator.h"
#include "komfydb/execution/op.h"

typedef komfydb::execution::Aggregator::AggregateType AggregateType;

namespace {

using komfydb::common::Type;
using komfydb::execution::Op;

};  // namespace

namespace komfydb::common {

class Field {
 public:
  virtual ~Field(){};

  virtual absl::StatusOr<bool> Compare(const Op& op, const Field* f) const = 0;

  virtual Type GetType() const = 0;

  virtual std::unique_ptr<Field> CreateCopy() const = 0;

  virtual operator std::string() const = 0;
};

};  // namespace komfydb::common

#endif  // __FIELD_H__
