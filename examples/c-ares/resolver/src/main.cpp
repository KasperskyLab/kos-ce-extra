// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include <kos_net.h>
#include <kosipc/application.h>
#include <kosipc/event_loop.h>
#include <kosipc/make_application.h>
#include <kosipc/serve_static_channel.h>
#include <component/logrr/cpp/logger.h>

#include <example/NameResolver.edl.cpp.h>

#include "resolver.h"

LOGRR_DECLARE_SUBPROVIDER(RESOLVER);
LOGRR_DEFINE_SUBPROVIDER(RESOLVER);

using namespace kosipc;
using namespace resolver;

int main(void)
{
    LOG(INFO, "Started");

    if (!wait_for_iface(NULL, IWF_IP4 | IWF_GW4, DEFAULT_TIMEOUT))
    {
        LOG(ERROR, "Network up failed");
        return EXIT_FAILURE;
    }

    try
    {
        auto             app{MakeApplicationAutodetect()};
        Resolver         resolver{app};
        components::Root root;

        root.resolver = &resolver;
        auto loop     = app.MakeEventLoop(ServeStaticChannel(resolver::SeviceId, root));
        loop.Run();
    }
    catch (const std::exception& exc)
    {
        LOG(ERROR, "Error occurred: {}", exc.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
