#include "komfydb/storage/buffer_pool.h"

#include "buffer_pool.h"
#include "glog/logging.h"

#include "komfydb/storage/db_file.h"
#include "komfydb/transaction/transaction_id.h"
#include "komfydb/utils/status_macros.h"

namespace komfydb::storage {

BufferPool::BufferPool(std::shared_ptr<Catalog> catalog, int pages_cnt)
    : pages_cnt(pages_cnt), catalog(std::move(catalog)) {}

BufferPool::~BufferPool() {
  LOG(INFO) << "Flushing all pages...";
  static_cast<void>(
      FlushPages(transaction::TransactionId(transaction::NO_TID)));
  LOG(INFO) << "All pages flushed.";
}

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
  // TODO(Transactions): can only evict if not dirty or transaction finished
  if (lru.empty()) {
    return absl::OkStatus();
  }

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
  ASSIGN_OR_RETURN(DbFile * file, catalog->GetDatabaseFile(pid.GetTableId()));
  RETURN_IF_ERROR(file->WritePage(page));
  // TODO(transactions) cancel transaction if error
  return absl::OkStatus();
}

absl::Status BufferPool::FlushPages(TransactionId tid) {
  for (auto it = page_pool.begin(); it != page_pool.end(); it++) {
    if (tid.GetId() == transaction::NO_TID ||
        it->second->GetLastTransaction() == tid) {
      RETURN_IF_ERROR(FlushPage(it->second->GetId()));
    }
  }
  return absl::OkStatus();
}

absl::Status BufferPool::InsertPage(std::unique_ptr<Page> page) {
  if (page_pool.size() == pages_cnt) {
    RETURN_IF_ERROR(EvictPage());
  }
  lru.push_front(page->GetId());
  pid_to_lru[page->GetId()] = lru.begin();
  page_pool[page->GetId()] = std::move(page);
  return absl::OkStatus();
}

std::list<PageId> BufferPool::GetLru() {
  return lru;
}

absl::Status BufferPool::InsertTuples(
    std::vector<std::unique_ptr<Tuple>>&& tuples, uint32_t table_id,
    TransactionId tid) {
  ASSIGN_OR_RETURN(DbFile * dbfile, catalog->GetDatabaseFile(table_id));
  int page_count = dbfile->PageCount();
  int tuples_added = 0, all_tuples = tuples.size();
  // Iterate through existing pages
  for (int page_no = 0; page_no < page_count && tuples_added < all_tuples;
       page_no++) {
    ASSIGN_OR_RETURN(Page * page, GetPage(tid, PageId(table_id, page_no),
                                          Permissions::READ_WRITE));
    int free_space = page->GetFreeSpace();
    int to_add = std::min(all_tuples - tuples_added, free_space);
    RETURN_IF_ERROR(page->AddTuples(&tuples[tuples_added], to_add));
    page->SetDirty(true, tid);
    tuples_added += to_add;
  }
  // Create new pages while needed
  while (tuples_added < all_tuples) {
    ASSIGN_OR_RETURN(std::unique_ptr<Page> page, dbfile->CreatePage());
    int free_space = page->GetFreeSpace();
    int to_add = std::min(all_tuples - tuples_added, free_space);
    RETURN_IF_ERROR(page->AddTuples(&tuples[tuples_added], to_add));
    page->SetDirty(true, tid);
    tuples_added += to_add;
    RETURN_IF_ERROR(InsertPage(std::move(page)));
  }
  return absl::OkStatus();
}

absl::Status BufferPool::RemoveTuples(std::vector<RecordId>& ids_to_remove,
                                      uint32_t table_id, TransactionId tid) {
  ASSIGN_OR_RETURN(DbFile * dbfile, catalog->GetDatabaseFile(table_id));
  int page_count = dbfile->PageCount(), all_tuples = ids_to_remove.size();
  Page* page = nullptr;
  for (int i = 0; i < all_tuples; i++) {
    if (!page || ids_to_remove[i].GetPageId() != page->GetId()) {
      ASSIGN_OR_RETURN(page, GetPage(tid, ids_to_remove[i].GetPageId(),
                                     Permissions::READ_WRITE));
    }
    RETURN_IF_ERROR(page->RemoveRecord(ids_to_remove[i]));
    page->SetDirty(true, tid);
  }
  return absl::OkStatus();
}

};  // namespace komfydb::storage
