#include "komfydb/storage/catalog.h"

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"
#include "glog/logging.h"

#include "heap_file.h"
#include "komfydb/storage/db_file.h"
#include "komfydb/storage/heap_file.h"
#include "komfydb/utils/status_macros.h"
#include "komfydb/utils/utility.h"

namespace {

template <typename K, typename V>
absl::StatusOr<V> StatusOrMapElement(const absl::flat_hash_map<K, V>& map,
                                     const K& key) {
  if (!map.contains(key)) {
    return absl::InvalidArgumentError(
        absl::StrCat("No element for given key=", key));
  }
  return map.at(key);
}

};  // namespace

namespace komfydb::storage {

void Catalog::AddTable(std::unique_ptr<DbFile> file, std::string_view name,
                       std::string_view primary_key) {
  LOG(INFO) << "Adding table: tid=" << file->GetId() << " name=" << name
            << " pk=" << primary_key;
  int id = file->GetId();

  db_files[id] = std::move(file);
  table_names[id] = name;
  primary_keys[id] = primary_key;
  table_name_to_id[name] = id;
}

void Catalog::AddTable(std::unique_ptr<DbFile> file, std::string_view name) {
  AddTable(std::move(file), name, "");
}

void Catalog::AddTable(std::unique_ptr<DbFile> file) {
  AddTable(std::move(file), common::GenerateUuidV4());
}

absl::StatusOr<int> Catalog::GetTableId(std::string_view name) const {
  return StatusOrMapElement(table_name_to_id, std::string(name));
}

absl::StatusOr<std::string> Catalog::GetTableName(int table_id) const {
  return StatusOrMapElement(table_names, table_id);
}

absl::StatusOr<DbFile*> Catalog::GetDatabaseFile(int table_id) const {
  LOG(INFO) << "Getting DbFile with tid " << table_id << "\n";
  auto it = db_files.find(table_id);
  if (it == db_files.end()) {
    return absl::InvalidArgumentError(
        absl::StrCat("No table with id=", table_id));
  }
  return it->second.get();
}

absl::StatusOr<DbFile*> Catalog::GetDatabaseFile(std::string_view name) {
  ASSIGN_OR_RETURN(
      int table_id,
      StatusOrMapElement(table_name_to_id, static_cast<std::string>(name)));
  return GetDatabaseFile(table_id);
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
  for (auto& it : db_files) {
    result.push_back(it.first);
  }
  return result;
}

void clear();

};  // namespace komfydb::storage
