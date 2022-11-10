#ifndef __DB_FILE_ITERATOR_H__
#define __DB_FILE_ITERATOR_H__

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/common/tuple.h"

namespace {

using komfydb::common::Tuple;

};  // namespace

namespace komfydb::storage {

class TableIterator {
 private:
  int page_ctr;
  int table_id;
  int num_pages;
  TransactionId tid;
  std::vector<Tuple>& tuples;
  std::vector<Tuple>::iterator current_tuple;
  static std::shared_ptr<BufferPool> bufferpool;
  static std::shared_ptr<Catalog> catalog;

 public:
  virtual absl::Status Open();

  virtual void Close();

  virtual absl::StatusOr<bool> HasNext();

  virtual absl::StatusOr<Tuple> Next();
};

};  // namespace komfydb::storage

#endif  // __DB_FILE_ITERATOR_H__
