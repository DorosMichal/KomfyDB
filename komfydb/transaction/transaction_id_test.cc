#include <pthread.h>

#include <vector>

#include "gtest/gtest.h"

#include "komfydb/transaction/transaction_id.h"

namespace komfydb::transaction {

void *InstantiateTransactionIds(void *args) {
  long cnt = (long)args;
  std::vector<long> *ids = new std::vector<long>;
  for (int i = 0; i < cnt; i++) {
    TransactionId tid;
    ids->push_back(tid.GetId());
  }
  return ids;
}

TEST(TransacionId, MultithreadedConstructing) {
  const long cnt = 1e6;
  const int thrds = 5;

  pthread_t thread_ids[thrds];  
  std::vector<long> *ids[thrds]; 

  for (int i = 0; i < thrds; i++) {
    pthread_create(&thread_ids[i], nullptr, InstantiateTransactionIds, (void *)cnt);
  }

  for (int i = 0; i < thrds; i++) {
    pthread_join(thread_ids[i], (void **)&ids[i]);
  }

  std::vector<long> all_ids;
  for (int i = 0; i < thrds; i++) {
    all_ids.insert(all_ids.end(), ids[i]->begin(), ids[i]->end());
  }
  
  ASSERT_EQ(all_ids.size(), thrds*cnt);
  sort(all_ids.begin(), all_ids.end());

  for (long i = 0; i < thrds*cnt; i++) {
    EXPECT_EQ(all_ids[i], i+1);
  }
  
  for (int i = 0; i < thrds; i++) {
    delete ids[i];
  }
}

};
