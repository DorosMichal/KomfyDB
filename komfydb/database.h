#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <filesystem>

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
  std::filesystem::path catalog_directory;

  Database(std::shared_ptr<Catalog> catalog,
           std::shared_ptr<BufferPool> buffer_pool,
           std::shared_ptr<Parser> parser,
           optimizer::TableStatsMap&& table_stats_map,
           std::filesystem::path catalog_directory);

  absl::Status CreateTable(Query& query);

 public:
  static std::unique_ptr<Database> Create(
      std::filesystem::path catalog_directory);

  void Repl();

  void TestRepl();

  absl::Status LoadSchema(std::filesystem::path schema_path);

  std::shared_ptr<Parser> GetParser();

  std::shared_ptr<Catalog> GetCatalog();

  std::shared_ptr<BufferPool> GetBufferPool();

  optimizer::TableStatsMap& GetTableStatsMap();
};

};  // namespace komfydb

#endif  // __DATABASE_H__
