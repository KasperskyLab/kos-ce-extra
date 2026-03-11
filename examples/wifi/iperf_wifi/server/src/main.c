/**
 * © 2025 AO Kaspersky Lab
 * Licensed under the MIT License
 */

#include <stdio.h>
#include <rtl/retcode.h>
#include <iperf/iperf_api.h>
#include <kos_net.h>

#define WLAN_INTERFACE "wl0"

static Retcode SetupNetwork(void)
{
    fprintf(stderr, "[Server]: Waiting for ipv4 on network interface %s...\n", WLAN_INTERFACE);

    if (!wait_for_iface(WLAN_INTERFACE, IWF_IP4, 60))
    {
        fprintf(stderr, "[Server]: Error: Failed to set up network interface %s\n", WLAN_INTERFACE);
        return rcFail;
    }

    if (!list_network_ifaces())
    {
        fprintf(stderr, "[Server]: Error: Failed to list network interfaces\n");
        return rcFail;
    }

    return rcOk;
}

int main(int argc, char* argv[])
{
    struct iperf_test* iperfTest = NULL;

    if (rcOk != SetupNetwork())
    {
        return EXIT_FAILURE;
    }

    iperfTest = iperf_new_test();
    if (iperfTest == NULL)
    {
        fprintf(stderr, "[Server]: Error: Test creation failed\n");
        return EXIT_FAILURE;
    }

    if (iperf_defaults(iperfTest) < 0)
    {
        fprintf(stderr, "[Server]: Error: Test initialization failed\n");
        iperf_free_test(iperfTest);
        return EXIT_FAILURE;
    }

    if (iperf_parse_arguments(iperfTest, argc, argv) < 0)
    {
        fprintf(stderr, "[Server]: Error: Arguments parsing failed\n");
        iperf_free_test(iperfTest);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "[Server]: Test started\n");

    if (run_test(iperfTest) < 0)
    {
        fprintf(stderr, "[Server]: Error: Test run failed\n");
        iperf_free_test(iperfTest);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "[Server]: Test finished\n");

    iperf_free_test(iperfTest);

    return EXIT_SUCCESS;
}
