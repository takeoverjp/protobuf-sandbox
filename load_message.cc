#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/util/message_differencer.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

#include "sample.pb.h"

using ::google::protobuf::Descriptor;
using ::google::protobuf::DynamicCastToGenerated;
using ::google::protobuf::DynamicMessageFactory;
using ::google::protobuf::Message;
using ::google::protobuf::util::MessageDifferencer;
using ::sandbox::SampleMessage;

static std::string serialized_message;
static const int kTestCount = 1000000;

static void StoreMessage(const std::string& name, const Message& message) {}

static void InitializeSampleMessage(SampleMessage* message) {
  message->set_string_field("string field");
  message->set_int_field(2);
  message->set_bool_field(true);
  message->add_repeated_int_field(4);
  message->add_repeated_int_field(5);
  message->add_repeated_int_field(6);
  message->mutable_embedded_message_field()->set_string_field(
      "embedded string field");
  message->mutable_embedded_message_field()->set_int_field(7);
}

static void SetUp() {
  sandbox::SampleMessage message;
  InitializeSampleMessage(&message);
  serialized_message = message.SerializeAsString();
}

static void TearDown() {}

static void CheckSampleMessage(const SampleMessage& message) {
  // std::cout << message.DebugString() << std::endl;
  SampleMessage expectation;
  InitializeSampleMessage(&expectation);
  assert(MessageDifferencer::Equivalent(expectation, message));
}

static std::unique_ptr<Message> LoadMessageBasedOnDescriptor(
    DynamicMessageFactory* factory, const Descriptor* descriptor) {
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
  message->ParseFromString(serialized_message);

  return message;
}

static std::unique_ptr<Message> LoadMessageBasedOnMessage(
    const Message& instance) {
  // 1. Message::Newで、mutableなMessageを取得
  //    得たMessageは呼び出し元が解放しなければならないので、unique_ptrで受ける
  auto message = std::unique_ptr<Message>(instance.New());
  if (message == nullptr) {
    return nullptr;
  }

  // 2. Message::ParseFromXXXで、Messageをdeserialize
  message->ParseFromString(serialized_message);

  return message;
}

static void TestLoadMessageBasedOnMessage() {
  SetUp();

  clock_t start = clock();

  for (int i = 0; i < kTestCount; i++) {
    std::unique_ptr<Message> message =
        LoadMessageBasedOnMessage(SampleMessage::default_instance());

    // 4. 具体的なMessage型にキャスト
    SampleMessage* sample_message =
        DynamicCastToGenerated<SampleMessage>(message.get());
    CheckSampleMessage(*sample_message);
  }

  clock_t end = clock();

  std::cout << __func__ << " : " << (end - start) / (kTestCount / 1000) << " / "
            << (CLOCKS_PER_SEC * 1000) << "[sec]" << std::endl;

  TearDown();
}

static void TestLoadMessageBasedOnDescriptor() {
  SetUp();

  clock_t start = clock();

  for (int i = 0; i < kTestCount; i++) {
    SampleMessage sample_message;

    {
      // 0. DynamicMessageFactoryを生成
      DynamicMessageFactory factory;

      std::unique_ptr<Message> message =
          LoadMessageBasedOnDescriptor(&factory, SampleMessage::descriptor());

      // 4. 具体的なMessage型にコピー
      sample_message.CopyFrom(*message);

      // 5. ここでDynamicMessageFactory解放
      //    以降、factoryから生成したmessageは参照してはいけない
      //    sample_messageは引き続き使用できる
    }

    CheckSampleMessage(sample_message);
  }

  clock_t end = clock();

  std::cout << __func__ << " : " << (end - start) / (kTestCount / 1000) << " / "
            << (CLOCKS_PER_SEC * 1000) << "[sec]" << std::endl;

  TearDown();
}

int main() {
  TestLoadMessageBasedOnMessage();
  TestLoadMessageBasedOnDescriptor();

  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}