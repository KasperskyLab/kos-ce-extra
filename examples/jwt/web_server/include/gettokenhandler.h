// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#pragma once

#include "httpurihandler.h"

#include <jwt_example/IToken.idl.cpp.h>

class GetTokenHandler: public HttpUriHandler
{
public:
    GetTokenHandler(beast::string_view docRoot, kosipc::stdcpp::jwt_example::IToken *proxy);

private:
    std::optional<Response>              HandleGet(const Request &req);
    kosipc::stdcpp::jwt_example::IToken *m_proxy;
};
