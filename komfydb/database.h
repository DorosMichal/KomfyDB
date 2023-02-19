#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "absl/status/statusor.h"

#include "komfydb/execution/query.h"
#include "komfydb/optimizer/table_stats.h"
#include "komfydb/parser.h"
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
  std::shared_ptr<Parser> parser;
  optimizer::TableStatsMap table_stats_map;
  std::string catalog_directory;

  Database(std::shared_ptr<Catalog> catalog,
           std::shared_ptr<BufferPool> buffer_pool,
           std::shared_ptr<Parser> parser,
           optimizer::TableStatsMap&& table_stats_map,
           std::string_view catalog_directory);

  absl::Status CreateTable(Query& query);

 public:
  static std::unique_ptr<Database> Create(std::string_view catalog_directory);

  void Repl();

  absl::Status LoadSchema(std::string_view schema_path);

  std::shared_ptr<Parser> GetParser();

  std::shared_ptr<Catalog> GetCatalog();

  std::shared_ptr<BufferPool> GetBufferPool();

  optimizer::TableStatsMap& GetTableStatsMap();
};

};  // namespace komfydb

#endif  // __DATABASE_H__
