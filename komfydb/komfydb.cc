#include <iostream>
#include <string>

#include "absl/status/statusor.h"
#include "glog/logging.h"

#include "common/type.h"
#include "komfydb/common/td_item.h"
#include "komfydb/common/type.h"
#include "komfydb/database.h"
#include "komfydb/storage/heap_page.h"
#include "utils/status_macros.h"

using namespace komfydb;

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  LOG(INFO) << "Welcome to KonfyDB!";

  absl::StatusOr<Database> db =
      Database::LoadSchema("komfydb/testdata/database_catalog_test.txt");
  if (!db.ok()) {
    LOG(ERROR) << "LoadSchema error: " << db.status().message();
  }

  std::shared_ptr<Catalog> catalog = db->GetCatalog();
  std::vector<int> table_ids = catalog->GetTableIds();

  LOG(INFO) << "Tables:";
  for (auto table_id : table_ids) {
    LOG(INFO) << table_id << "->" << catalog->GetTableName(table_id).value();
    auto file = catalog->GetDatabaseFile(table_id).value();
    std::unique_ptr<storage::Page> page =
        file->ReadPage(storage::PageId(table_id, 0)).value();
    storage::HeapPage* hp = static_cast<storage::HeapPage*>(page.get());

    for (auto& record : hp->GetRecords()) {
      LOG(INFO) << static_cast<std::string>(record);
    }
  }
}
