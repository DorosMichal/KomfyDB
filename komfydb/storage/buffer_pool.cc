#include "komfydb/storage/buffer_pool.h"

#include "glog/logging.h"

#include "komfydb/storage/db_file.h"
#include "komfydb/utils/status_macros.h"

namespace komfydb::storage {

BufferPool::BufferPool(std::shared_ptr<Catalog> catalog, int pages_cnt)
    : pages_cnt(pages_cnt), catalog(std::move(catalog)) {}

absl::StatusOr<Page*> BufferPool::GetPage(TransactionId tid, PageId pid,
                                          Permissions perm) {
  if (!page_pool.contains(pid)) {
    if (page_pool.size() == pages_cnt) {
      RETURN_IF_ERROR(EvictPage());
    }
    LOG(INFO) << "Getting page from disk: " << static_cast<std::string>(pid);
    ASSIGN_OR_RETURN(DbFile * file, catalog->GetDatabaseFile(pid.GetTableId()));
    ASSIGN_OR_RETURN(page_pool[pid], file->ReadPage(pid));
    lru.push_front(pid);
    pid_to_lru[pid] = lru.begin();
  } else {
    // We need to move the page to the beginning of the LRU list.
    auto it = pid_to_lru[pid];
    lru.erase(it);
    lru.push_front(pid);
    pid_to_lru[pid] = lru.begin();
  }
  return page_pool.at(pid).get();
}

absl::Status BufferPool::EvictPage() {
  PageId evict = lru.back();
  lru.pop_back();

  RETURN_IF_ERROR(FlushPage(evict));
  page_pool.erase(evict);
  return absl::OkStatus();
}

absl::Status BufferPool::FlushPage(PageId pid) {
  Page* page = page_pool[pid].get();
  if (!page->IsDirty()) {
    return absl::OkStatus();
  }
  // TODO(Writeable pages)
  return absl::UnimplementedError("Modifying pages not implemented yet.");
}

absl::Status BufferPool::FlushPages(TransactionId tid) {
  // TODO(Writeable pages)
  return absl::UnimplementedError("Modifying pages not implemented yet.");
}

std::list<PageId> BufferPool::GetLru() {
  return lru;
}

};  // namespace komfydb::storage
