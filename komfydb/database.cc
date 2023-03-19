#include "komfydb/database.h"

#include <readline/history.h>
#include <readline/readline.h>

#include <filesystem>
#include <fstream>
#include <string>

#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "glog/logging.h"

#include "komfydb/common/tuple_desc.h"
#include "komfydb/common/type.h"
#include "komfydb/execution/executor.h"
#include "komfydb/execution/query.h"
#include "komfydb/optimizer/table_stats.h"
#include "komfydb/storage/db_file.h"
#include "komfydb/storage/heap_file.h"
#include "komfydb/transaction/transaction_id.h"
#include "komfydb/utils/status_macros.h"

namespace {

std::string ReadInputLine() {
  if (isatty(STDIN_FILENO)) {
    while (true) {
      char* input = readline("KomfyDB> ");
      if (input == nullptr) {
        return "";
      }
      if (!strlen(input)) {
        free(input);
        continue;
      }
      add_history(input);
      return input;
    }
  } else {
    while (true) {
      std::string result;
      if (!std::getline(std::cin, result)) {
        return "";
      }
      if (result == "") {
        continue;
      }
      return result;
    }
  }
}

};  // namespace

namespace komfydb {

Database::Database(std::shared_ptr<Catalog> catalog,
                   std::shared_ptr<BufferPool> buffer_pool,
                   std::shared_ptr<Parser> parser,
                   optimizer::TableStatsMap&& table_stats_map,
                   std::string_view catalog_directory)
    : catalog(catalog),
      buffer_pool(buffer_pool),
      parser(parser),
      table_stats_map(std::move(table_stats_map)),
      catalog_directory(catalog_directory) {}

std::unique_ptr<Database> Database::Create(std::string_view catalog_directory) {
  std::shared_ptr<Catalog> catalog = std::make_shared<Catalog>();
  std::shared_ptr<BufferPool> buffer_pool =
      std::make_shared<BufferPool>(catalog);
  optimizer::TableStatsMap table_stats_map;
  std::shared_ptr<Parser> parser =
      std::make_shared<Parser>(catalog, buffer_pool, table_stats_map);
  return std::unique_ptr<Database>(new Database(catalog, buffer_pool, parser,
                                                std::move(table_stats_map),
                                                catalog_directory));
}

absl::Status Database::CreateTable(Query& query) {
  if (catalog->GetTableId(query.table_name).ok()) {
    return absl::InvalidArgumentError(
        absl::StrCat("Table ", query.table_name, " already exists."));
  }
  absl::StatusOr<std::unique_ptr<DbFile>> new_file = HeapFile::Create(
      absl::StrCat(catalog_directory, query.table_name, ".dat"),
      query.tuple_desc, Permissions::READ_WRITE);
  if (!new_file.status().ok()) {
    return absl::InvalidArgumentError(
        absl::StrCat("Cannot create table: ", new_file.status().message()));
  }

  catalog->AddTable(std::move(*new_file), query.table_name, query.primary_key);
  return absl::OkStatus();
}

absl::Status Database::LoadSchema(std::string_view schema_path) {
  LOG(INFO) << "Loading schema " << schema_path;
  std::fstream schema_file;
  schema_file.open((std::string)schema_path, std::ios::in);

  if (!schema_file.good()) {
    return absl::InvalidArgumentError(
        absl::StrCat("Cannot open catalog file: ", schema_path));
  }

  std::string command;
  while (std::getline(schema_file, command)) {
    ASSIGN_OR_RETURN(
        Query query,
        parser->ParseQuery(command, TransactionId(transaction::NO_TID), false));
    if (query.type != Query::CREATE_TABLE) {
      return absl::InvalidArgumentError(absl::StrCat(
          "Invalid command in schema, only CREATE TABLE is allowed: ",
          command));
    }
    RETURN_IF_ERROR(CreateTable(query));
  }
  return absl::OkStatus();
}

void Database::Repl() {
  execution::Executor executor;
  absl::Status status;
  std::string query_str;

  while ((query_str = ReadInputLine()) != "") {
    hsql::SQLParserResult result;
    hsql::SQLParser::parse(query_str, &result);

    absl::StatusOr<Query> query =
        parser->ParseQuery(query_str, TransactionId(), false);
    if (!query.ok()) {
      std::cout << "Parsing error: " << query.status().message() << std::endl;
      continue;
    }

    switch (query->type) {
      case Query::ITERATOR: {
        query->iterator->Explain(std::cout);
        status = executor.PrettyExecute(std::move(query->iterator));
        if (!status.ok()) {
          std::cout << "Executor error: " << status.message() << std::endl;
        }
        break;
      }
      case Query::CREATE_TABLE: {
        status = CreateTable(*query);
        if (!status.ok()) {
          std::cout << "Create table error: " << status.message() << std::endl;
        }
        break;
      }
    }
  }
}

void Database::TestRepl() {
  execution::Executor executor;
  absl::Status status;
  char* input;

  while ((input = readline("")) != nullptr) {
    if (!strlen(input)) {
      continue;
    }
    std::string query_str = input;
    free(input);

    hsql::SQLParserResult result;
    hsql::SQLParser::parse(query_str, &result);

    absl::StatusOr<Query> query =
        parser->ParseQuery(query_str, TransactionId(), false);
    if (!query.ok()) {
      std::cout << "Parsing error: " << query.status().message() << std::endl;
      continue;
    }

    switch (query->type) {
      case Query::ITERATOR: {
        status = executor.PythonExecute(std::move(query->iterator));
        if (!status.ok()) {
          std::cout << "Executor error: " << status.message() << std::endl;
        }
        break;
      }
      case Query::CREATE_TABLE: {
        status = CreateTable(*query);
        if (!status.ok()) {
          std::cout << "Create table error: " << status.message() << std::endl;
        }
        break;
      }
    }
  }
}

std::shared_ptr<Parser> Database::GetParser() {
  return parser;
}

std::shared_ptr<Catalog> Database::GetCatalog() {
  return catalog;
}

std::shared_ptr<BufferPool> Database::GetBufferPool() {
  return buffer_pool;
}

optimizer::TableStatsMap& Database::GetTableStatsMap() {
  return table_stats_map;
}

};  // namespace komfydb
