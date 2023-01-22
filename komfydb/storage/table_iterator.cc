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
                             std::string_view table_name,
                             std::shared_ptr<Catalog> catalog,
                             std::shared_ptr<BufferPool> bufferpool)
    : catalog(std::move(catalog)),
      bufferpool(std::move(bufferpool)),
      table_name(table_name),
      tid(tid),
      table_id(table_id) {}

absl::StatusOr<std::unique_ptr<TableIterator>> TableIterator::Create(
    TransactionId tid, int table_id, std::shared_ptr<Catalog> catalog,
    std::shared_ptr<BufferPool> bufferpool) {

  ASSIGN_OR_RETURN(std::string table_name, catalog->GetTableName(table_id));
  return std::unique_ptr<TableIterator>(new TableIterator(
      tid, table_id, table_name, std::move(catalog), std::move(bufferpool)));
}

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

absl::Status TableIterator::Rewind() {
  Close();
  return Open();
}

absl::Status TableIterator::HasNext() {
  if (current_tuple != records.end()) {
    return absl::OkStatus();
  }

  RETURN_IF_ERROR(LoadNextPage());
  return HasNext();
}

absl::StatusOr<Record> TableIterator::Next() {
  RETURN_IF_ERROR(HasNext());
  return std::move(*current_tuple++);
}

absl::StatusOr<TupleDesc*> TableIterator::GetTupleDesc() {
  return catalog->GetTupleDesc(table_id);
}

std::string TableIterator::GetTableName() {
  return table_name;
}

};  // namespace komfydb::storage
