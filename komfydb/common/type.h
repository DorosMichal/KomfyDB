#ifndef __TYPE_H__
#define __TYPE_H__

#include <iostream>

namespace komfydb::common {

class Type {
 public:
  enum Value {
    INT,
    STRING,
  };

  Type(const Value& value);

  int GetLen();

  Value GetValue();

 private:
  Value value;
};

};  // namespace komfydb::common

#endif  // __TYPE_H__
