// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include <boost/system/detail/error_code.hpp>
#include <kos_net.h>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <memory>
#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <thread>

#include "connection_manager.h"

namespace po = boost::program_options;
namespace ba = boost::asio;

void GetConfigurationOptions(po::options_description& desc, po::variables_map& vm)
{
    std::ifstream configFile("/romfs/client.conf");

    vm = po::variables_map();

    po::store(po::parse_config_file(configFile, desc), vm);
    po::notify(vm);
}

class BoostTcpSocket
{
public:
    BoostTcpSocket(const std::string& ip, unsigned short port)
        : m_ep(ba::ip::address::from_string(ip), port), m_ios(), m_sock(m_ios)
    {
        m_sock.connect(m_ep, m_ec);
        LogOperationResult("TCP connection");
    }

    ~BoostTcpSocket()
    {
        if (m_sock.is_open())
        {
            m_sock.close(m_ec);
            LogOperationResult("Close socket");
        }
    }

    void Send(const std::string& message)
    {
        if (m_ec)
            return;
        m_sock.send(ba::buffer(message), 0, m_ec);
        std::stringstream ss;
        ss << "Send message \"" << message << '\"';
        auto messageStr = ss.str();
        messageStr.erase(
            std::remove(messageStr.begin(), messageStr.end(), '\n'), messageStr.cend());
        LogOperationResult(messageStr);
    }

private:
    ba::ip::tcp::endpoint     m_ep;
    ba::io_service            m_ios;
    ba::ip::tcp::socket       m_sock;
    boost::system::error_code m_ec;

    void LogOperationResult(const std::string& message)
    {
        if (m_ec)
        {
            std::cout << message << " failed! Error: " << m_ec.message() << std::endl;
        }
        else
        {
            std::cout << message << " success." << std::endl;
        }
    }
};

int main(int, char**)
{
    std::string    ssid;
    std::string    psk;
    std::string    serverIp;
    std::string    networkList;
    bool           getNetworkResult;
    unsigned short serverPort;

    std::cout << "Start Wpa Client" << std::endl;

    po::options_description desc("Options");
    desc.add_options()("ssid", po::value<std::string>(&ssid), "ssid");
    desc.add_options()("psk", po::value<std::string>(&psk), "psk");
    desc.add_options()("serverIp", po::value<std::string>(&serverIp), "serverIp");
    desc.add_options()("serverPort", po::value<unsigned short>(&serverPort), "serverPort");

    po::variables_map vm;

    GetConfigurationOptions(desc, vm);

    std::cout << "ssid: " << ssid << std::endl
              << "psk: " << psk << std::endl
              << "server ip:port: " << serverIp << ":" << serverPort << std::endl;

    auto connectionManager = wpa_supplicant::ConnectionManager(ssid, psk);
    std::cout << "Connection status: " << connectionManager.GetConnectionStatus() << std::endl;

    std::cout << "*************************************" << std::endl;

    getNetworkResult = connectionManager.GetNetworks(networkList);

    std::cout << networkList << std::endl << "*************************************" << std::endl;

    if (!getNetworkResult)
        return EXIT_FAILURE;

    if (!connectionManager.Connect())
    {
        std::cout << "Connection error" << std::endl;
        return EXIT_FAILURE;
    }

    ssid = connectionManager.GetCurrentSSID();

    std::cout << "WiFi connection success to ssid '" << ssid << "'" << std::endl
              << "Connection status: " << connectionManager.GetConnectionStatus() << std::endl;

    if (!serverIp.empty())
    {
        // Wait for DHCP to get the IP address.
        std::this_thread::sleep_for(std::chrono::seconds(10));

        BoostTcpSocket boostSocket(serverIp, serverPort);

        std::string message = "Hello from KasperskyOS\n";

        boostSocket.Send(message);
    }
    else
    {
        std::cout << "Server IP missing from config, not connecting." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (!connectionManager.Disconnect())
    {
        std::cout << "WiFi disconnection error" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "WiFi disconnection success from ssid '" << ssid << "'" << std::endl
              << "Connection status: " << connectionManager.GetConnectionStatus() << std::endl;

    return EXIT_SUCCESS;
}
