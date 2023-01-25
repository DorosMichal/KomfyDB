#include <hsql/sql/SelectStatement.h>
#include <readline/history.h>
#include <readline/readline.h>

#include <iostream>

#include "execution/logical_plan/logical_plan.h"
#include "execution/op_iterator.h"
#include "glog/logging.h"
#include "hsql/SQLParser.h"
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
#include "optimizer/table_stats.h"
#include "transaction/transaction_id.h"
#include "utils/status_macros.h"

using namespace komfydb;

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);

  const std::string catalog_directory = "komfydb/testdata/";
  std::unique_ptr<Database> db = Database::Create(catalog_directory);

  db->TestRepl();
}
