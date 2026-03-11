// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include "allrequesthandler.h"
#include "log.h"

#include <boost/beast/version.hpp>

namespace {

beast::string_view GetMimeType(beast::string_view path)
{
    using beast::iequals;
    auto const ext = [&path] {
        auto const pos = path.rfind(".");
        if (pos == beast::string_view::npos)
        {
            return beast::string_view{};
        }
        return path.substr(pos);
    }();

    if (iequals(ext, ".htm"))
        return "text/html";
    if (iequals(ext, ".html"))
        return "text/html";
    if (iequals(ext, ".php"))
        return "text/html";
    if (iequals(ext, ".css"))
        return "text/css";
    if (iequals(ext, ".txt"))
        return "text/plain";
    if (iequals(ext, ".png"))
        return "image/png";
    if (iequals(ext, ".jpe"))
        return "image/jpeg";
    if (iequals(ext, ".jpeg"))
        return "image/jpeg";
    if (iequals(ext, ".jpg"))
        return "image/jpeg";
    if (iequals(ext, ".ico"))
        return "image/vnd.microsoft.icon";
    if (iequals(ext, ".svg"))
        return "image/svg+xml";
    if (iequals(ext, ".svgz"))
        return "image/svg+xml";
    return "application/text";
}

std::string ConcatPath(beast::string_view base, beast::string_view path)
{
    if (base.empty())
    {
        return std::string(path);
    }

    constexpr char PathSeparator = '/';
    std::string    result{base};

    if (result.back() == PathSeparator)
    {
        result.resize(result.size() - 1);
    }
    result.append(path.data(), path.size());

    return result;
}

} // namespace

AllRequestHandler::AllRequestHandler(beast::string_view docRoot): HttpUriHandler{docRoot}
{
    SetMethodHandler(http::verb::get, [this](const Request& req) { return HandleGet(req); });
}

std::optional<Response> AllRequestHandler::HandleGet(const Request& req)
{
    log::Log(req);

    if (req.method() != http::verb::get)
    {
        return std::nullopt;
    }

    const std::string uri = req.target();

    std::string path = ConcatPath(GetDocRoot(), uri);
    if (path.back() == '/')
    {
        path.append("index.html");
    }

    beast::error_code           ec;
    http::file_body::value_type body;
    body.open(path.c_str(), beast::file_mode::scan, ec);

    if (ec == beast::errc::no_such_file_or_directory)
    {
        return MakeErrorResponse(
            req,
            http::status::not_found,
            "The resource '" + std::string(req.target()) + "' was not found.");
    }
    else if (ec)
    {
        return MakeErrorResponse(req, http::status::internal_server_error, ec.message());
    }

    auto const Size = body.size();

    http::response<http::file_body> res{
        std::piecewise_construct,
        std::make_tuple(std::move(body)),
        std::make_tuple(http::status::ok, req.version())};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, GetMimeType(path));
    res.content_length(Size);
    res.keep_alive(req.keep_alive());

    return res;
}
