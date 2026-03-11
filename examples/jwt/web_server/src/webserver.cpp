// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include <boost/beast/version.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "log.h"
#include "webserver.h"

using namespace std::chrono_literals;

namespace {

void Fail(beast::error_code ec, char const* what)
{
    LOG(INFO, "{}: {}", what, ec.message());
}

class Session: public std::enable_shared_from_this<Session>
{
    WebServer*                       m_server;
    ssl::stream<beast::tcp_stream>   m_stream;
    beast::flat_buffer               m_buffer;
    http::request<http::string_body> m_req;

public:
    explicit Session(WebServer* server, tcp::socket&& socket, ssl::context& sslCtx)
        : m_server{server}, m_stream(std::move(socket), sslCtx)
    {}

    void Run()
    {
        net::dispatch(
            m_stream.get_executor(),
            beast::bind_front_handler(&Session::OnRun, shared_from_this()));
    }

    void OnRun()
    {
        m_stream.async_handshake(
            ssl::stream_base::server,
            beast::bind_front_handler(&Session::OnHandshake, shared_from_this()));
    }

    void OnHandshake(beast::error_code ec)
    {
        if (ec)
        {
            Fail(ec, "handshake");
        }
        else
        {
            Read();
        }
    }

    void Read()
    {
        m_req = {};

        http::async_read(
            m_stream,
            m_buffer,
            m_req,
            beast::bind_front_handler(&Session::OnRead, shared_from_this()));
    }

    void OnRead(beast::error_code ec, [[maybe_unused]] std::size_t bytesTransferred)
    {
        if (ec)
        {
            if (ec == http::error::end_of_stream)
            {
                Close();
            }
            else
            {
                Fail(ec, "read");
            }
        }
        else
        {
            SendResponse(m_server->HandleRequest(std::move(m_req)));
        }
    }

    void SendResponse(http::message_generator&& msg)
    {
        bool keepAlive = msg.keep_alive();

        beast::async_write(
            m_stream,
            std::move(msg),
            beast::bind_front_handler(&Session::OnWrite, shared_from_this(), keepAlive));
    }

    void OnWrite(
        bool keepAlive, beast::error_code ec, [[maybe_unused]] std::size_t bytesTransferred)
    {
        if (ec)
        {
            Fail(ec, "write");
        }
        else
        {
            if (!keepAlive)
            {
                Close();
            }

            Read();
        }
    }

    void Close()
    {
        beast::get_lowest_layer(m_stream).expires_after(std::chrono::seconds(30));

        m_stream.async_shutdown(
            beast::bind_front_handler(&Session::OnShutdown, shared_from_this()));
    }

    void OnShutdown(beast::error_code ec)
    {
        if (ec)
        {
            Fail(ec, "shutdown");
        }
    }
};

class Listener: public std::enable_shared_from_this<Listener>
{
public:
    Listener(WebServer* server, net::io_context& ioc, ssl::context& sslCtx, tcp::endpoint endpoint)
        : m_server{server}, m_ioContext{ioc}, m_sslContext{sslCtx}, m_acceptor{ioc, endpoint}
    {
        m_acceptor.listen(net::socket_base::max_listen_connections);
    }

    void Run()
    {
        Accept();
    }

private:
    void Accept()
    {
        m_acceptor.async_accept(
            net::make_strand(m_ioContext),
            beast::bind_front_handler(&Listener::OnAccept, shared_from_this()));
    }

    void OnAccept(beast::error_code ec, tcp::socket socket)
    {
        if (ec)
        {
            Fail(ec, "accept");
        }
        else
        {
            std::make_shared<Session>(m_server, std::move(socket), m_sslContext)->Run();
        }

        Accept();
    }

    WebServer*       m_server;
    net::io_context& m_ioContext;
    ssl::context&    m_sslContext;
    tcp::acceptor    m_acceptor;
};

} // namespace

WebServer::WebServer(
    const std::string&              docRoot,
    unsigned short                  port,
    std::string                     certName,
    std::string                     keyName,
    std::string                     dhName,
    std::shared_ptr<HttpUriHandler> defaultHandler)
    : m_docRoot{std::make_shared<std::string>(docRoot)}
    , m_port{port}
    , m_uriHandlers{}
    , m_defaultHandler{defaultHandler}
    , m_certName{certName}
    , m_keyName{keyName}
    , m_dhName{dhName}
{}

Retcode WebServer::Run()
{
    auto const Address = net::ip::make_address("0.0.0.0");
    const int  Threads = 4;

    net::io_context ioc{Threads};

    try
    {
        ssl::context sslCtx{ssl::context::tlsv12};
        // clang-format off
        sslCtx.set_options(
            boost::asio::ssl::context::default_workarounds |
            boost::asio::ssl::context::no_sslv2 |
            boost::asio::ssl::context::single_dh_use
        );
        // clang-format on
        sslCtx.use_certificate_chain_file(m_certName);
        sslCtx.use_private_key_file(m_keyName, boost::asio::ssl::context::file_format::pem);
        sslCtx.use_tmp_dh_file(m_dhName);

        std::make_shared<Listener>(this, ioc, sslCtx, tcp::endpoint{Address, m_port})->Run();
        std::vector<std::thread> v;
        v.reserve(Threads - 1);
        for (auto i = Threads - 1; i > 0; --i)
            v.emplace_back([&ioc] { ioc.run(); });
        LOG(INFO, "WebServer started (port: {})", m_port);
        ioc.run();
    }
    catch (std::exception& err)
    {
        LOG(ERROR, "Error: {}", err.what());
        return rcFail;
    }

    return rcOk;
}

Retcode WebServer::SetHandler(const std::string& uri, std::shared_ptr<HttpUriHandler> handler)
{
    if (uri.compare(""))
    {
        m_uriHandlers[uri] = handler;
    }
    else
    {
        m_defaultHandler = handler;
    }

    return rcOk;
}

Retcode WebServer::RemoveHandler(const std::string& uri)
{
    if (uri.compare(""))
    {
        m_uriHandlers.erase(uri);
    }
    else
    {
        m_defaultHandler.reset();
    }

    return rcOk;
}

http::message_generator WebServer::HandleRequest(const Request&& req)
{
    // Request path must be absolute and not contain "..".
    if (req.target().empty() || req.target()[0] != '/'
        || req.target().find("..") != beast::string_view::npos)
    {
        return MakeErrorResponse(req, http::status::bad_request, "Illegal request-target");
    }

    std::string uri = req.target().substr(0, req.target().find('?'));

    auto handler = m_uriHandlers.find(uri);
    if (handler != m_uriHandlers.end())
    {
        std::optional<Response> res{handler->second->HandleRequest(req)};

        if (res)
        {
            return std::move(res).value();
        }
    }
    else if (m_defaultHandler)
    {
        std::optional<Response> res{m_defaultHandler->HandleRequest(req)};

        if (res)
        {
            return std::move(res).value();
        }
    }

    return MakeErrorResponse(req, http::status::method_not_allowed, "Unknown HTTP-method");
}
