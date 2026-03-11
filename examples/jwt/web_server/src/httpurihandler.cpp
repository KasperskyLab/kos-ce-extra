// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include "httpurihandler.h"

#include <boost/beast/version.hpp>

Response MakeErrorResponse(const Request& req, http::status errCode, beast::string_view why)
{
    StringResponse res{errCode, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = std::string(why);
    res.prepare_payload();
    return res;
};

HttpUriHandler::HttpUriHandler(beast::string_view docRoot): m_docRoot{docRoot} {}

std::optional<Response> HttpUriHandler::HandleRequest(const Request& req)
{
    auto methodHandler = m_httpMethods.find(req.method());
    if (methodHandler != m_httpMethods.end())
    {
        return methodHandler->second(req);
    }

    return std::nullopt;
}

void HttpUriHandler::SetMethodHandler(
    boost::beast::http::verb method, HandleRequestCallback handler)
{
    m_httpMethods[method] = handler;
}

beast::string_view HttpUriHandler::GetDocRoot()
{
    return m_docRoot;
}
