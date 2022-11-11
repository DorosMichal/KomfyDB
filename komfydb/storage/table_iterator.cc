#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/storage/table_iterator.h"

namespace komfydb::storage {

TableIterator::TableIterator(TransactionId tid, int table_id,
                             std::shared_ptr<Catalog> catalog,
                             std::shared_ptr<BufferPool> bufferpool)
    : tid(tid), table_id(table_id), catalog(catalog), bufferpool(bufferpool) {}

absl::StatusOr<bool> TableIterator::LoadNextPage() {
  if (++page_ctr >= num_pages)
    return false;
    ASSIGN_OR_RETURN(Page* page, bufferpool->GetPage(PageId(table_id, page_ctr));
    records = page->GetRecords();
    current_tuple = records->begin();
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
  if (current_tuple != records->end())
    return true;

  ASSIGN_OR_RETURN(bool good, LoadNextPage());
  if (!good)
    return false;
  return HasNext();
}

absl::StatusOr<Record> TableIterator::Next() {
  ASSIGN_OR_RETURN(bool good, HasNext());
  if (!good)
    return absl::OutOfRangeError("No more records in this table");
  return *current_tuple++;
}

};  // namespace komfydb::storage
