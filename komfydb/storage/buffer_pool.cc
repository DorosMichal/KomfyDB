#include "komfydb/utils/status_macros.h"

#include "komfydb/storage/buffer_pool.h"
#include "komfydb/storage/db_file.h"

namespace komfydb::storage {

absl::StatusOr<Page*> BufferPool::GetPage(TransactionId tid, PageId pid,
                                          Permissions perm) {
  if (!page_pool.contains(pid)) {
    if (page_pool.size() == pages_cnt) {
      return absl::InvalidArgumentError("Cannot read another file to the pool");
    }
    ASSIGN_OR_RETURN(DbFile * file, catalog->GetDatabaseFile(pid.GetTableId()));
    page_pool[pid] = file->ReadPage(pid);
  }

  return page_pool.at(pid).get();
}

};  // namespace komfydb::storage
