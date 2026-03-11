// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include <iostream>
#include <utility>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>

#include <kosipc/application.h>
#include <kosipc/make_application.h>
#include <kosipc/connect_static_channel.h>
#include <component/logrr/cpp/logger.h>

#include <common/ipc.h>
#include <common/callable.h>

#include <example/Client.edl.cpp.h>

LOGRR_DECLARE_SUBPROVIDER(CLIENT);
LOGRR_DEFINE_SUBPROVIDER(CLIENT);

using namespace kosipc;
using namespace resolver;
using namespace util;

class OnResolveCallback: public ipc::ResolveCallback
{
public:
    void OnResolve(const std::string& name, const std::string& address) override
    {
        LOG(INFO, "Host {} has address: {}", name, address);
    }
};

int main(int argc, const char* argv[])
{
    LOG(INFO, "Started");

    if (argc == 1)
    {
        LOG(WARNING, "No host names passed - nothing to do");
        return EXIT_SUCCESS;
    }

    auto              app{MakeApplicationAutodetect()};
    OnResolveCallback onResolve{};
    components::Root  root;
    root.onResolve = &onResolve;

    try
    {
        Callable callback{app, root.onResolve};
        auto     resolver{app.MakeProxy<ipc::Resolver>(
            ConnectStaticChannel(resolver::SeviceId, resolver::Endpoint))};
        for (auto name = argv + 1; *name != nullptr; ++name)
        {
            resolver->Resolve(*name, callback.Get());
        }
    }
    catch (const std::exception& exc)
    {
        LOG(ERROR, "Error occurred while name resolving: {}", exc.what());
        return EXIT_FAILURE;
    }

    LOG(INFO, "Finished");
    return EXIT_SUCCESS;
}
