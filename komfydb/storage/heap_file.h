#ifndef __HEAP_FILE_H__
#define __HEAP_FILE_H__

#include <iostream>
#include <fstream>
#include <list>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/storage/db_file.h"
#include "komfydb/storage/page.h"
#include "komfydb/storage/page_id.h"
#include "komfydb/storage/db_file_iterator.h"
#include "komfydb/transaction/transaction_id.h"
#include "komfydb/common/tuple_desc.h"

namespace {

using komfydb::common::Tuple;
using komfydb::common::TupleDesc;
using komfydb::transaction::TransactionId;

};  // namespace

namespace komfydb::storage {

class HeapFile : DbFile {
 private:
  std::fstream file;

 public:
  HeapFile(std::fstream& file, TupleDesc td);

  std::fstream& GetFile();

  Page ReadPage(PageId id) override;

  absl::Status WritePage(Page p) override;

  absl::StatusOr<std::list<Page>> InsertTuple(TransactionId tid,
                                                      Tuple t) override;

  absl::StatusOr<std::list<Page>> DeleteTuple(TransactionId tid,
                                                      Tuple t) override;

  DbFileIterator Iterator(TransactionId tid) override;

  int GetId() override;

  TupleDesc GetTupleDesc() override;
};

};

#endif  // __HEAP_FILE_H__
