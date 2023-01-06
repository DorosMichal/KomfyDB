#include "komfydb/database.h"

#include <filesystem>
#include <fstream>
#include <string>

#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "glog/logging.h"

#include "komfydb/common/tuple_desc.h"
#include "komfydb/common/type.h"
#include "komfydb/storage/db_file.h"
#include "komfydb/storage/heap_file.h"
#include "komfydb/utils/status_macros.h"

namespace {

using komfydb::common::TupleDesc;
using komfydb::common::Type;
using komfydb::storage::DbFile;
using komfydb::storage::HeapFile;

inline void ltrim(std::string& s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));
}

// trim from end (in place)
inline void rtrim(std::string& s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

// trim from both ends (in place)
inline void trim(std::string& s) {
  ltrim(s);
  rtrim(s);
}

inline void ToLower(std::string& s) {
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
}

std::vector<std::string> SplitLine(std::string line, char delim) {
  std::vector<std::string> tokens;
  std::stringstream line_stream(line);
  std::string token;

  while (getline(line_stream, token, delim)) {
    trim(token);
    tokens.push_back(token);
  }
  return tokens;
}

absl::StatusOr<Type> GetType(std::string& type_str) {
  ToLower(type_str);
  if (type_str == "int") {
    return Type::INT;
  } else if (type_str == "string") {
    return Type::STRING;
  } else {
    return absl::InvalidArgumentError(
        absl::StrCat("Cannot parse type: ", type_str));
  }
}

};  // namespace

namespace komfydb {

Database::Database(std::shared_ptr<Catalog> catalog)
    : catalog(catalog), buffer_pool(std::make_shared<BufferPool>(catalog)) {}

absl::StatusOr<Database> Database::LoadSchema(
    absl::string_view catalog_file_path) {
  std::string directory =
      std::filesystem::path(catalog_file_path).parent_path().string();
  std::fstream catalog_file;
  catalog_file.open((std::string)catalog_file_path, std::ios::in);
  if (!catalog_file.good()) {
    return absl::InvalidArgumentError(
        absl::StrCat("Cannot open catalog file: ", catalog_file_path));
  }

  std::string line;
  std::shared_ptr<Catalog> catalog = std::make_shared<Catalog>();

  while (std::getline(catalog_file, line)) {
    std::string name = line.substr(0, line.find("("));
    line = line.substr(line.find("(") + 1);
    line.pop_back();  // delete last ')'
    trim(name);
    std::vector<std::string> tokens = SplitLine(line, ',');
    std::vector<std::string> names;
    std::vector<Type> types;
    std::string primary_key = "";
    for (auto token : tokens) {
      LOG(INFO) << "Parsing token " << token;
      trim(token);
      std::vector<std::string> parts = SplitLine(token, ' ');
      if (parts.size() < 2 || parts.size() > 4) {
        return absl::InvalidArgumentError(
            absl::StrCat("Cannot parse field description: ", token));
      }
      ASSIGN_OR_RETURN(Type type, GetType(parts[1]));
      types.push_back(type);
      trim(parts[0]);
      names.push_back(parts[0]);
      if (parts.size() == 3) {
        trim(parts[2]);
        ToLower(parts[2]);
        if (parts[2] == "pk") {
          primary_key = parts[0];
        } else {
          return absl::InvalidArgumentError(
              absl::StrCat("Unknown field attribute: ", parts[2]));
        }
      }
    }

    TupleDesc tuple_desc(types, names);
    ASSIGN_OR_RETURN(std::unique_ptr<HeapFile> hp,
                     HeapFile::Create(directory + "/" + name + ".dat",
                                      tuple_desc, Permissions::READ_ONLY));
    catalog->AddTable(std::move(hp), name, primary_key);
  }

  return Database(std::move(catalog));
}

std::shared_ptr<Catalog> Database::GetCatalog() {
  return catalog;
}

std::shared_ptr<BufferPool> Database::GetBufferPool() {
  return buffer_pool;
}

};  // namespace komfydb
