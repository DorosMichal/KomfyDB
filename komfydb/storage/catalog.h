#ifndef __CATALOG_H__
#define __CATALOG_H__

#include <memory>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"

#include "komfydb/storage/db_file.h"

namespace komfydb::storage {

class Catalog {
 private:
  absl::flat_hash_map<int, std::unique_ptr<DbFile>> db_files;
  absl::flat_hash_map<int, std::string> table_names;
  absl::flat_hash_map<int, std::string> primary_keys;
  absl::flat_hash_map<std::string, int> table_name_to_id;

 public:
  Catalog() = default;

  // Add table to catalog. If table with given name exists, then remove it
  // and add this one. If file with given id exists, also replace it.
  void AddTable(std::unique_ptr<DbFile> file, std::string_view name,
                std::string_view primary_key);

  void AddTable(std::unique_ptr<DbFile> file, std::string_view name);

  void AddTable(std::unique_ptr<DbFile> file);

  absl::StatusOr<int> GetTableId(std::string_view name) const;

  absl::StatusOr<std::string> GetTableName(int table_id) const;

  absl::StatusOr<TupleDesc*> GetTupleDesc(int table_id) const;

  absl::StatusOr<std::string> GetPrimaryKey(int table_name) const;

  absl::StatusOr<DbFile*> GetDatabaseFile(int table_id) const;

  absl::StatusOr<DbFile*> GetDatabaseFile(std::string_view name);

  absl::StatusOr<DbFile*> CreateDatabase(std::string_view name,
                                         TupleDesc tuple_desc);

  std::vector<int> GetTableIds();

  void clear();
};

};  // namespace komfydb::storage

#endif  // __CATALOG_H__
