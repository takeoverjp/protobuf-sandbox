#include <google/protobuf/message.h>
#include <google/protobuf/util/field_mask_util.h>

#include <iostream>
#include <vector>

#include "merge_qiita.pb.h"

using google::protobuf::FieldMask;
using google::protobuf::Message;
using google::protobuf::util::FieldMaskUtil;

static void Dump(const FieldMask& mask) {
  std::cout << "[FieldMask]" << std::endl << mask.DebugString() << std::endl;
}

static void Dump(const FieldMaskUtil::MergeOptions& merge_options) {
  std::cout << "[FieldMaskUtil::MergeOptions]" << std::endl
            << "replace_message_fields : "
            << merge_options.replace_message_fields() << std::endl
            << "replace_repeated_fields : "
            << merge_options.replace_repeated_fields() << std::endl
            << std::endl;
}

static void Dump(const std::string& title, const Message& message) {
  std::cout << "[" << title << "]" << std::endl
            << message.DebugString() << std::endl;
}

static void InitializeMessageA(sandbox::MergeQiitaMessage* message) {
  message->set_string_field("a");
  message->set_bool_field(false);
  message->add_repeated_int_field(1);
  message->add_repeated_int_field(2);
  message->add_repeated_int_field(3);
  message->mutable_embedded_message_field()->set_int_field(1);

  auto field = message->add_repeated_embedded_message_field();
  field->set_string_field("a 1");
  field = message->add_repeated_embedded_message_field();
  field->set_string_field("a 2");
  field = message->add_repeated_embedded_message_field();
  field->set_string_field("a 3");

  // Dump("message_a", *message);
}

static void InitializeMessageB(sandbox::MergeQiitaMessage* message) {
  message->set_string_field("b");
  message->set_int_field(2);
  message->set_bool_field(true);
  message->add_repeated_int_field(4);
  message->add_repeated_int_field(5);
  message->add_repeated_int_field(6);
  message->mutable_embedded_message_field()->set_string_field("b");

  auto field = message->add_repeated_embedded_message_field();
  field->set_int_field(1);
  field = message->add_repeated_embedded_message_field();
  field->set_int_field(2);
  field = message->add_repeated_embedded_message_field();
  field->set_int_field(3);

  // Dump("message_b", *message);
}

static void MergeSimply() {
  sandbox::MergeQiitaMessage message_a;
  InitializeMessageA(&message_a);

  sandbox::MergeQiitaMessage message_b;
  InitializeMessageB(&message_b);

  message_b.MergeFrom(message_a);
  Dump("message_b (after merge)", message_b);
}

static void MergeWithFieldMask() {
  sandbox::MergeQiitaMessage message_a;
  InitializeMessageA(&message_a);

  sandbox::MergeQiitaMessage message_b;
  InitializeMessageB(&message_b);

  FieldMask mask;
  mask.add_paths("string_field");
  mask.add_paths("bool_field");
  mask.add_paths("repeated_int_field");
  mask.add_paths("embedded_message_field.int_field");
  Dump(mask);

  FieldMaskUtil::MergeOptions merge_options;
  FieldMaskUtil::MergeMessageTo(message_a, mask, merge_options, &message_b);

  Dump("message_b (after merge)", message_b);
}

static void MergeWithReplaceMessageFieldsOption(bool replace_message_fields) {
  sandbox::MergeQiitaMessage message_a;
  InitializeMessageA(&message_a);

  sandbox::MergeQiitaMessage message_b;
  InitializeMessageB(&message_b);

  FieldMask mask;
  mask.add_paths("embedded_message_field");
  Dump(mask);

  FieldMaskUtil::MergeOptions merge_options;
  merge_options.set_replace_message_fields(replace_message_fields);
  Dump(merge_options);

  FieldMaskUtil::MergeMessageTo(message_a, mask, merge_options, &message_b);

  Dump("message_b (after merge)", message_b);
}

static void MergeWithReplaceRepeatedFieldsOption(bool replace_repeated_fields) {
  sandbox::MergeQiitaMessage message_a;
  InitializeMessageA(&message_a);

  sandbox::MergeQiitaMessage message_b;
  InitializeMessageB(&message_b);

  FieldMask mask;
  mask.add_paths("string_field");
  mask.add_paths("bool_field");
  mask.add_paths("repeated_int_field");
  mask.add_paths("embedded_message_field.int_field");
  Dump(mask);

  FieldMaskUtil::MergeOptions merge_options;
  merge_options.set_replace_repeated_fields(replace_repeated_fields);
  Dump(merge_options);

  FieldMaskUtil::MergeMessageTo(message_a, mask, merge_options, &message_b);

  Dump("message_b (after merge)", message_b);
}

static void MergeEachRepeatedField() {
  sandbox::MergeQiitaMessage message_a;
  InitializeMessageA(&message_a);

  sandbox::MergeQiitaMessage message_b;
  InitializeMessageB(&message_b);

  FieldMask mask;
  mask.add_paths("string_field");
  Dump(mask);

  FieldMaskUtil::MergeOptions merge_options;

  assert(message_a.repeated_embedded_message_field_size() ==
         message_b.repeated_embedded_message_field_size());
  int len = message_a.repeated_embedded_message_field_size();
  for (int i = 0; i < len; i++) {
    FieldMaskUtil::MergeMessageTo(message_a.repeated_embedded_message_field(i), mask,
                                  merge_options,
                                  message_b.mutable_repeated_embedded_message_field(i));
  }

  Dump("message_b (after merge)", message_b);
}

int main(int argc, char* argv[]) {
  std::string mode;
  if (argc >= 2) {
    mode = argv[1];
  }

  if (mode == "field_mask") {
    MergeWithFieldMask();
  } else if (mode == "option_message") {
    MergeWithReplaceMessageFieldsOption(false);
    MergeWithReplaceMessageFieldsOption(true);
  } else if (mode == "option_repeated") {
    MergeWithReplaceRepeatedFieldsOption(false);
    MergeWithReplaceRepeatedFieldsOption(true);
  } else if (mode == "repeated") {
    MergeEachRepeatedField();
  } else {
    MergeSimply();
  }
  return 0;
}
