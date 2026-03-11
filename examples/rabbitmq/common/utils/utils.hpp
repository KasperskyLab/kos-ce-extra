// © 2026 AO Kaspersky Lab
// Licensed under the MIT License

#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include <rabbitmq-c/amqp.h>

namespace utils {
void     ThrowOnError(int x, const std::string& context);
void     ThrowOnAmqpError(amqp_rpc_reply_t x, const std::string& context);
void     AmqpDump(const void* buffer, size_t len);
void     MicroSleep(int usec);
uint64_t MicroSecNow(void);
} // namespace utils

#endif // _UTILS_H
