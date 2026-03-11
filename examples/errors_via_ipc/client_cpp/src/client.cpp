// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include <chrono>
#include <iostream>
#include <optional>
#include <random>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>

#include <kosipc/connect_static_channel.h>

#include <component/logrr/cpp/logger.h>

// Generated headers.
#include <errors_via_ipc_example/Client.edl.cpp.h>
#include <errors_via_ipc_example/NumberSummation.idl.cpp.h>

#include "common.h"
#include "exception_converter.h"

constexpr auto Tag = "CXX Client";
using namespace std::chrono_literals;
using namespace std::literals;
using namespace kosipc::stdcpp;
using namespace errors_via_ipc_example;

namespace {

constexpr auto permDelay = 1s;
constexpr auto busyDelay = 100ms;

using Name     = std::string_view;
using Endpoint = kosipc::unique_ptr<NumberSummation>;
using Server   = std::tuple<Name, Endpoint>;
using Servers  = std::array<Server, 2>;

} // namespace

std::optional<uint64_t> ProcessRequest(Servers& servers, uint32_t firstServer, Request req)
{
    for (uint32_t curServer{firstServer};; curServer ^= 1, ++req.repeats)
    {
        auto& [name, endpoint] = servers[curServer];
        try
        {
            uint64_t result{};
            endpoint->SumRequest(req, result);
            return result;
        }
        catch (const err::exception::ServerBusy& e)
        {
            LOG(INFO, "'{}' is busy, Changing server...", name);
            std::this_thread::sleep_for(busyDelay);
            continue;
        }
        catch (const std::runtime_error& e)
        {
            if (req.repeats > MaxSecurityGrantedRepeats)
            {
                LOG(ERROR, "Transport error, number of repeats exceeded {}!", req.repeats - 1);
            }
            else
            {
                LOG(ERROR, "{}", Tag, e.what());
            }
            return std::nullopt;
        }
    }
}


// Client entry point.
int main()
try
{
    logrr::Init(Tag);
    LOG(INFO, "Started");

    // Create Application object.
    kosipc::ApplicationSettings as;
    as.SetRetcodeExceptionConverter(std::make_shared<err::RetcodeExceptionConverter>());
    kosipc::Application app = kosipc::MakeApplicationAutodetect(as);

    Servers servers{
        {{"C Server"sv,
          app.MakeProxy<NumberSummation>(
              kosipc::ConnectStaticChannel(ServerChannelName[0], "log"))},
         {"CXX Server"sv,
          app.MakeProxy<NumberSummation>(
              kosipc::ConnectStaticChannel(ServerChannelName[1], "log"))}}
    };

    std::mt19937                            generator(std::random_device{}());
    std::uniform_int_distribution<uint32_t> serversDistribute{0, 1};
    std::uniform_int_distribution<uint32_t> numbersDistribute{1, 100};

    while (true)
    {
        Request req{0, numbersDistribute(generator), numbersDistribute(generator)};
        auto    result = ProcessRequest(servers, serversDistribute(generator), std::move(req));
        if (result)
        {
            LOG(INFO, "Result = {}", *result);
        }
        std::this_thread::sleep_for(permDelay);
    }

    return EXIT_SUCCESS;
}
catch (...)
{
    LOG(ERROR, "Got an unhandled exception! Exiting.");
    return EXIT_FAILURE;
}
