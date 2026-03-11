// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#pragma once

#include <map>
#include <optional>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ssl.hpp>

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
namespace ssl   = boost::asio::ssl;


using tcp            = boost::asio::ip::tcp;
using Request        = http::request<http::string_body>;
using Response       = http::message_generator;
using FileResponse   = http::response<http::file_body>;
using StringResponse = http::response<http::string_body>;

class HttpUriHandler;

using HandleRequestCallback = std::function<std::optional<Response>(const Request& req)>;
using HttpMethodHandlerMap  = std::map<boost::beast::http::verb, HandleRequestCallback>;

Response MakeErrorResponse(const Request& req, http::status errCode, beast::string_view why);

class HttpUriHandler
{
public:
    HttpUriHandler(beast::string_view docRoot);
    virtual ~HttpUriHandler() = default;

    std::optional<Response> HandleRequest(const Request& req);

protected:
    void               SetMethodHandler(boost::beast::http::verb, HandleRequestCallback);
    beast::string_view GetDocRoot();

private:
    beast::string_view   m_docRoot;
    HttpMethodHandlerMap m_httpMethods;
};
