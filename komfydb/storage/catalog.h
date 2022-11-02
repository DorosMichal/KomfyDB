#ifndef __CATALOG_H__
#define __CATALOG_H__

#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"

#include "komfydb/storage/db_file.h"

namespace komfydb::storage {

class Catalog {
 private:
  absl::flat_hash_map<int, DbFile*> db_files;
  absl::flat_hash_map<int, std::string> table_names;
  absl::flat_hash_map<int, std::string> primary_keys;
  absl::flat_hash_map<std::string, int> table_name_to_id;

 public:
  Catalog() = default;

  // Deletes all DbFiles in db_files.
  ~Catalog();

  // Add table to catalog. If table with given name exists, then remove it
  // and add this one. If file with given id exists, also replace it.
  void AddTable(DbFile* file, std::string name, std::string primary_key);

  void AddTable(DbFile* file, std::string name);

  void AddTable(DbFile* file);

  absl::StatusOr<int> GetTableId(std::string name) const;

  absl::StatusOr<std::string> GetTableName(int table_id) const;

  absl::StatusOr<TupleDesc*> GetTupleDesc(int table_id) const;

  absl::StatusOr<std::string> GetPrimaryKey(int table_name) const;

  absl::StatusOr<DbFile*> GetDatabaseFile(int table_id) const;

  // TODO(Iterator)
  std::vector<int> GetTableIds();

  void clear();
};

};  // namespace komfydb::storage

#endif  // __CATALOG_H__
