#ifndef __TYPE_H__
#define __TYPE_H__

#include <iostream>

#include "komfydb/storage/field.h"

namespace komfydb::common {

class Type {
 public:
  enum Value {
    INT,
    STRING,
  };

  Type(const Value& value);

  virtual int GetLen();

  virtual Value GetValue();

  virtual storage::Field Parse(std::istream& ist);

 private:
  Value value;
};

};  // namespace komfydb::common

#endif  // __TYPE_H__
