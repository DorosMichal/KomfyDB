#include <iostream>
#include <thread>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <sockpp/tcp_acceptor.h>
#include <sockpp/version.h>

#include "komfydb/config.h"

DEFINE_int32(port, CONFIG_SERVER_PORT, "KomfyDB's server listening port.");

extern const char komfydb_logo[];

void worker(sockpp::tcp_socket sock) {
  ssize_t n;
  char buf[512];
  while ((n = sock.read(buf, sizeof(buf))) > 0)
    sock.write_n(buf, n);

  LOG(INFO) << "Connection closed from " << sock.peer_address();
}

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  sockpp::initialize();
  sockpp::tcp_acceptor acc(FLAGS_port);

  LOG(INFO) << komfydb_logo;

  if (!acc) {
    LOG(ERROR) << "Error creating tcp acceptor: " << acc.last_error_str();
    return 1;
  }
  LOG(INFO) << "KomfyDB is listening on port " << FLAGS_port;

  while (true) {
    sockpp::inet_address peer;

    sockpp::tcp_socket sock = acc.accept(&peer);
    LOG(INFO) << "Received a connection request from " << peer;

    if (!sock) {
      LOG(ERROR) << "Error accepting incoming connection: "
                 << acc.last_error_str();
    } else {
      std::thread thr(worker, std::move(sock));
      thr.detach();
    }
  }

  LOG(INFO) << "Bye bye!";
}
