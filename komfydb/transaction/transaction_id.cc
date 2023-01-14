#include "komfydb/transaction/transaction_id.h"

namespace komfydb::transaction {

std::atomic_long TransactionId::counter{0};

TransactionId::TransactionId() {
  id = ++counter;
}

TransactionId::TransactionId(long id) : id(id) {}

long TransactionId::GetId() {
  return id;
}

bool TransactionId::operator==(const TransactionId& tid) const {
  return tid.id == id;
}

bool TransactionId::operator==(const long& id) const {
  return this->id == id;
}

}  // namespace komfydb::transaction
