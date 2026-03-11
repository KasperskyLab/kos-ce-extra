//
// © 2025 AO Kaspersky Lab
// Licensed under the MIT License
//

#ifndef COMMON_IPC_H
#define COMMON_IPC_H

#include <protobuf_example/IConsumer.idl.cpp.h>

namespace consumer {

namespace ipc =  kosipc::stdcpp::protobuf_example;

constexpr auto ServiceId = "message_consumer";
constexpr auto Endpoint  = "consumer";

} // namespace consumer
#endif // COMMON_IPC_H
