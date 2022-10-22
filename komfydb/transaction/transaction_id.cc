#include "komfydb/transaction/transaction_id.h"

namespace komfydb::transaction {

std::atomic_long TransactionId::counter{0};
  
TransactionId::TransactionId() {
  id = ++counter;
}

long TransactionId::GetId() {
  return id;
}

bool TransactionId::operator==(const TransactionId &tid) const {
  return tid.id == id;  
}

}
