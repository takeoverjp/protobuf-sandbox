#include <google/protobuf/message.h>
#include <google/protobuf/util/field_mask_util.h>
#include <google/protobuf/util/message_differencer.h>

#include <fstream>
#include <iostream>
#include <vector>

#include "compare.new.pb.h"

using google::protobuf::FieldDescriptor;
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
  message->set_survived_field(3);

  Dump("message_1", *message);
}

static void InitializeMessageB(sandbox::CompareMessage* message) {
  message->set_survived_field(3);
  message->set_new_field(4);

  Dump("message_2", *message);
}

int main() {
  sandbox::CompareMessage message_1;
  InitializeMessageA(&message_1);
  sandbox::CompareMessage message_2;
  InitializeMessageB(&message_2);
  sandbox::CompareMessage message_old_1;
  LoadMessage("compare.old.data.1", &message_old_1);
  sandbox::CompareMessage message_old_2;
  LoadMessage("compare.old.data.2", &message_old_2);

  assert(MessageDifferencer::Equals(message_1, message_2) == false);
#if GOOGLE_PROTOBUF_VERSION > 3005000
  assert(MessageDifferencer::Equals(message_1, message_old_1) == false);
  assert(MessageDifferencer::Equals(message_1, message_old_2) == false);
  assert(MessageDifferencer::Equals(message_old_1, message_old_2) == false);
#else
  assert(MessageDifferencer::Equals(message_1, message_old_1) == true);
  assert(MessageDifferencer::Equals(message_1, message_old_2) == true);
  assert(MessageDifferencer::Equals(message_old_1, message_old_2) == true);
#endif

  assert(MessageDifferencer::Equivalent(message_1, message_2) == false);
  assert(MessageDifferencer::Equivalent(message_1, message_old_1) == true);
  assert(MessageDifferencer::Equivalent(message_1, message_old_2) == true);
  assert(MessageDifferencer::Equivalent(message_old_1, message_old_2) == true);

  MessageDifferencer diff;
  assert(diff.Compare(message_1, message_2) == false);
#if GOOGLE_PROTOBUF_VERSION > 3005000
  assert(diff.Compare(message_1, message_old_1) == false);
  assert(diff.Compare(message_1, message_old_2) == false);
  assert(diff.Compare(message_old_1, message_old_2) == false);
#else
  assert(diff.Compare(message_1, message_old_1) == true);
  assert(diff.Compare(message_1, message_old_2) == true);
  assert(diff.Compare(message_old_1, message_old_2) == true);
#endif

  diff.set_message_field_comparison(
      MessageDifferencer::MessageFieldComparison::EQUIVALENT);
  assert(diff.Compare(message_1, message_2) == false);
  assert(diff.Compare(message_1, message_old_1) == true);
  assert(diff.Compare(message_1, message_old_2) == true);
  assert(diff.Compare(message_old_1, message_old_2) == true);

  FieldMask mask;
  mask.add_paths("survived_field");
  std::vector<const FieldDescriptor*> fields;
  assert(
      FieldMaskUtil::GetFieldDescriptors(sandbox::CompareMessage::descriptor(),
                                         FieldMaskUtil::ToString(mask), &fields));

  diff.set_message_field_comparison(
      MessageDifferencer::MessageFieldComparison::EQUAL);
  assert(diff.CompareWithFields(message_1, message_2, fields, fields) == true);
  assert(diff.CompareWithFields(message_1, message_old_1, fields, fields) == true);
  assert(diff.CompareWithFields(message_1, message_old_2, fields, fields) == true);
  assert(diff.CompareWithFields(message_old_1, message_old_2, fields, fields) == true);

  diff.set_message_field_comparison(
      MessageDifferencer::MessageFieldComparison::EQUIVALENT);
  assert(diff.CompareWithFields(message_1, message_2, fields, fields) == true);
  assert(diff.CompareWithFields(message_1, message_old_1, fields, fields) == true);
  assert(diff.CompareWithFields(message_1, message_old_2, fields, fields) == true);
  assert(diff.CompareWithFields(message_old_1, message_old_2, fields, fields) == true);

  FieldMaskUtil::TrimMessage(mask, &message_1);
  FieldMaskUtil::TrimMessage(mask, &message_2);
  FieldMaskUtil::TrimMessage(mask, &message_old_1);
  FieldMaskUtil::TrimMessage(mask, &message_old_2);

  assert(MessageDifferencer::Equals(message_1, message_2) == true);
#if GOOGLE_PROTOBUF_VERSION > 3005000
  assert(MessageDifferencer::Equals(message_1, message_old_1) == false);
  assert(MessageDifferencer::Equals(message_1, message_old_2) == false);
  assert(MessageDifferencer::Equals(message_old_1, message_old_2) == false);
#else
  assert(MessageDifferencer::Equals(message_1, message_old_1) == true);
  assert(MessageDifferencer::Equals(message_1, message_old_2) == true);
  assert(MessageDifferencer::Equals(message_old_1, message_old_2) == true);
#endif

  assert(MessageDifferencer::Equivalent(message_1, message_2) == true);
  assert(MessageDifferencer::Equivalent(message_1, message_old_1) == true);
  assert(MessageDifferencer::Equivalent(message_1, message_old_2) == true);
  assert(MessageDifferencer::Equivalent(message_old_1, message_old_2) == true);
  return 0;
}