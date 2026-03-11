// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#pragma once

#include "httpurihandler.h"

class AllRequestHandler: public HttpUriHandler
{
public:
    AllRequestHandler(beast::string_view docRoot);

private:
    std::optional<Response> HandleGet(const Request& req);
    std::optional<Response> MakeIndexPageResponse(const Request& req, std::string uri);
};
