#ifndef __ORDER_BY_H__
#define __ORDER_BY_H__

#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/execution/op_iterator.h"

namespace komfydb::execution {

class OrderBy : public OpIterator {
 public:
  enum Order {
    ASCENDING,
    DESCENDING,
  };

  static absl::StatusOr<std::unique_ptr<OrderBy>> Create(
      std::unique_ptr<OpIterator> child, int order_by_field, Order order);

  Order GetOrder();

  int GetOrderByField();

  absl::Status Open() override;

  void Close() override;

 private:
  std::unique_ptr<OpIterator> child;
  int order_by_field;
  Order order;
  std::vector<std::unique_ptr<Record>> child_records;
  std::vector<std::unique_ptr<Record>>::iterator it;

  OrderBy(std::unique_ptr<OpIterator> child, int order_by_field, Order order,
          TupleDesc& tuple_desc);

  absl::Status FetchNext() override;
};

}  // namespace komfydb::execution

#endif