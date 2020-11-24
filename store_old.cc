#include <fstream>
#include <iostream>
#include <string>

#include "compare.old.pb.h"

using google::protobuf::Message;
using std::ios;

static void StoreMessage(const std::string& name, const Message& message) {
  std::fstream output(name, ios::out | ios::trunc | ios::binary);
  if (!message.SerializeToOstream(&output)) {
    std::cerr << "Failed to write message." << std::endl;
  }
}

static void Store(const std::string& name, int old_field, int new_field) {
  sandbox::CompareMessage message;
  message.set_old_field(old_field);
  message.set_survived_field(new_field);
  StoreMessage(name, message);
}

int main() {
  Store("compare.old.data.1", 1, 3);
  Store("compare.old.data.2", 2, 3);

  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}