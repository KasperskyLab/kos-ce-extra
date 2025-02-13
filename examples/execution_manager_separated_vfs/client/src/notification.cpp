/*
 * © 2025 AO Kaspersky Lab
 * Licensed under the MIT License
 */

#include "notification.h"

#include <iostream>

#include <coresrv/nk/transport-kos.h>
#include <coresrv/ns/ns_api.h>
#include <coresrv/cm/cm_api.h>

#include <client/Notification.idl.h>

#include <client/Client.edl.h>

#include <kl/core/Types.idl.h>

extern const char Tag[];

using namespace kos::rtl;

static nk_err_t Notification_impl(
    [[maybe_unused]] struct client_Notification                        *self,
    [[maybe_unused]] const struct client_Notification_Notification_req *req,
    [[maybe_unused]] const struct nk_arena                             *req_arena,
    [[maybe_unused]] struct client_Notification_Notification_res       *res,
    [[maybe_unused]] struct nk_arena                                   *res_arena)
{
    std::cout << Tag << ": Received notification from the application\n";
    return NK_EOK;
}

Result PublishService()
{
    NsHandle ns;
    Result   rc = TO_RESULT(NsCreate(RTL_NULL, INFINITE_TIMEOUT, &ns));
    if (rc != result_code::Ok)
    {
        std::cerr << Tag << ": Failed to create the ns handle\n";
        return rc;
    }

    rc = TO_RESULT(
        NsPublishService(
            ns,
            "client.Notification",
            "client.Client",
            "client.Client.notificationImpl"));
    if (rc != result_code::Ok)
    {
        std::cerr << Tag << ": Failed to publish the endpoints on the name server\n";
        return rc;
    }

    return rc;
}

Result WaitForNotification()
{
    char client[kl_core_Types_UCoreStringSize];
    char service[kl_core_Types_UCoreStringSize];

    NkKosTransport transport;
    Handle         handle;

    Result rc = TO_RESULT(KnCmListen(RTL_NULL, INFINITE_TIMEOUT, client, service));
    if (rc != result_code::Ok)
    {
        std::cerr << Tag << ": Failed to start listen for IPC connections\n";
        return rc;
    }

    rc = TO_RESULT(KnCmAccept(client, service, 0, INVALID_HANDLE, &handle));
    if (rc != result_code::Ok)
    {
        std::cerr << Tag << ": Failed to accept the IPC connection\n";
        return rc;
    }

    NkKosTransport_Init(&transport, handle, NK_NULL, 0);

    const struct client_Notification_ops notification_ops = {.Notification = Notification_impl};
    struct client_Notification           notification     = {.ops = &notification_ops};
    struct client_Client_entity          entity;

    client_Client_entity_init(&entity, &notification);

    union client_Client_entity_req req;
    union client_Client_entity_res res;

    nk_req_reset(&req);

    nk_err_t nk_rc = nk_transport_recv(&transport.base, &req.base_, NULL);
    if (nk_rc != NK_EOK)
    {
        std::cerr << Tag << ": Failed to receive request from the application\n";
        return result_code::TransferError;
    }

    client_Client_entity_dispatch(&entity, &req.base_, NULL, &res.base_, NULL);

    nk_rc = nk_transport_reply(&transport.base, &res.base_, NULL);
    if (nk_rc != NK_EOK)
    {
        std::cerr << Tag << ": Failed to send response to the application\n";
        return result_code::TransferError;
    }

    return result_code::Ok;
}
