// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H
#pragma once

#include <wpa_ipc_client.h>

#include <string>
#include <memory>

namespace wpa_supplicant {

typedef bool StatusChecker(const std::string &);

class ConnectionManager
{
public:
    ConnectionManager(std::string ssid, std::string psk);
    virtual ~ConnectionManager(){};

    bool GetNetworks(std::string &result);

    bool        Connect();
    bool        Disconnect();
    std::string GetConnectionStatus();
    std::string GetCurrentSSID();

protected:
    bool        StartScan();
    std::string GetScanResults();
    std::string GetStatus();
    std::string AddNetwork();
    bool        SetNetwork(
               const std::string &networkId, const std::string &property, const std::string &value);
    bool SelectNetwork(const std::string &networkId);
    bool DisableNetwork(const std::string &networkId);

private:
    std::string                m_ssid;
    std::string                m_psk;
    std::shared_ptr<WpaClient> m_wpaClient;

    std::string m_networkId;

    std::string RunCommand(const std::string &cmd);
    std::string GetProperty(const std::string &blob, const std::string &propery);

    bool WaitStatus(const std::string &propertyName, int timeout, StatusChecker checker);
};

} // namespace wpa_supplicant

#endif // CONNECTION_MANAGER_H
