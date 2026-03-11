// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include "webserver.h"

#include "allrequesthandler.h"
#include "gettokenhandler.h"
#include "getdatahandler.h"
#include "log.h"

#include <kos_net.h>

#include <jwt_example/WebServer.edl.cpp.h>
#include <jwt_example/IToken.idl.cpp.h>

#include <kosipc/connect_static_channel.h>
#include <kosipc/make_application.h>

#include <boost/program_options.hpp>

#include <connections.h>

namespace {

constexpr auto DocumentRoot = "/www";
constexpr auto Port         = 1106;

} // namespace

int main(int argc, const char* argv[])
{
    logrr::Init(AppTag);

    std::string certName;
    std::string keyName;
    std::string dhName;

    try
    {
        namespace po = boost::program_options;
        po::options_description desc("Allowed options");
        // clang-format off
        desc.add_options()
            ("help",                           "This help")
            ("cert", po::value<std::string>(), "SSL certificate")
            ("key",  po::value<std::string>(), "SSL certificate key")
            ("dh",   po::value<std::string>(), "SSL DH parameters")
        ;
        // clang-format on

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cerr << desc << std::endl;
            return EXIT_SUCCESS;
        }

        if (!vm.count("cert"))
        {
            LOG(ERROR, "No --cert argument!");
            return EXIT_FAILURE;
        }

        certName = vm["cert"].as<std::string>();

        if (!vm.count("key"))
        {
            LOG(ERROR, "No --key argument!");
            return EXIT_FAILURE;
        }

        keyName = vm["key"].as<std::string>();

        if (!vm.count("dh"))
        {
            LOG(ERROR, "No --dh argument!");
            return EXIT_FAILURE;
        }

        dhName = vm["dh"].as<std::string>();
    }
    catch (std::exception& err)
    {
        LOG(ERROR, "Error: {}", err.what());
        return EXIT_FAILURE;
    }

    if (!wait_for_iface(nullptr, IWF_IP4, DEFAULT_TIMEOUT))
    {
        LOG(ERROR, "Error: Wait for network failed!");
        return EXIT_FAILURE;
    }

    list_network_ifaces();

    kosipc::Application app   = kosipc::MakeApplicationAutodetect();
    auto                proxy = app.MakeProxy<kosipc::stdcpp::jwt_example::IToken>(
        kosipc::ConnectStaticChannel(connections::ConnectionName, "token"));

    WebServer server{DocumentRoot, Port, certName, keyName, dhName};

    server.SetHandler("", std::make_shared<AllRequestHandler>(DocumentRoot));
    server.SetHandler("/gettoken", std::make_shared<GetTokenHandler>(DocumentRoot, proxy.get()));
    server.SetHandler("/getdata", std::make_shared<GetDataHandler>(DocumentRoot, proxy.get()));

    server.Run();

    return EXIT_SUCCESS;
}
