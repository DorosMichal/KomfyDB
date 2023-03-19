#ifndef __LIMIT_H__
#define __LIMIT_H__

#include "komfydb/execution/op_iterator.h"

namespace komfydb::execution {

using limit_t = long;

class Limit : public OpIterator {
 public:
  static absl::StatusOr<std::unique_ptr<Limit>> Create(
      std::unique_ptr<OpIterator> child, limit_t limit);

  absl::Status Open() override;

  void Close() override;

  absl::Status Rewind() override;

  void Explain(std::ostream& os, int indent = 0) override;

 private:
  std::unique_ptr<OpIterator> child;
  limit_t limit;
  limit_t fetched;

  Limit(std::unique_ptr<OpIterator> child, limit_t limit);

  absl::Status FetchNext() override;
};

};  // namespace komfydb::execution

#endif  // __LIMIT_H__
