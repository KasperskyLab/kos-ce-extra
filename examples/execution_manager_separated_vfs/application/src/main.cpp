// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include <cstdlib>
#include <iostream>

#include <unistd.h>

#include <kl/core/Types.idl.h>

#include <coresrv/nk/transport-kos.h>
#include <coresrv/ns/ns_api.h>
#include <coresrv/cm/cm_api.h>
#include <coresrv/task/task_api.h>

#include <client/Notification.idl.h>
#include <application/Application.edl.h>

#define TASK_NAME_LEN 100

const char Tag[] = "[Application]";

static Retcode SendNotificationToClient()
{
    NsHandle ns;
    char     server[kl_core_Types_UCoreStringSize];
    char     service[kl_core_Types_UCoreStringSize];


    Retcode rc = NsCreate(NULL, INFINITE_TIMEOUT, &ns);
    if (rc != rcOk)
    {
        return rc;
    }

    rc = NsEnumServices(
        ns,
        "client.Notification",
        0,
        server,
        kl_core_Types_UCoreStringSize,
        service,
        kl_core_Types_UCoreStringSize);
    if (rc != rcOk)
    {
        return rc;
    }

    Handle   handle;
    uint32_t rsid;

    rc = KnCmConnect(server, service, INFINITE_TIMEOUT, &handle, &rsid);
    if (rc != rcOk)
    {
        return rc;
    }

    NkKosTransport transport;
    NkKosTransport_Init(&transport, handle, NK_NULL, 0);

    struct client_Notification_proxy proxy;

    client_Notification_proxy_init(&proxy, &transport.base, (nk_iid_t)rsid);

    struct client_Notification_Notification_req req;
    struct client_Notification_Notification_res res;

    return client_Notification_Notification(&proxy.base, &req, NULL, &res, NULL);
}

int main(int argc, const char *argv[], const char *envs[])
{
    std::string idx = "";
    char        buf[TASK_NAME_LEN];
    Retcode     rc = KnTaskGetName(buf, TASK_NAME_LEN);

    if (argc > 1)
    {
        idx = argv[1];
    }
    std::cout << Tag << ": "
              << "application" << idx << " started\n";

    if (rc != rcOk)
    {
        std::cerr << Tag << ": "
                  << "Failed to get Task name, error code " << rc << '\n';
        return rc;
    }

    std::cout << Tag << ": "
              << "ARGS: ";
    for (int i = 0; i < argc; ++i)
    {
        std::cout << argv[i] << ", ";
    }

    std::cout << "\n"
              << Tag << ": "
              << "ENVS: ";
    for (const char **env = envs; *env != 0; ++env)
    {
        std::cout << *env << ", ";
    }
    std::cout << "\n";

    // We notify the client who launched this application that all the work is done.
    rc = SendNotificationToClient();
    if (rc != rcOk)
    {
        std::cerr << Tag << ": "
                  << "Failed to notify client, error code " << rc << '\n';
        return rc;
    }

    // We do not terminate the application ourselves, but wait for the client
    // to send a request to the ExecutionManager component to stop the application.
    while (true)
    {
        std::cout << Tag << ": "
                  << "Task with name = " << buf << " is still alive...\n";
        sleep(20);
    }

    return EXIT_SUCCESS;
}
