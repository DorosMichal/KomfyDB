#ifndef __BUFFER_POOL_H__
#define __BUFFER_POOL_H__

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"

#include "komfydb/common/permissions.h"
#include "komfydb/config.h"
#include "komfydb/storage/catalog.h"
#include "komfydb/storage/page.h"
#include "komfydb/storage/page_id.h"
#include "komfydb/transaction/transaction_id.h"

namespace {

using komfydb::common::Permissions;
using komfydb::transaction::TransactionId;

};  // namespace

namespace komfydb::storage {

class BufferPool {
 private:
  const int pages_cnt;
  std::shared_ptr<Catalog> catalog;
  absl::flat_hash_map<PageId, std::unique_ptr<Page>> page_pool;

  const static int PAGE_SIZE = CONFIG_PAGE_SIZE;
  const static int PAGES_CNT = CONFIG_BUFFER_POOL_PAGES_CNT;

 public:
  BufferPool(std::shared_ptr<Catalog> catalog, int pages_cnt = PAGES_CNT)
      : pages_cnt(pages_cnt), catalog(std::move(catalog)) {}

  int GetPageSize() const { return pages_cnt; }

  absl::StatusOr<Page*> GetPage(TransactionId tid, PageId pid,
                                Permissions perm);
};

};  // namespace komfydb::storage

#endif  // __BUFFER_POOL_H__
