// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include "getdatahandler.h"
#include "log.h"

#include <boost/beast/version.hpp>
#include <boost/url/parse.hpp>
#include <boost/url.hpp>

#include <string_view>

namespace {

std::string GetParamValue(const std::string uri, const std::string name)
{
    std::string       key, val;
    std::stringstream ss{uri};
    std::getline(ss, key, '?');
    while (std::getline(ss, key, '=') && std::getline(ss, val, '&'))
    {
        if (key == name)
        {
            return val;
        }
    }

    return "";
}

} // namespace

// clang-format off
GetDataHandler::GetDataHandler(
    beast::string_view docRoot,
    kosipc::stdcpp::jwt_example::IToken* proxy)
    : HttpUriHandler{docRoot}, m_proxy(proxy)
{
    SetMethodHandler(http::verb::get, [this](const Request& req) { return HandleGet(req); });
}
// clang-format on

std::optional<Response> GetDataHandler::HandleGet(const Request& req)
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

    kosipc::stdcpp::jwt_example::Token token  = GetParamValue(req.target(), "jwt");
    kosipc::stdcpp::kl::core::Boolean  result = 0;

    m_proxy->Verify(token, result);

    std::string body;

    if (result)
    {
        body = "<div class=\"success\">Data access succeeded</div>";
    }
    else
    {
        body = "<div class=\"error\">Wrong token or token service unavailable</div>";
    }

    res.body() = body;
    res.prepare_payload();

    return res;
}
