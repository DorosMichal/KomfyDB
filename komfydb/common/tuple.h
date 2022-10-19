#ifndef __TUPLE_H__
#define __TUPLE_H__

#include "komfydb/common/tuple_desc.h"

namespace komfydb::common {

class Tuple {
 private:
  TupleDesc td;

 public:
  Tuple(const TupleDesc& td);

  TupleDesc getTupleDesc();
};

};  // namespace komfydb::common

#endif  // __TUPLE_H__
