#include <iostream>
#include "glog/logging.h"

int main(int argc, char *argv[]) {
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);


  LOG(INFO) << "Welcome to KonfyDB!";
}
