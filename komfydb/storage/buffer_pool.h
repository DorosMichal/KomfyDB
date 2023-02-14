#ifndef __BUFFER_POOL_H__
#define __BUFFER_POOL_H__

#include <list>

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"

#include "komfydb/common/permissions.h"
#include "komfydb/common/tuple.h"
#include "komfydb/config.h"
#include "komfydb/storage/catalog.h"
#include "komfydb/storage/page.h"
#include "komfydb/storage/page_id.h"
#include "komfydb/transaction/transaction_id.h"

namespace {

using komfydb::common::Permissions;
using komfydb::common::Tuple;
using komfydb::transaction::TransactionId;

};  // namespace

namespace komfydb::storage {

class BufferPool {
 public:
  const static int PAGE_SIZE = CONFIG_PAGE_SIZE;
  const static int PAGES_CNT = CONFIG_BUFFER_POOL_PAGES_CNT;

  BufferPool(std::shared_ptr<Catalog> catalog, int pages_cnt = PAGES_CNT);

  ~BufferPool();

  int GetPageSize() const { return pages_cnt; }

  absl::StatusOr<Page*> GetPage(TransactionId tid, PageId pid,
                                Permissions perm);

  absl::Status FlushPage(PageId pid);

  absl::Status FlushPages(TransactionId tid);

  absl::Status InsertTuples(std::vector<std::unique_ptr<Tuple>>&& tuples,
                            uint32_t table_id, TransactionId tid);

  absl::Status RemoveTuples(std::vector<RecordId>& ids, uint32_t table_id,
                            TransactionId tid);

  // Test purposes only.
  std::list<PageId> GetLru();

 private:
  const int pages_cnt;
  std::shared_ptr<Catalog> catalog;
  absl::flat_hash_map<PageId, std::unique_ptr<Page>> page_pool;
  absl::flat_hash_map<PageId, std::list<PageId>::iterator> pid_to_lru;
  std::list<PageId> lru;

  absl::Status EvictPage();

  absl::Status InsertPage(std::unique_ptr<Page> page);
};

};  // namespace komfydb::storage

#endif  // __BUFFER_POOL_H__
