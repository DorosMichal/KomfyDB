#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"

#include "komfydb/utils/status_macros.h"
#include "komfydb/utils/utility.h"
#include "komfydb/storage/catalog.h"

namespace {

template <typename K, typename V>
absl::StatusOr<V> StatusOrMapElement(const absl::flat_hash_map<K, V>& map,
                                     const K& value) {
  auto it = map.find(value);
  if (it == map.end()) {
    return absl::InvalidArgumentError("No table with given name");
  }
  return it->second;
}

};  // namespace

namespace komfydb::storage {

Catalog::~Catalog() {
  for (auto it : db_files) {
    delete it.second;
  }
}

void Catalog::AddTable(DbFile* file, std::string name,
                       std::string primary_key) {
  int id = file->GetId();

  db_files[id] = file;
  table_names[id] = name;
  primary_keys[id] = primary_key;
  table_name_to_id[name] = id;
}

void Catalog::AddTable(DbFile* file, std::string name) {
  AddTable(std::move(file), name, "");
}

void Catalog::AddTable(DbFile* file) {
  AddTable(std::move(file), common::GenerateUuidV4());
}

absl::StatusOr<int> Catalog::GetTableId(std::string name) const {
  return StatusOrMapElement(table_name_to_id, name);
}

absl::StatusOr<std::string> Catalog::GetTableName(int table_id) const {
  return StatusOrMapElement(table_names, table_id);
}

absl::StatusOr<DbFile*> Catalog::GetDatabaseFile(int table_id) const {
  return StatusOrMapElement(db_files, table_id);
}

absl::StatusOr<TupleDesc*> Catalog::GetTupleDesc(int table_id) const {
  ASSIGN_OR_RETURN(DbFile * file, GetDatabaseFile(table_id));
  return file->GetTupleDesc();
}

absl::StatusOr<std::string> Catalog::GetPrimaryKey(int table_id) const {
  return StatusOrMapElement(primary_keys, table_id);
}

std::vector<int> Catalog::GetTableIds() {
  std::vector<int> result;
  for (auto it : db_files) {
    result.push_back(it.first);
  }
  return result;
}

void clear();

};  // namespace komfydb::storage
