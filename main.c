#include <iostream>
#include <vector>
#include <google/protobuf/message.h>
#include <google/protobuf/util/field_mask_util.h>
#include "person.pb.h"

int
main (void) {
  tutorial::Person person;
  std::cerr << "tutorial::Person::descriptor()->full_name() : " << tutorial::Person::descriptor()->full_name() << std::endl;
  std::cerr << "person.GetTypeName() : " << person.New() << std::endl;
  std::cerr << "person.GetTypeName() : " << person.GetTypeName() << std::endl;
  std::cerr << "person.descriptor()->full_name() : " << person.descriptor()->full_name() << std::endl;
  std::cerr << "person.GetDescriptor()->full_name() : " << person.GetDescriptor()->full_name() << std::endl;
  std::cerr << "person.GetDescriptor()->DebugString() : " << person.GetDescriptor()->DebugString() << std::endl;

  std::vector<const google::protobuf::FieldDescriptor*>field_descriptors;

  google::protobuf::Message* message = &person;
  google::protobuf::Message* msg2 = message->New();
  std::cerr << "msg2->GetTypeName() : " << msg2->GetTypeName() << std::endl;
  dynamic_cast<tutorial::Person*>(msg2);
  
  const tutorial::TutorialOptions options = person.GetDescriptor()->options().GetExtension(tutorial::tutorial_options);
  std::cerr << "person.GetDescriptor()-> : " << options.DebugString() << std::endl;

  //  google::protobuf::util::FieldMaskUtil::GetFieldDescriptors(tutorial::Person::descriptor(), "PhoneNumber", &field_descriptors);
  // MessageDifferencer md;
  // md.CompareWithFields(msg1, msg2, field_descriptors, field_discriptors);
  return 0;
}
