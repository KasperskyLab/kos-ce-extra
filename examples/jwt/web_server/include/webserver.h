// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#pragma once

#include <map>

#include <rtl/retcode.h>

#include "httpurihandler.h"

using UriHandlerMap = std::map<std::string, std::shared_ptr<HttpUriHandler>>;

class WebServer
{
public:
    WebServer(
        const std::string&              docRoot,
        unsigned short                  port,
        std::string                     certName,
        std::string                     keyName,
        std::string                     dhName,
        std::shared_ptr<HttpUriHandler> defaultHandler = nullptr);
    ~WebServer() = default;

    Retcode Run();
    // uri "" is default handler.
    Retcode SetHandler(const std::string& uri, std::shared_ptr<HttpUriHandler> handler);
    Retcode RemoveHandler(const std::string& uri);

    http::message_generator HandleRequest(const Request&& req);

private:
    std::shared_ptr<std::string>    m_docRoot;
    unsigned short                  m_port;
    UriHandlerMap                   m_uriHandlers;
    std::shared_ptr<HttpUriHandler> m_defaultHandler;
    std::string                     m_certName;
    std::string                     m_keyName;
    std::string                     m_dhName;
};
