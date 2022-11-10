#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/common/tuple.h"

namespace komfydb::storage {

TableIterator::TableIterator(TransactionId tid, int table_id)
    : tid(tid), table_id(table_id) {}

absl::StatusOr<bool> TableIterator::LoadNextPage() {
  if (++page_ctr >= num_pages)
    return false;
    ASSIGN_OR_RETURN(Page* page, bufferpool->GetPage(PageId(table_id, page_ctr));
    tuples = page->GetTuples();
    current_tuple = tuples.begin();
    return true;
}

absl::Status TableIterator::LoadFirstPage() {
  page_ctr = -1;
  ASSIGN_OR_RETURN(bool good, LoadNextPage());
  if (!good)
    return absl::OutOfRangeError("No pages in this file/table");
  return absl::StatusOK();
}

absl::Status TableIterator::Open() {
  ASSIGN_OR_RETURN(DbFile * file, catalog->GetDatabaseFile(table_id));
  num_pages = file->GetNumPages();
  RETURN_IF_ERROR(LoadFirstPage());

  return absl::OkStatus();
}

void TableIterator::Close() {}

absl::StatusOr<bool> TableIterator::HasNext() {
  if (current_tuple != tuples.end())
    return true;

  ASSIGN_OR_RETURN(bool good, LoadNextPage());
  if (!good)
    return false;
  return HasNext();
}

absl::StatusOr<Tuple> TableIterator::Next() {
  ASSIGN_OR_RETURN(bool good, HasNext());
  if (!good)
    return absl::OutOfRangeError("No more tuples in this table");
  return *current_tuple++;
}
};
}
;  // namespace komfydb::storage
