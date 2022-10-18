#ifndef __TUPLE_H__
#define __TUPLE_H__

#include "komfydb/storage/tuple_desc.h"

namespace komfydb::storage {

class Tuple {
 private:
  TupleDesc td;

 public:
  Tuple(const TupleDesc& td);

  TupleDesc getTupleDesc();
};

};  // namespace komfydb::storage

#endif  // __TUPLE_H__
