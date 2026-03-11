// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include "gettokenhandler.h"
#include "log.h"

#include <boost/beast/version.hpp>

// clang-format off
GetTokenHandler::GetTokenHandler(
    beast::string_view docRoot,
    kosipc::stdcpp::jwt_example::IToken* proxy)
    : HttpUriHandler{docRoot}, m_proxy(proxy)
{
    SetMethodHandler(http::verb::get, [this](const Request& req) { return HandleGet(req); });
}
// clang-format on

std::optional<Response> GetTokenHandler::HandleGet(const Request& req)
{
    log::Log(req);

    if (req.method() != http::verb::get)
    {
        return std::nullopt;
    }

    StringResponse res{http::status::ok, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/plain");
    res.keep_alive(req.keep_alive());

    kosipc::stdcpp::jwt_example::Token token;

    m_proxy->Generate(token);

    res.body() = token;
    res.prepare_payload();

    return res;
}
