#include "komfydb/storage/table_iterator.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/common/permissions.h"
#include "komfydb/utils/status_macros.h"

namespace {

using komfydb::common::Permissions;

};

namespace komfydb::storage {

TableIterator::TableIterator(TransactionId tid, int table_id,
                             std::shared_ptr<Catalog> catalog,
                             std::shared_ptr<BufferPool> bufferpool)
    : tid(tid), table_id(table_id), catalog(catalog), bufferpool(bufferpool) {}

absl::Status TableIterator::LoadNextPage() {
  page_ctr++;
  ASSIGN_OR_RETURN(Page * page,
                   bufferpool->GetPage(tid, PageId(table_id, page_ctr),
                                       Permissions::READ_ONLY));
  records = page->GetRecords();
  current_tuple = records.begin();
  return absl::OkStatus();
}

absl::Status TableIterator::Open() {
  page_ctr = -1;
  RETURN_IF_ERROR(LoadNextPage());

  return absl::OkStatus();
}

void TableIterator::Close() {}

bool TableIterator::HasNext() {
  if (current_tuple != records.end()) {
    return true;
  }

  absl::Status status = LoadNextPage();
  if (absl::IsOutOfRange(status)) {
    return false;
  }
  assert(status.ok());
  return HasNext();
}

absl::StatusOr<Record> TableIterator::Next() {
  if (!HasNext()) {
    return absl::OutOfRangeError("No more records in this table");
  }
  return std::move(*current_tuple++);
}

absl::StatusOr<TupleDesc*> TableIterator::GetTupleDesc() {
  return catalog->GetTupleDesc(table_id);
}

};  // namespace komfydb::storage
