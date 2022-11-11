#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "absl/status/statusor.h"

#include "komfydb/storage/buffer_pool.h"
#include "komfydb/storage/catalog.h"

namespace {

using komfydb::storage::BufferPool;
using komfydb::storage::Catalog;

};  // namespace

namespace komfydb {

class Database {
 private:
  std::shared_ptr<Catalog> catalog;
  std::shared_ptr<BufferPool> buffer_pool;

  Database(std::shared_ptr<Catalog> catalog);

 public:
  static absl::StatusOr<Database> LoadSchema(absl::string_view catalog_file);

  std::shared_ptr<Catalog> GetCatalog();

  std::shared_ptr<BufferPool> GetBufferPool();
};

};  // namespace komfydb

#endif  // __DATABASE_H__
