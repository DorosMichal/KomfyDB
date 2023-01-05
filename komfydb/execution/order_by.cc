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
                 Order order, TupleDesc& td)
    : child(std::move(child)),
      td(td),
      order_by_field(order_by_field),
      order(order) {}

absl::StatusOr<std::unique_ptr<OrderBy>> OrderBy::Create(
    std::unique_ptr<OpIterator> child, int order_by_field, Order order) {
  TupleDesc* td = child->GetTupleDesc();
  /* Check if td has field nr order_by_field */
  RETURN_IF_ERROR(td->GetFieldType(order_by_field).status());

  return std::unique_ptr<OrderBy>(
      new OrderBy(std::move(child), order_by_field, order, *td));
}

OrderBy::Order OrderBy::GetOrder() {
  return order;
}

int OrderBy::GetOrderByField() {
  return order_by_field;
}

absl::Status OrderBy::Open() {
  RETURN_IF_ERROR(child->Open());
  absl::StatusOr<Record> rec;
  while ((rec = child->Next()).ok()) {
    child_records.push_back(std::move(*rec));
  }
  if (!absl::IsOutOfRange(rec.status())) {
    return rec.status();
  }
  std::sort(child_records.begin(), child_records.end(),
            [this](const Record& a, const Record& b) {
              absl::StatusOr<Field*> fa = a.GetField(order_by_field);
              absl::StatusOr<Field*> fb = b.GetField(order_by_field);
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

absl::StatusOr<Record> OrderBy::Next() {
  if (it == child_records.end()) {
    return absl::OutOfRangeError("No more records in this OpIterator.");
  }
  return *it++;
}

TupleDesc* OrderBy::GetTupleDesc() {
  return &td;
}

}  // namespace komfydb::execution
