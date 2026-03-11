// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include "connection_manager.h"

#include <memory>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <sstream>

namespace wpa_supplicant {

constexpr auto maxBufferSize  = 4096;
constexpr auto defaultTimeout = 10;
constexpr auto scanTime = 10;

ConnectionManager::ConnectionManager(std::string ssid, std::string psk)
    : m_ssid(std::move(ssid)), m_psk(std::move(psk))
{
    m_wpaClient = std::shared_ptr<WpaClient>(InitWpa(), FreeWpa);
}

std::string ConnectionManager::RunCommand(const std::string &cmd)
{
    std::array<char, maxBufferSize> buffer;
    std::string                     result;

    if (ExecuteCommand(m_wpaClient.get(), cmd.c_str(), buffer.data(), buffer.size()) == rcOk)
    {
        result = std::string(buffer.data());
    }

    return result;
}

bool ConnectionManager::StartScan()
{
    std::string cmd{"SCAN"};

    auto result = RunCommand(cmd);

    return result == "OK";
}

std::string ConnectionManager::GetScanResults()
{
    std::string cmd{"SCAN_RESULTS"};

    auto result = RunCommand(cmd);

    return result;
}

std::string ConnectionManager::GetStatus()
{
    std::string cmd{"STATUS"};

    auto result = RunCommand(cmd);

    return result;
}

std::string ConnectionManager::AddNetwork()
{
    std::string cmd{"ADD_NETWORK"};

    auto networkId = RunCommand(cmd);

    return networkId;
}

bool ConnectionManager::SetNetwork(
    const std::string &networkId, const std::string &property, const std::string &value)
{
    std::string cmd = "SET_NETWORK " + networkId + " " + property + " " + value;

    auto result = RunCommand(cmd);

    return result == "OK";
}

bool ConnectionManager::SelectNetwork(const std::string &networkId)
{
    std::string cmd = "SELECT_NETWORK " + networkId;

    auto result = RunCommand(cmd);

    return result == "OK";
}

bool ConnectionManager::DisableNetwork(const std::string &networkId)
{
    std::string cmd = "DISABLE_NETWORK " + networkId;

    auto result = RunCommand(cmd);

    return result == "OK";
}

bool ConnectionManager::GetNetworks(std::string &result)
{
    if (!StartScan())
    {
        result = "Failed to get list of available networks";
        return false;
    }

    std::this_thread::sleep_for(std::chrono::seconds(scanTime));
    result = GetScanResults();

    return true;
}

bool ConnectionManager::WaitStatus(
    const std::string &propertyName, int timeout, StatusChecker checker)
{
    while (timeout > 0)
    {
        // Wait for the connection to complete.
        std::this_thread::sleep_for(std::chrono::seconds(1));
        timeout--;

        auto status = GetProperty(GetStatus(), propertyName);

        if (checker(status))
        {
            return true;
        }
    }

    return false;
}

bool ConnectionManager::Connect()
{
    auto timeout = defaultTimeout;
    m_networkId  = AddNetwork();

    if (!SetNetwork(m_networkId, "ssid", m_ssid))
    {
        return false;
    }

    if (!SetNetwork(m_networkId, "psk", m_psk))
    {
        return false;
    }

    if (!SelectNetwork(m_networkId))
    {
        return false;
    }

    return WaitStatus(
        "wpa_state", timeout, [](const std::string &status) { return status == "COMPLETED"; });
}

bool ConnectionManager::Disconnect()
{
    auto timeout = defaultTimeout;
    if (!DisableNetwork(m_networkId))
    {
        return false;
    }

    return WaitStatus(
        "wpa_state", timeout, [](const std::string &status) { return status != "COMPLETED"; });
}

std::string ConnectionManager::GetConnectionStatus()
{
    return GetProperty(GetStatus(), "wpa_state");
}

std::string ConnectionManager::GetCurrentSSID()
{
    return GetProperty(GetStatus(), "ssid");
}

std::string ConnectionManager::GetProperty(const std::string &blob, const std::string &property)
{
    std::stringstream ss(blob);
    std::string       to;

    while (std::getline(ss, to, '\n'))
    {
        auto pos = to.find("=");

        if (pos != std::string::npos)
        {
            auto token = to.substr(0, pos);
            if (token == property)
            {
                return to.substr(pos + 1);
            }
        }
    }

    return std::string();
}

} // namespace wpa_supplicant
