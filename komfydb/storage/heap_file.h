#ifndef __HEAP_FILE_H__
#define __HEAP_FILE_H__

#include <fstream>
#include <iostream>
#include <list>
#include <memory>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "komfydb/common/permissions.h"
#include "komfydb/common/tuple.h"
#include "komfydb/common/tuple_desc.h"
#include "komfydb/storage/db_file.h"
#include "komfydb/storage/db_file_iterator.h"
#include "komfydb/storage/page.h"
#include "komfydb/storage/page_id.h"
#include "komfydb/transaction/transaction_id.h"

namespace {

using komfydb::common::Permissions;
using komfydb::common::TupleDesc;

};  // namespace

namespace komfydb::storage {

class HeapFile : public DbFile {
 private:
  std::fstream file;
  TupleDesc td;
  uint32_t table_id;
  Permissions permissions;

  // TODO This is probably a very bad way to create table_id's, it's not
  // thread safe. Probably it would be better to get some file's hash code.
  static uint32_t table_cnt;

  HeapFile(std::fstream file, TupleDesc td, uint32_t table_id,
           Permissions permissions);

 public:
  ~HeapFile();

  static absl::StatusOr<std::unique_ptr<HeapFile>> Create(
      const absl::string_view file_name, TupleDesc td, Permissions permissions);

  std::fstream* GetFile();

  absl::StatusOr<std::unique_ptr<Page>> ReadPage(PageId id) override;

  // absl::Status WritePage(Page* p) override;

  // absl::StatusOr<std::vector<Page*>> InsertTuple(TransactionId tid,
  //                                   Tuple t) override;

  // absl::StatusOr<std::vector<Page*>> DeleteTuple(TransactionId tid,
  //                                   Tuple t) override;

  uint32_t GetId() override;

  TupleDesc* GetTupleDesc() override;

  int GetNumPages() override;
};

};  // namespace komfydb::storage

#endif  // __HEAP_FILE_H__
