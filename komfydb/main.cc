#include <setjmp.h>
#include <signal.h>
#include <unistd.h>

#include <readline/history.h>
#include <readline/readline.h>

#include <csignal>
#include <filesystem>
#include <iostream>

#include "gflags/gflags.h"
#include "glog/logging.h"

#include "hsql/SQLParser.h"
#include "hsql/sql/SelectStatement.h"
#include "hsql/util/sqlhelper.h"

#include "komfydb/common/td_item.h"
#include "komfydb/common/type.h"
#include "komfydb/database.h"
#include "komfydb/execution/executor.h"
#include "komfydb/execution/order_by.h"
#include "komfydb/execution/seq_scan.h"
#include "komfydb/parser.h"
#include "komfydb/storage/heap_file.h"
#include "komfydb/storage/heap_page.h"
#include "komfydb/storage/table_iterator.h"
#include "komfydb/utils/status_macros.h"

using namespace komfydb;

static bool ValidateDirectoryFlag(const char* flagname,
                                  const std::string& value) {
  if (!std::filesystem::is_directory(value) && value != "") {
    std::cout << "Invalid path for " << flagname << ": " << value << std::endl;
    return false;
  }
  return true;
}

DEFINE_string(schema_dir, "./", "Path to the database schema.");
DEFINE_validator(schema_dir, &ValidateDirectoryFlag);

DEFINE_string(schema_file, "schema.txt",
              "Schema file. This file should be a list of \'CREATE TABLE\' "
              "commands. The tables are created/loaded from catalog_path (the "
              "same as schema_path by default)");

DEFINE_string(catalog_dir, "",
              "Path to tables\' files (by default the same as schema_path).");
DEFINE_validator(catalog_dir, &ValidateDirectoryFlag);

DEFINE_bool(load_schema, true,
            "Specifies if a schema file will be read or not.");

void HandlerPrint(std::string str) {
  write(STDOUT_FILENO, str.c_str(), str.length());
}

sigjmp_buf env;
void SigintHandler(int signum) {
  HandlerPrint(
      "Sigint received, terminating... (CTRL+C for immediate termination)\n");
  siglongjmp(env, 1);
}

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  if (FLAGS_catalog_dir == "") {
    FLAGS_catalog_dir = FLAGS_schema_dir;
  }

  std::filesystem::path catalog_path(FLAGS_catalog_dir);
  std::filesystem::path schema_path(FLAGS_schema_dir);

  absl::Status status;
  std::unique_ptr<Database> db = Database::Create(catalog_path);
  if (FLAGS_load_schema &&
      !(status = db->LoadSchema(schema_path / FLAGS_schema_file)).ok()) {
    std::cout << "Schema loading error: " << status.message();
    return 1;
  }

  struct sigaction action = {};
  action.sa_handler = &SigintHandler;
  // So the signal disposition is restored back to default after the first
  // handling of SIGINT.
  action.sa_flags = SA_RESETHAND;
  sigaction(SIGINT, &action, NULL);

  int result = sigsetjmp(env, 0);
  if (result) {
    return 0;
  }

  db->Repl();
}
