#include <google/protobuf/message.h>
#include <google/protobuf/util/field_mask_util.h>
#include <google/protobuf/util/message_differencer.h>

#include <fstream>
#include <iostream>
#include <vector>

#include "compare.new.pb.h"

using google::protobuf::FieldMask;
using google::protobuf::Message;
using google::protobuf::util::FieldMaskUtil;
using google::protobuf::util::MessageDifferencer;
using std::ios;

static void Dump(const FieldMask& mask) {
  std::cout << "[FieldMask]" << std::endl << mask.DebugString() << std::endl;
}

static void Dump(const std::string& title, const Message& message) {
  std::cout << "[" << title << "]" << std::endl
            << message.DebugString() << std::endl;
}

static void LoadMessage(const std::string& name,
                        sandbox::CompareMessage* message) {
  std::fstream input(name, ios::in | ios::binary);
  assert(input);
  if (!message->ParseFromIstream(&input)) {
    std::cerr << "Failed to parse address book." << std::endl;
  }

  Dump("message_old", *message);
}

static void InitializeMessageA(sandbox::CompareMessage* message) {
  message->set_new_field(3);

  Dump("message_a", *message);
}

static void InitializeMessageB(sandbox::CompareMessage* message) {
  message->set_new_field(4);

  Dump("message_b", *message);
}

static void Compare() {
  sandbox::CompareMessage message_a;
  InitializeMessageA(&message_a);
  sandbox::CompareMessage message_b;
  InitializeMessageB(&message_b);
  sandbox::CompareMessage message_old_1;
  LoadMessage("compare.old.data.1", &message_old_1);
  sandbox::CompareMessage message_old_2;
  LoadMessage("compare.old.data.2", &message_old_2);

  assert(MessageDifferencer::Equals(message_a, message_b) == false);
  assert(MessageDifferencer::Equals(message_a, message_old_1) == false);
  assert(MessageDifferencer::Equals(message_a, message_old_2) == false);
  assert(MessageDifferencer::Equals(message_old_1, message_old_2) == false);

  assert(MessageDifferencer::Equivalent(message_a, message_b) == false);
  assert(MessageDifferencer::Equivalent(message_a, message_old_1) == true);
  assert(MessageDifferencer::Equivalent(message_a, message_old_2) == true);
  assert(MessageDifferencer::Equivalent(message_old_1, message_old_2) == true);

  MessageDifferencer diff;
  assert(diff.Compare(message_a, message_b) == false);
  assert(diff.Compare(message_a, message_old_1) == false);
  assert(diff.Compare(message_a, message_old_2) == false);
  assert(diff.Compare(message_old_1, message_old_2) == false);
}

int main() {
  Compare();
  return 0;
}