#include <iostream>
#include <string>

#include "common/type.h"
#include "glog/logging.h"

#include "komfydb/common/td_item.h"
#include "komfydb/common/type.h"

using namespace komfydb;

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  LOG(INFO) << "Welcome to KonfyDB!";

  common::Type t(common::Type::Value::INT);
  common::TDItem tditem(t, "name");

  LOG(INFO) << "Created tditem: " << (std::string)tditem;
}
