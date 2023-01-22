#ifndef __DB_FILE_ITERATOR_H__
#define __DB_FILE_ITERATOR_H__

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/storage/buffer_pool.h"
#include "komfydb/storage/catalog.h"
#include "komfydb/storage/record.h"
#include "komfydb/transaction/transaction_id.h"

namespace {

using komfydb::transaction::TransactionId;

};  // namespace

namespace komfydb::storage {

class TableIterator {
 public:
  static absl::StatusOr<std::unique_ptr<TableIterator>> Create(
      TransactionId tid, int table_id, std::shared_ptr<Catalog> catalog,
      std::shared_ptr<BufferPool> bufferpool);

  absl::Status Open();

  void Close();

  absl::Status Rewind();

  absl::Status HasNext();

  absl::StatusOr<Record> Next();

  absl::StatusOr<TupleDesc*> GetTupleDesc();

  std::string GetTableName();

 private:
  std::vector<Record> records;
  std::vector<Record>::iterator current_tuple;
  std::shared_ptr<Catalog> catalog;
  std::shared_ptr<BufferPool> bufferpool;
  std::string table_name;
  TransactionId tid;
  int page_ctr;
  int table_id;
  int num_pages;

  TableIterator(TransactionId tid, int table_id, std::string_view table_name,
                std::shared_ptr<Catalog> catalog,
                std::shared_ptr<BufferPool> bufferpool);

  absl::Status LoadNextPage();
};

};  // namespace komfydb::storage

#endif  // __DB_FILE_ITERATOR_H__
