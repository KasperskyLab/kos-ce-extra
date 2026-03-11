/**
 * © 2025 AO Kaspersky Lab
 * Licensed under the MIT License
 */

#include <stdlib.h>
#include <stdio.h>

#include <rtl/retcode_hr.h>
#include <rtl/compiler.h>

#include <mbim_wrapper.h>
#include <kos_net.h>

/**
 * Update variable below with your phone number and
 * uncomment to actually send SMS.
 * #define DEST_NUMBER    "+79000000000"
 */
#define SMS_TEXT "Hello from KasperskyOS!"

#define TIMEOUT_S  30
#define TIMEOUT_MS (TIMEOUT_S * 1000)

#ifndef DEST_NUMBER
#error "Destination number not set!"
#endif

static int NumberToNibbles(const char *data, size_t numlen, uint8_t *dst)
{
    size_t i;

    for (i = 0; i < numlen; ++i)
    {
        char c = data[i];

        if (c < '0' || c > '9')
            return -1;

        c -= '0';

        if (i % 2)
            dst[i / 2] = (dst[i / 2] & 0x0f) | ((c << 4) & 0xf0);
        else
            dst[i / 2] = 0xf0 | c;
    }

    return (int)((i + 1) / 2);
}

#define PDU_NUM_INTERNATIONAL    0x91
#define PDU_LEN_MIN              9
#define PDU_NUM_LEN_MAX          6
#define PDU_FLAGS_MTI_SMS_SUBMIT 0b01
#define PDU_DCS_ASCII7           0x00
#define PDU_ASCII7_MAX_DATA_LEN  160

static int AsciiToPDU(
    const char *smsc, const char *dest, const char *text, uint8_t *pdu, size_t buflen)
{
    size_t len = 0;
    size_t i;
    int    ret;

    if (!dest || !text || !pdu || buflen < PDU_LEN_MIN)
        return -1;

    /* If needed, fill in SCA. */
    if (smsc)
    {
        size_t numlen;
        size_t numbytes;

        /* Drop leading '+' if present. */
        if (smsc[0] == '+')
            smsc++;

        numlen   = strlen(smsc);
        numbytes = (numlen + 1) / 2;

        if (numbytes > PDU_NUM_LEN_MAX)
            return -1;

        *pdu++ = 1 + (uint8_t)numbytes; /* SCA.len */
        len += 2 + numbytes;

        if (buflen < len)
            return -1;

        *pdu++ = PDU_NUM_INTERNATIONAL; /* SCA.type */

        ret = NumberToNibbles(smsc, numlen, pdu);
        if (ret < 0)
            return ret;

        pdu += numbytes;
    }
    else
    {
        /* Default SCA indicated by len=0. */
        *pdu++ = 0; /* SCA.len */
        len++;
    }

    /* Now fill in TPDU. */

    *pdu++ = PDU_FLAGS_MTI_SMS_SUBMIT; /* Flags */
    *pdu++ = 0x00;                     /* Message Reference */
    len += 2;

    /* Destination Address. */
    do
    {
        size_t numlen;
        size_t numbytes;

        /* Drop leading '+' if present. */
        if (dest[0] == '+')
            dest++;

        numlen   = strlen(dest);
        numbytes = (numlen + 1) / 2;

        if (numbytes > PDU_NUM_LEN_MAX)
            return -1;

        *pdu++ = (uint8_t)numlen; /* DA.digitCount */
        len += 2 + numbytes;

        if (buflen < len)
            return -1;

        *pdu++ = PDU_NUM_INTERNATIONAL; /* DA.type */

        ret = NumberToNibbles(dest, numlen, pdu);
        if (ret < 0)
            return ret;

        pdu += numbytes;
    }
    while (0);

    *pdu++ = 0x00;           /* PID */
    *pdu++ = PDU_DCS_ASCII7; /* DCS */
    len += 2;

    /* User Data. */
    do
    {
        size_t textLen   = strlen(text);
        size_t textBytes = ((textLen * 7 + 7) / 8);
        size_t bitnum    = 0;
        size_t j;

        if (len + textBytes + 1 > buflen || textLen > PDU_ASCII7_MAX_DATA_LEN)
            return -1;

        *pdu++ = (uint8_t)textLen; /* UDL */
        len += 1 + textBytes;

        memset(pdu, 0, textBytes);

        for (i = 0; i < textLen; ++i)
        {
            for (j = 0; j < 7; ++j)
            {
                pdu[bitnum / 8] |= !!(text[i] & (1 << j)) << (bitnum % 8);
                bitnum++;
            }
        }
    }
    while (0);

    return (int)len;
}

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
            return ret;
        }

        if (subState == MBIM_SUBSCRIBER_READY_STATE_INITIALIZED)
            return 0;

        sleep(1);
    }

    fprintf(stderr, "Timeout waiting for sim! State=%d\n", subState);
    return -1;
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
            return -1;
        }

        switch (regInfo.registerState)
        {
            case MBIM_REGISTER_STATE_HOME:
            case MBIM_REGISTER_STATE_ROAMING:
            case MBIM_REGISTER_STATE_PARTNER:
                return 0;

            case MBIM_REGISTER_STATE_DENIED:
                return -1;
        }

        sleep(1);
    }

    fprintf(stderr, "Timeout waiting for registration!\n");

    return -1;
}

int main(void)
{
    Handle      mbim  = INVALID_HANDLE;
    const char *iface = "wwan0";
    int         ret;
    uint8_t     pdu[256] = {0};
    int         pduLen;
    uint32_t    reference = 0;

    /* Make sure modem device exists. */
    if (!wait_for_iface(iface, IWF_EXISTS, DEFAULT_TIMEOUT))
    {
        fprintf(stderr, "Timed out waiting for '%s'\n", iface);
        return EXIT_FAILURE;
    }

    list_network_ifaces();

    ret = VfsMbimOpen(&mbim, iface, TIMEOUT_MS);
    if (ret != 0)
    {
        MbimPerror(ret, "VfsMbimOpen");
        return EXIT_FAILURE;
    }

    ret = WaitSimReady(mbim, TIMEOUT_S);
    if (ret < 0)
    {
        VfsMbimClose(mbim);
        return EXIT_FAILURE;
    }

    ret = WaitRegistered(mbim, TIMEOUT_S);
    if (ret < 0)
    {
        VfsMbimClose(mbim);
        return EXIT_FAILURE;
    }

    pduLen = AsciiToPDU(NULL, DEST_NUMBER, SMS_TEXT, pdu, sizeof(pdu));
    if (pduLen < 0)
    {
        fprintf(stderr, "Failed to encode PDU\n");
        VfsMbimClose(mbim);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "Sending SMS '%s' to '%s'...\n", SMS_TEXT, DEST_NUMBER);

    fprintf(stderr, "PDU: {");
    for (int i = 0; i < pduLen; ++i)
    {
        fprintf(stderr, " %02x", pdu[i]);
    }
    fprintf(stderr, " }\n");

    ret = VfsMbimSmsSendPdu(mbim, pdu, (size_t)pduLen, &reference);
    if (ret != 0)
    {
        MbimPerror(ret, "VfsMbimSmsSendPdu");
    }
    else
    {
        fprintf(stderr, "Sent SMS to '%s'. Reference=%u\n", DEST_NUMBER, reference);
    }

    ret = VfsMbimClose(mbim);
    if (ret < 0)
    {
        MbimPerror(ret, "VfsMbimClose");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
