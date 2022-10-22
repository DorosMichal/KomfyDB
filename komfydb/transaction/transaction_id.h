#ifndef __TRANSACTION_ID_H__
#define __TRANSACTION_ID_H__

#include <atomic>

namespace komfydb::transaction {

class TransactionId {
 private:
  static std::atomic_long counter;

  long id;

 public:
  TransactionId();

  long GetId();

  bool operator==(const TransactionId& tid) const;

  // TODO(HashCode): is this necessary?
  // int HashCode();
};

};  // namespace komfydb::transaction

#endif  //__TRANSACTION_ID_H__
