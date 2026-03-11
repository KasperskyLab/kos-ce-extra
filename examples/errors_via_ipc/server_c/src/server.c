/**
 * © 2025 AO Kaspersky Lab
 * Licensed under the MIT License
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Files required for transport initialization. */
#include "transport.h"

/* Server program descriptions in EDL. */
#include <errors_via_ipc_example/Server.edl.h>

#include <component/logrr/clog/clog.h>

#include <assert.h>

typedef struct NumberSummationImpl
{
    struct errors_via_ipc_example_NumberSummation base; /* Base interface of the object. */
} NumberSummationImpl;

static const char *Tag = "C Server";

static uint64_t requestCounter = 1;

static nk_err_t SumRequest_impl(
    __unused struct errors_via_ipc_example_NumberSummation                *self,
    const struct errors_via_ipc_example_NumberSummation_SumRequest_req    *req,
    __unused const struct nk_arena                                        *req_arena,
    __unused struct errors_via_ipc_example_NumberSummation_SumRequest_res *res,
    __unused struct nk_arena                                              *res_arena)
{
    CLOG(
        INFO,
        "Received request: repeats: %u num1: %d num2: %d",
        req->request.repeats,
        req->request.num1,
        req->request.num2);

    if (requestCounter % 2)
    {
        res->result = req->request.num1 + req->request.num2;
    }
    else
    {
        CLOG(ERROR, "Server is busy");
        nk_err_reset(res);
        (res)->err_.status = rcBusy;
    }

    ++requestCounter;

    return NK_EOK;
}

/**
 * Constructor of the object implementing
 * the errors_via_ipc_example_NumberSummation interface.
 */
static struct errors_via_ipc_example_NumberSummation *CreateNumberSummationImpl(void)
{
    /* Table of interface method implementations. */
    static const struct errors_via_ipc_example_NumberSummation_ops ops = {
        .SumRequest = SumRequest_impl};

    /* Object implementing the interface. */
    static struct NumberSummationImpl impl = {.base = {&ops}};

    return &impl.base;
}

/* Server entry point. */
int main(void)
{
    NkKosTransport transport;
    ServiceId      iid;

    /* Get the server IPC handle of "server1_connection". */
    Handle handle = ServiceLocatorRegister(ServerChannelName[0], NULL, 0, &iid);
    if (handle == INVALID_HANDLE)
    {
        CLOG(ERROR, "Can`t establish static IPC connection!");
        return EXIT_FAILURE;
    }

    /* Initialize transport to the client. */
    NkKosTransport_Init(&transport, handle, NK_NULL, 0);

    /* Prepare request structures: fixed part and arena. */
    errors_via_ipc_example_Server_entity_req req;
    char            req_buffer[errors_via_ipc_example_Server_entity_req_arena_size];
    struct nk_arena req_arena = NK_ARENA_INITIALIZER(req_buffer, req_buffer + sizeof(req_buffer));

    /* Prepare response structures: fixed part. */
    errors_via_ipc_example_Server_entity_res res;
    char            res_buffer[errors_via_ipc_example_Server_entity_res_arena_size];
    struct nk_arena res_arena = NK_ARENA_INITIALIZER(res_buffer, res_buffer + sizeof(res_buffer));

    errors_via_ipc_example_Server_component component;
    errors_via_ipc_example_Server_component_init(&component, CreateNumberSummationImpl());

    /* Initialize the dispatcher of the Server program. */
    errors_via_ipc_example_Server_entity entity;
    errors_via_ipc_example_Server_entity_init(&entity, component.log);

    ClogInit(Tag);
    CLOG(INFO, "Started");

    nk_err_t nkError = NK_EOK;
    /* Dispatch loop implementation. */
    do
    {
        /* Reset the buffer containing the request. */
        nk_req_reset(&req);
        nk_arena_reset(&req_arena);

        /* Wait for the server program request. */
        nkError = nk_transport_recv(&transport.base, &req.base_, &req_arena);
        if (nkError != NK_EOK)
        {
            CLOG(ERROR, "nk_transport_recv error");
            break;
        }
        else
        {
            /**
             * Process the received request by calling the LogSequence_impl
             * implementation of the requested LogSequence interface method.
             */
            nkError = errors_via_ipc_example_Server_entity_dispatch(
                &entity, &req.base_, &req_arena, &res.base_, &res_arena);
            if (nkError != NK_EOK)
            {
                CLOG(ERROR, "Can`t process the received request");
                break;
            }
        }

        nkError = nk_transport_reply(&transport.base, &res.base_, &res_arena);
        /* Send response. */
        if (nkError != NK_EOK)
        {
            CLOG(ERROR, "nk_transport_reply error");
            break;
        }
    }
    while (true);

    return EXIT_FAILURE;
}
