// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include <cstring>
#include <exception>
#include <iostream>
#include <memory>

#include <kos_net.h>
#include <component/logrr/cpp/logger.h>

#include "general.h"
#include "publisher.h"

namespace {
constexpr auto DefaultPort = 5672;
} // namespace

int main()
{
    logrr::Init(app::AppTag);

    if (!wait_for_iface(nullptr, IWF_IP4 | IWF_GW4, DEFAULT_TIMEOUT))
    {
        LOG(ERROR, "Wait for network failed!");
        return EXIT_FAILURE;
    }

    char *brokerAddress = getenv("AMQP_BROKER_ADDRESS");
    if (!brokerAddress)
    {
        LOG(ERROR, "Unable to get broker address from env AMQP_BROKER_ADDRESS.");
        return EXIT_FAILURE;
    }

    const int brokerPort = [] {
        try
        {
            return std::stoi(getenv("AMQP_BROKER_PORT"));
        }
        catch (const std::invalid_argument &ex)
        {
            LOG(ERROR, "Failed to get AMQP broker port. Using default {} port.", DefaultPort);
            return DefaultPort;
        }
    }();

    LOG(INFO, "AMQP BROKER {}:{}", brokerAddress, brokerPort);

    try
    {
        auto pub = std::make_unique<Publisher>(brokerAddress, brokerPort);
        LOG(INFO, "Sending data ...");
        pub->SendData();
    }
    catch (const std::exception &e)
    {
        LOG(ERROR, "{}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
