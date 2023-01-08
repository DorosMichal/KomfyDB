#include "komfydb/storage/buffer_pool.h"

#include "glog/logging.h"

#include "komfydb/storage/db_file.h"
#include "komfydb/utils/status_macros.h"

namespace komfydb::storage {

absl::StatusOr<Page*> BufferPool::GetPage(TransactionId tid, PageId pid,
                                          Permissions perm) {
  static int cnt = 0;
  if (!page_pool.count(pid)) {
    LOG(INFO) << "Reading for the " << ++cnt << " time.";
    LOG(INFO) << page_pool.contains(pid) << " " << pid.GetTableId() << " "
              << pid.GetPageNumber();
    if (page_pool.size() == pages_cnt) {
      // return absl::InvalidArgumentError("Cannot read another file to the pool");
      LOG(ERROR) << "Buffer run out of space\n";
      assert(false);
    }
    LOG(INFO) << "Getting page from disk: " << static_cast<std::string>(pid);
    ASSIGN_OR_RETURN(DbFile * file, catalog->GetDatabaseFile(pid.GetTableId()));
    LOG(INFO) << "Siema xd " << page_pool.contains(pid);
    ASSIGN_OR_RETURN(page_pool[pid], file->ReadPage(pid));
    LOG(INFO) << "Siema xd " << page_pool.contains(pid);
  }

  return page_pool.at(pid).get();
}

};  // namespace komfydb::storage
