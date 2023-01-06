#include "komfydb/execution/order_by.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/execution/op.h"
#include "komfydb/utils/status_macros.h"

namespace {

using namespace komfydb::common;

}

namespace komfydb::execution {

OrderBy::OrderBy(std::unique_ptr<OpIterator> child, int order_by_field,
                 Order order, TupleDesc& tuple_desc)
    : OpIterator(tuple_desc),
      child(std::move(child)),
      order_by_field(order_by_field),
      order(order) {}

absl::StatusOr<std::unique_ptr<OrderBy>> OrderBy::Create(
    std::unique_ptr<OpIterator> child, int order_by_field, Order order) {
  TupleDesc* tuple_desc = child->GetTupleDesc();
  /* Check if tuple_desc has field nr order_by_field */
  RETURN_IF_ERROR(tuple_desc->GetFieldType(order_by_field).status());

  return std::unique_ptr<OrderBy>(
      new OrderBy(std::move(child), order_by_field, order, *tuple_desc));
}

OrderBy::Order OrderBy::GetOrder() {
  return order;
}

int OrderBy::GetOrderByField() {
  return order_by_field;
}

absl::Status OrderBy::Open() {
  RETURN_IF_ERROR(child->Open());
  ITERATE_RECORDS(child, rec) {
    child_records.push_back(std::move(rec.value()));
  }
  RETURN_IF_NOT_OOR(rec.status());
  std::sort(child_records.begin(), child_records.end(),
            [this](const std::unique_ptr<Record>& a,
                   const std::unique_ptr<Record>& b) {
              absl::StatusOr<Field*> fa = a->GetField(order_by_field);
              absl::StatusOr<Field*> fb = b->GetField(order_by_field);
              assert(fa.ok());
              assert(fb.ok());
              Op::Value comp = (order == Order::ASCENDING)
                                   ? Op::Value::LESS_THAN
                                   : Op::Value::GREATER_THAN;
              absl::StatusOr<bool> result = (*fa)->Compare(Op(comp), *fb);
              assert(result.ok());
              return *result;
            });
  it = child_records.begin();
  return absl::OkStatus();
}

void OrderBy::Close() {
  child->Close();
}

absl::Status OrderBy::FetchNext() {
  if (it == child_records.end()) {
    return absl::OutOfRangeError("No more records in this OpIterator");
  }
  next_record = std::move(*it++);
  return absl::OkStatus();
}

}  // namespace komfydb::execution