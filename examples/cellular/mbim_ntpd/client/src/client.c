/**
 * © 2026 AO Kaspersky Lab
 * Licensed under the MIT License
 */

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <kos_net.h>
#include <rtl/rtc.h>

#include <mbim_wrapper.h>
#include <net/mbim.h>

#define WWAN_IFACE    "wwan0"
#define WWAN_APN      u"internet"
#define WWAN_USER     u""
#define WWAN_PASSWORD u""
#define TIMEOUT_S     30
#define TIMEOUT_MS    (TIMEOUT_S * 1000)

#define WHITE "\e[1;37m"
#define RED   "\e[1;31m"
#define GREEN "\e[1;32m"
#define NC    "\e[0m"

#define KOS_BIRTHDAY_YEAR  2011
#define KOS_BIRTHDAY_MONTH 11
#define KOS_BIRTHDAY_DAY   11
#define ATTEMPTS           10
#define TIME_STEP_SEC      5

static int MbimPerror(int ret, const char *msg)
{
    if (ret < 0)
        perror(msg);
    else if (ret > 0)
        fprintf(stderr, "%s: MBIM Status %d\n", msg, ret);

    return ret;
}

static int WaitSimReady(Handle mbim, int seconds)
{
    uint32_t subState;
    int      timeout;
    int      ret;

    for (timeout = 0; timeout < seconds; ++timeout)
    {
        ret = VfsMbimInfoSim(mbim, &subState);
        if (ret != 0)
        {
            MbimPerror(ret, "VfsMbimInfoSim");
            return EXIT_FAILURE;
        }

        if (subState == MBIM_SUBSCRIBER_READY_STATE_INITIALIZED)
            return EXIT_SUCCESS;

        sleep(1);
    }

    fprintf(stderr, "Timeout waiting for sim! State=%d\n", subState);
    return EXIT_FAILURE;
}

static int WaitRegistered(Handle mbim, int seconds)
{
    int timeout;
    int ret;

    for (timeout = 0; timeout < seconds; ++timeout)
    {
        uint16_t providerId[MBIM_PROVIDER_ID_LEN_MAX]     = u"";
        uint16_t providerName[MBIM_PROVIDER_NAME_LEN_MAX] = u"";
        uint16_t roamingText[MBIM_ROAMING_TEXT_LEN_MAX]   = u"";

        MbimLibcRegistrationStateInfo regInfo = {
            .providerId      = providerId,
            .providerIdLen   = sizeof(providerId),
            .providerName    = providerName,
            .providerNameLen = sizeof(providerName),
            .roamingText     = roamingText,
            .roamingTextLen  = sizeof(roamingText),
        };

        ret = VfsMbimInfoRegisterState(mbim, &regInfo);
        if (ret != 0)
        {
            MbimPerror(ret, "VfsMbimInfoRegisterState");
            return EXIT_FAILURE;
        }

        switch (regInfo.registerState)
        {
            case MBIM_REGISTER_STATE_HOME:
            case MBIM_REGISTER_STATE_ROAMING:
            case MBIM_REGISTER_STATE_PARTNER:
                return EXIT_SUCCESS;

            case MBIM_REGISTER_STATE_DENIED:
                return EXIT_FAILURE;
        }

        sleep(1);
    }

    fprintf(stderr, "Timeout waiting for registration!\n");

    return EXIT_FAILURE;
}

static int SetDefaultGateway(in_addr_t *addr)
{
    int             sockfd;
    struct ortentry newRt = {
        /* clang-format off */
        .rt_dst =
        {
            .ss_family = AF_INET,
            .ss_len    = sizeof(struct sockaddr_in),
            /* .sin_addr = INADDR_ANY, */
        },
        .rt_gateway =
        {
            .ss_family = AF_INET,
            .ss_len    = sizeof(struct sockaddr_in),
        },
        .rt_mask =
        {
            .ss_family = AF_INET,
            .ss_len    = sizeof(struct sockaddr_in),
            /* .sin_addr = INADDR_ANY, */
        },
        .rt_flags = RTF_UP | RTF_GATEWAY,
        /* clang-format on */
    };
    struct sockaddr_in *gateway = (struct sockaddr_in *)&newRt.rt_gateway;

    static_assert(INADDR_ANY == 0, "INADDR_ANY is not a broadcast address");

    sockfd = socket(AF_ROUTE, SOCK_RAW, 0);
    if (sockfd == -1)
    {
        perror("socket(AF_ROUTE, SOCK_RAW, 0) failed");
        return EXIT_FAILURE;
    }

    memcpy(&gateway->sin_addr.s_addr, addr, sizeof(*addr));

    if (ioctl(sockfd, SIOCADDRT, &newRt) < 0)
    {
        if (errno == EEXIST)
        {
            newRt.rt_update = 1;
            if (ioctl(sockfd, SIOCADDRT, &newRt) == 0)
            {
                close(sockfd);
                return EXIT_SUCCESS;
            }
        }

        perror("ioctl(SIOCADDRT) failed");
        close(sockfd);
        return EXIT_FAILURE;
    }

    close(sockfd);
    return EXIT_SUCCESS;
}

/* char16_t is uint_least16_t */
static_assert(sizeof(u'\0') == sizeof(uint16_t), "Width of char16_t must be 16 bits");

static uint32_t StrLen16PadEven(uint16_t *s)
{
    uint32_t ret = 0;

    while (s && *s++)
        ret++;

    if (ret % 2 == 1)
        ret++;

    return ret;
}

