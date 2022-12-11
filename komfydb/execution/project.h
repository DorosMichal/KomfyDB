#ifndef __PROJECT_H__
#define __PROJECT_H__

#include <vector>

#include "komfydb/common/tuple_desc.h"
#include "komfydb/execution/op_iterator.h"

namespace komfydb::execution {

class Project : public OpIterator {
 private:
  OpIterator* child;

 public:
  Project(std::vector<int> out_field_idxs, OpIterator* child);
};

}  // namespace komfydb::execution

#endif  // __PROJECT_H__