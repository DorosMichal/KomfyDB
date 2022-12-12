#ifndef __PROJECT_H__
#define __PROJECT_H__

#include <vector>

#include "komfydb/common/tuple_desc.h"
#include "komfydb/execution/op_iterator.h"

namespace komfydb::execution {

class Project : public OpIterator {
 private:
  std::unique_ptr<OpIterator> child;
  std::vector<int>& out_field_idxs;

  Project(std::unique_ptr<OpIterator> child, std::vector<int>& out_field_idxs,
          TupleDesc tuple_desc);

  absl::Status FetchNext() override;

 public:
  static absl::StatusOr<std::unique_ptr<Project>> Create(
      std::unique_ptr<OpIterator> child, std::vector<int>& out_field_idxs);

  absl::Status Open() override;

  void Close() override;

  absl::Status Rewind() override;

  std::string GetAlias();
};

}  // namespace komfydb::execution

#endif  // __PROJECT_H__