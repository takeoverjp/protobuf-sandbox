#include <google/protobuf/message.h>
#include <google/protobuf/util/field_mask_util.h>

#include <iostream>
#include <vector>

#include "merge.pb.h"

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

static void MergeSimply() {
  sandbox::MergeMessage message_a;
  message_a.set_int32_field_from_a(1);
  message_a.set_string_field_from_a("From message_a");
  Dump("message_a", message_a);

  sandbox::MergeMessage message_b;
  message_b.set_int32_field_from_b(2);
  message_b.set_string_field_from_b("From message_b");
  Dump("message_b", message_b);

  FieldMask mask;
  mask.add_paths("int32_field_from_a");
  mask.add_paths("string_field_from_a");
  Dump(mask);

  FieldMaskUtil::MergeOptions merge_options;
  FieldMaskUtil::MergeMessageTo(message_a, mask, merge_options, &message_b);

  Dump("message_b (after merge)", message_b);
}

static void MergeWithReplaceMessageFieldsOption(bool replace_message_fields) {
  sandbox::MergeMessage message_a;
  message_a.mutable_message_field()->set_from_a("From message_a");
  Dump("message_a", message_a);

  sandbox::MergeMessage message_b;
  message_b.mutable_message_field()->set_from_b("From message_b");
  Dump("message_b", message_b);

  FieldMask mask;
  mask.add_paths("message_field");
  // Dump(mask);

  FieldMaskUtil::MergeOptions merge_options;
  merge_options.set_replace_message_fields(replace_message_fields);
  Dump(merge_options);

  FieldMaskUtil::MergeMessageTo(message_a, mask, merge_options, &message_b);

  Dump("message_b (after merge)", message_b);
}

static void MergeWithReplaceRepeatedFieldsOption(bool replace_repeated_fields) {
  sandbox::MergeMessage message_a;
  sandbox::MergeMessage::MessageField* field =
      message_a.add_repeated_message_field();
  field->set_from_a("From message_a 0");
  field = message_a.add_repeated_message_field();
  field->set_from_a("From message_a 1");
  Dump("message_a", message_a);

  sandbox::MergeMessage message_b;
  field = message_b.add_repeated_message_field();
  field->set_from_b("From message_b 0");
  field = message_b.add_repeated_message_field();
  field->set_from_b("From message_b 1");
  Dump("message_b", message_b);

  FieldMask mask;
  mask.add_paths("repeated_message_field");
  // Dump(mask);

  FieldMaskUtil::MergeOptions merge_options;
  merge_options.set_replace_repeated_fields(replace_repeated_fields);
  Dump(merge_options);

  FieldMaskUtil::MergeMessageTo(message_a, mask, merge_options, &message_b);

  Dump("message_b (after merge)", message_b);
}

static void MergeEachRepeatedField() {
  sandbox::MergeMessage message_a;
  sandbox::MergeMessage::MessageField* field =
      message_a.add_repeated_message_field();
  field->set_from_a("From message_a 0");
  field = message_a.add_repeated_message_field();
  field->set_from_a("From message_a 1");
  Dump("message_a", message_a);

  sandbox::MergeMessage message_b;
  field = message_b.add_repeated_message_field();
  field->set_from_b("From message_b 0");
  field = message_b.add_repeated_message_field();
  field->set_from_b("From message_b 1");
  Dump("message_b", message_b);

  FieldMask mask;
  mask.add_paths("from_a");
  Dump(mask);

  FieldMaskUtil::MergeOptions merge_options;

  assert(message_a.repeated_message_field_size() ==
         message_b.repeated_message_field_size());
  int len = message_a.repeated_message_field_size();
  for (int i = 0; i < len; i++) {
    FieldMaskUtil::MergeMessageTo(message_a.repeated_message_field(i), mask,
                                  merge_options,
                                  message_b.mutable_repeated_message_field(i));
  }

  Dump("message_b (after merge)", message_b);
}

int main(int argc, char* argv[]) {
  std::string mode;
  if (argc >= 2) {
    mode = argv[1];
  }

  if (mode == "option_field") {
    MergeWithReplaceMessageFieldsOption(false);
    MergeWithReplaceMessageFieldsOption(true);
  } else if (mode == "option_repeated") {
    MergeWithReplaceRepeatedFieldsOption(false);
    MergeWithReplaceRepeatedFieldsOption(true);
  } else if (mode == "repeat") {
    MergeEachRepeatedField();
  } else {
    MergeSimply();
  }
  return 0;
}
