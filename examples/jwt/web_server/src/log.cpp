// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include "log.h"

void log::Log(const Request& req)
{
    LOG(INFO, "Local URL:  {:.{}s}", req.target().data(), static_cast<int>(req.target().size()));
    LOG(INFO,
        "Request:    {:.{}s}",
        req.method_string().data(),
        static_cast<int>(req.method_string().size()));
}
