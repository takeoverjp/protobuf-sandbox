#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/generated_message_reflection.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

#include "sample.pb.h"

using ::google::protobuf::Descriptor;
using ::google::protobuf::DynamicMessageFactory;
using ::google::protobuf::Message;
using ::sandbox::SampleMessage;

const std::string kMessageFile = "message.data";

static void StoreMessage(const std::string& name, const Message& message) {}

static void SetUp() {
  sandbox::SampleMessage message;
  message.set_string_field("string field");
  message.set_int_field(2);
  message.set_bool_field(true);
  message.add_repeated_int_field(4);
  message.add_repeated_int_field(5);
  message.add_repeated_int_field(6);
  message.mutable_embedded_message_field()->set_string_field(
      "embedded string field");
  message.mutable_embedded_message_field()->set_int_field(7);

  {
    std::ofstream output(kMessageFile, std::ios::trunc | std::ios::binary);
    if (!message.SerializeToOstream(&output)) {
      std::cerr << "Failed to write message." << std::endl;
    }
  }
}

static void TearDown() { unlink(kMessageFile.c_str()); }

static std::unique_ptr<Message> LoadMessageBasedOnDescriptor(
    DynamicMessageFactory* factory, const std::string file_name,
    const Descriptor* descriptor) {
  if ((factory == nullptr) || (descriptor == nullptr)) {
    return nullptr;
  }

  // 1. DynamicMessageFactory::GetPrototypeで、Descriptorからconst Messageを取得
  const Message* prototype = factory->GetPrototype(descriptor);
  if (prototype == nullptr) {
    return nullptr;
  }

  // 2. Message::Newで、mutableなMessageを取得
  //    得たMessageは呼び出し元が解放しなければならないので、unique_ptrで受ける
  auto message = std::unique_ptr<Message>(prototype->New());
  if (message == nullptr) {
    return nullptr;
  }

  // 3. Message::ParseFromXXXで、Messageをdeserialize
  {
    std::ifstream input(file_name, std::ios::binary);
    message->ParseFromIstream(&input);
  }

  return message;
}

int main() {
  SetUp();

  SampleMessage sample;

  {
    DynamicMessageFactory factory;
    std::unique_ptr<Message> message = LoadMessageBasedOnDescriptor(
        &factory, kMessageFile, SampleMessage::descriptor());
    sample.CopyFrom(*message);

    // 4. ここでDynamicMessageFactory解放
    //    以降、factoryから生成したmessageは参照してはいけない
  }

  std::cout << sample.DebugString() << std::endl;

  google::protobuf::ShutdownProtobufLibrary();

  TearDown();

  return 0;
}