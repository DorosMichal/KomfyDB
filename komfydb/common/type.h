#ifndef __TYPE_H__
#define __TYPE_H__

#include <iostream>

namespace komfydb::common {

class Type {
 public:
  const static int STR_LEN = 128;
  const static int INT_LEN = 4;

  enum Value {
    INT,
    STRING,
  };

  Type(const Value& value);

  int GetLen();

  Value GetValue();

  operator std::string();

 private:
  Value value;
};

};  // namespace komfydb::common

#endif  // __TYPE_H__
