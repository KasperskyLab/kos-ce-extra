/**
 * © 2025 AO Kaspersky Lab
 * Licensed under the MIT License
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <time.h>
#include <kos/thread.h>
#include <rtl/retcode.h>

/* Files required for transport initialization. */
#include "transport.h"

/* Client program descriptions in EDL. */
#include <errors_via_ipc_example/Client.edl.h>
#include <errors_via_ipc_example/NumberSummation.idl.h>

#include <component/logrr/clog/clog.h>

#define SERVERS_COUNT    2
#define CHANNEL_NAME_LEN 20
#define ENDP_NAME_LEN    35
#define PERM_DELAY_MS    1000
#define BUSY_DELAY_MS    100

static const char *Tag        = "C Client";
static const char *Servers[2] = {"C Server", "CXX Server"};

typedef struct errors_via_ipc_example_NumberSummation_proxy NumberSummationProxy;

static Retcode ServerTransportInit(
    const char *channelName, Handle *outHandle, NkKosTransport *transport, nk_iid_t *outRiid)
{
    const char *endpName = "log";

    /* Get the client IPC handle of a server connection. */
    Handle handle = ServiceLocatorConnect(channelName);
    if (handle == INVALID_HANDLE)
    {
        CLOG(ERROR, "Can`t establish static IPC connection!");
        return rcFail;
    }

    /* Initialize IPC transport for interaction with the ServerN program. */
    NkKosTransport_Init(transport, handle, NK_NULL, 0);

    /**
     * Get the Runtime Interface ID (RIID) for the
     * errors_via_ipc_example.ServerN.log interface.
     */
    nk_iid_t riid = ServiceLocatorGetRiid(handle, endpName);
    if (riid == INVALID_RIID)
    {
        CLOG(
            ERROR,
            "Error: can`t get runtime implementation ID (RIID) of "
            "interface errors_via_ipc_example.Server.log!");
        return rcFail;
    }

    *outHandle = handle;
    *outRiid   = riid;

    return rcOk;
}

/* Client entry point. */
int main(void)
{
    NkKosTransport       transport[SERVERS_COUNT];
    NumberSummationProxy proxy[SERVERS_COUNT];
    Handle               handle[SERVERS_COUNT] = {INVALID_HANDLE, INVALID_HANDLE};
    nk_iid_t             riid[SERVERS_COUNT]   = {INVALID_RIID, INVALID_RIID};

    Retcode rc = rcFail;

    for (uint8_t i = 0; i < SERVERS_COUNT; ++i)
    {
        rc = ServerTransportInit(ServerChannelName[i], &handle[i], &transport[i], &riid[i]);

        if (rc != rcOk)
        {
            return EXIT_FAILURE;
        }

        /**
         * Initialize proxy-object. Every method of proxy-object will be
         * realized as request to server.
         */
        errors_via_ipc_example_NumberSummation_proxy_init(&proxy[i], &transport[i].base, riid[i]);
        if (NK_NULL == &proxy[i])
        {
            return EXIT_FAILURE;
        }
    }

    ClogInit(Tag);
    CLOG(INFO, "Started");

    errors_via_ipc_example_NumberSummation_SumRequest_req req;
    errors_via_ipc_example_NumberSummation_SumRequest_res res;
    int                                                   curServer = 0;

    srand((unsigned)time(NULL));

    nk_err_t nkError = NK_EOK;
    do
    {
        curServer           = (uint32_t)rand() % SERVERS_COUNT;
        req.request.num1    = (uint32_t)rand() % 100;
        req.request.num2    = (uint32_t)rand() % 100;
        req.request.repeats = 0;

        for (;;)
        {
            nkError = errors_via_ipc_example_NumberSummation_SumRequest(
                &proxy[curServer].base, &req, NULL, &res, NULL);

            if (nkError == NK_EOK)
            {
                if (res.err_.status == rcBusy)
                {
                    CLOG(INFO, "'%s' is busy, changing server...", Servers[curServer]);
                    curServer ^= 1;
                    ++req.request.repeats;
                    KnSleep(BUSY_DELAY_MS);
                }
                else
                {
                    CLOG(INFO, "Result = %ld", res.result);
                    KnSleep(PERM_DELAY_MS);
                    break;
                }
            }
            else if (nkError == NK_EPERM)
            {
                CLOG(
                    ERROR,
                    "Transport error, number of repeats exceeded %d!",
                    req.request.repeats - 1);

                KnSleep(PERM_DELAY_MS);
                break;
            }
            else
            {
                CLOG(ERROR, "SumRequest error: %d", nkError);
            }
        }
    }
    while (true);

    return nkError == NK_EOK ? EXIT_SUCCESS : EXIT_FAILURE;
}
