// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#pragma once

#include <iostream>

#include <component/logrr/cpp/logger.h>

#include "httpurihandler.h"

namespace log {

// Log output of the WEB-server handler parameters
void Log(const Request& req);

} // namespace log