static int MbimConnect(Handle mbim, uint16_t *apn, uint16_t *userName, uint16_t *password)
{
    int              ret;
    MbimLibcIpConfig ipConfig;

    ret = WaitSimReady(mbim, TIMEOUT_S);
    if (ret != 0)
        return EXIT_FAILURE;

    ret = WaitRegistered(mbim, TIMEOUT_S);
    if (ret != 0)
        return EXIT_FAILURE;

    ret = VfsMbimConnect(
        mbim,
        0,
        apn,
        StrLen16PadEven(apn),
        userName,
        StrLen16PadEven(userName),
        password,
        StrLen16PadEven(password),
        MBIM_COMPRESSION_NONE,
        (userName[0] == u'\0' ? MBIM_AUTH_PROTOCOL_NONE : MBIM_AUTH_PROTOCOL_PAP),
        MBIM_CONTEXT_IP_TYPE_DEFAULT,
        MBIM_CONTEXT_TYPE_INTERNET);
    if (ret != 0)
    {
        MbimPerror(ret, "VfsMbimConnect failed");
        return EXIT_FAILURE;
    }

    /* Connection must result in an event that establishes an IP. */
    if (wait_for_iface(WWAN_IFACE, IWF_IP4, 60) == false)
        return EXIT_FAILURE;

    /* Need to set the default gateway. */
    ret = VfsMbimInfoIpConnection(mbim, &ipConfig);
    if (ret != 0)
    {
        MbimPerror(ret, "VfsMbimInfoIpConnection failed");
        return EXIT_FAILURE;
    }

    if (ipConfig.ipv4.addressCount < 1)
    {
        fprintf(stderr, "No IPv4 address in the connection!\n");
        return EXIT_FAILURE;
    }

    static_assert(
        sizeof(ipConfig.ipv4.addresses[0].address) == sizeof(in_addr_t),
        "Mbim IPv4 address is not compatible with in_addr_t");

    return SetDefaultGateway((in_addr_t *)ipConfig.ipv4.addresses[0].address);
}

static int GetCurrentTimeinfo(struct tm **timeinfo)
{
    time_t rawtime;

    if (timeinfo == NULL)
        return EXIT_FAILURE;

    time(&rawtime);
    *timeinfo = localtime(&rawtime);
    if (*timeinfo == NULL)
    {
        perror("localtime failed");
        return EXIT_FAILURE;
    }
    fprintf(stderr, "Current date and time: " WHITE "%s" NC, asctime(*timeinfo));

    return EXIT_SUCCESS;
}

static int SetInitialTime(void)
{
    struct tm      *timeinfo;
    struct timespec ts;
    time_t          rawtime;

    if (GetCurrentTimeinfo(&timeinfo) != 0)
    {
        perror("getCurrentTimeinfo failed");
        return EXIT_FAILURE;
    }

    timeinfo->tm_year = KOS_BIRTHDAY_YEAR - RTL_INITIAL_YEAR;
    timeinfo->tm_mon  = KOS_BIRTHDAY_MONTH - 1;
    timeinfo->tm_mday = KOS_BIRTHDAY_DAY;
    rawtime           = mktime(timeinfo);
    ts.tv_sec         = rawtime;
    ts.tv_nsec        = 0;

    fprintf(
        stderr,
        "Set current date to %d.%d.%d\n",
        KOS_BIRTHDAY_DAY,
        KOS_BIRTHDAY_MONTH,
        KOS_BIRTHDAY_YEAR);

    if (clock_settime(CLOCK_REALTIME, &ts) != 0)
    {
        perror("clock_settime failed");
        return EXIT_FAILURE;
    }

    if (GetCurrentTimeinfo(&timeinfo) != 0)
    {
        perror("getCurrentTimeinfo failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int WaitTimeUpdate(void)
{
    struct tm *timeinfo;

    for (int i = 0; i < ATTEMPTS; i++)
    {
        if (GetCurrentTimeinfo(&timeinfo) != 0)
        {
            perror("getCurrentTimeinfo failed");
            return EXIT_FAILURE;
        }

        if (timeinfo->tm_year > KOS_BIRTHDAY_YEAR - RTL_INITIAL_YEAR)
        {
            fprintf(stderr, GREEN "Test passed\n" NC);
            return EXIT_SUCCESS;
        }

        sleep(TIME_STEP_SEC);
    }

    fprintf(stderr, RED "Test failed\n" NC);
    return EXIT_FAILURE;
}

int main(void)
{
    Handle mbim = INVALID_HANDLE;
    int    ret;

    ret = SetInitialTime();
    if (ret != EXIT_SUCCESS)
        return ret;

    /* Make sure modem device exists. */
    if (wait_for_iface(WWAN_IFACE, IWF_EXISTS, 60) == false)
    {
        fprintf(stderr, "Timed out waiting for '%s'\n", WWAN_IFACE);
        return EXIT_FAILURE;
    }

    ret = VfsMbimOpen(&mbim, WWAN_IFACE, TIMEOUT_MS);
    if (ret != 0)
    {
        MbimPerror(ret, "VfsMbimOpen");
        return EXIT_FAILURE;
    }

    ret = MbimConnect(mbim, WWAN_APN, WWAN_USER, WWAN_PASSWORD);
    if (ret != 0)
    {
        VfsMbimClose(mbim);
        return EXIT_FAILURE;
    }

    list_network_ifaces();

    ret = WaitTimeUpdate();

    VfsMbimClose(mbim);
    return ret;
}
