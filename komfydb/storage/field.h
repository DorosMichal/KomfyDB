#ifndef __FIELD_H__
#define __FIELD_H__


#include <iostream>
#include <string>

#include "komfydb/execution/predicate.h"
#include "komfydb/common/type.h"


namespace {
  
using komfydb::execution::Predicate;
using komfydb::execution::Op;
using komfydb::common::Type;

};


namespace komfydb::storage {

class Field {
public:
  virtual void Serialize(std::ostream& os) const;

  virtual bool Compare(const Op& op, const Field& value) const;

  virtual Type GetType();

  virtual int HashCode();
  
  virtual operator std::string();
};

};


#endif  // __FIELD_H__
