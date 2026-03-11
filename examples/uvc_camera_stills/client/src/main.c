/**
 * © 2025 AO Kaspersky Lab
 * Licensed under the MIT License
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <sys/endian.h>

#include <rtl/retcode_hr.h>

#include <bsp/bsp.h>
#include <camera/camera.h>

#define SNAPSHOT_COUNT    3
#define SNAPSHOT_DELAY_MS 1000

/**
 * Hardware platforms can only support limited bandwidth. Exceeding the bandwidth capacity may
 * result in corrupted or partial frames.
 */
#define MAX_HEIGHT_MJPEG 1080
#define MAX_HEIGHT_YUYV  240

static Retcode ConfigureCamera(CameraHandle camera, size_t height, CameraPixelFormat pFmt)
{
    Retcode      rc;
    ssize_t      bestProfileIdx = -1;
    size_t       bestHeight     = 0;
    CameraInfo   info;
    unsigned int i = 0;

    while ((rc = CameraEnumProfiles(camera, i, &info)) == rcOk)
    {
        if (info.pFmt == pFmt)
        {
            if (info.resY <= height && info.resY > bestHeight)
            {
                bestHeight     = info.resY;
                bestProfileIdx = i;
            }
        }
        i++;
    }
    if (rc != rcResourceNotFound)
    {
        fprintf(
            stderr, "Failed to enumerate profiles: " RETCODE_HR_FMT "\n", RETCODE_HR_PARAMS(rc));
        return rc;
    }

    if (bestProfileIdx == -1)
        return rcResourceNotFound;

    return CameraSelectProfile(camera, (unsigned int)bestProfileIdx);
}

#define clamp_u8(x) ((uint8_t)rtl_max(0, rtl_min(x, 255)))

static void YUYVToRGB(
    void *yuyvv, size_t yuyvSize, void *rgb, size_t __attribute__((unused)) rgbSize)
{
    size_t i;
    struct
    {
        uint8_t y1;
        uint8_t u;
        uint8_t y2;
        uint8_t v;
    } __attribute__((packed)) *src = yuyvv;
    struct
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } __attribute__((packed)) *dst = rgb;

    assert(yuyvSize % sizeof(*src) == 0);
    assert(rgbSize % sizeof(*dst) == 0);
    assert(yuyvSize / sizeof(*src) * 2 == rgbSize / sizeof(*dst));

    for (i = 0; i < yuyvSize / sizeof(*src); ++i)
    {
        float y1 = src[i].y1 - 16;
        float y2 = src[i].y2 - 16;
        float u  = src[i].u - 128;
        float v  = src[i].v - 128;

        dst[i * 2 + 0].r = clamp_u8(y1 * 1.00 + u * 0.00 + v * 1.28);
        dst[i * 2 + 0].g = clamp_u8(y1 * 1.00 + u * -0.21 + v * -0.38);
        dst[i * 2 + 0].b = clamp_u8(y1 * 1.00 + u * 2.13 + v * 0.00);

        dst[i * 2 + 1].r = clamp_u8(y2 * 1.00 + u * 0.00 + v * 1.28);
        dst[i * 2 + 1].g = clamp_u8(y2 * 1.00 + u * -0.21 + v * -0.38);
        dst[i * 2 + 1].b = clamp_u8(y2 * 1.00 + u * 2.13 + v * 0.00);
    }
}

static Retcode SaveYUYV(void *data, size_t size, CameraInfo *info, RtlTimeSpec *ts)
{
    char   filename[NAME_MAX];
    FILE  *file;
    size_t written;
    void  *rgb;
    size_t rgbSize;

    assert(info->bpp == 2);

    snprintf(
        filename, sizeof(filename), "/img_%04ld_%03d.ppm", ts->sec, ts->nsec / RTL_NSEC_PER_MSEC);

    file = fopen(filename, "wb");
    if (!file)
    {
        perror("Failed to create file");
        return rcFail;
    }

    /* PPM header. */
    fprintf(file, "P6\n\n%7u %7u\n255\n", info->resX, info->resY);

    rgbSize = info->resX * info->resY * 3;
    rgb     = malloc(rgbSize);

    YUYVToRGB(data, size, rgb, rgbSize);

    written = fwrite(rgb, 1, rgbSize, file);
    free(rgb);
    fclose(file);

    if (written != rgbSize)
        return rcFail;

    fprintf(stderr, "Saved image to '%s'\n", filename);

    return rcOk;
}

static size_t GetJpegSize(uint8_t *jpeg, size_t size)
{
    size_t i = 0;
    enum
    {
        JpegPadding = 0xff,
        JpegFF      = 0x00,
        JpegSOI     = 0xd8,
        JpegEOI     = 0xd9,
        JpegTEM     = 0x01,
        JpegRST0    = 0xd0,
        JpegRST7    = 0xd7,
        JpegSOS     = 0xda,
    };

    while (i < size)
    {
        switch (jpeg[i++])
        {
            case JpegPadding:
            case JpegSOI:
            case JpegTEM:
            case JpegRST0 ... JpegRST7:
                break;

            case JpegSOS:
                while (i < size - 1)
                {
                    if (jpeg[i++] != JpegPadding)
                        continue;

                    switch (jpeg[i])
                    {
                        case JpegFF:
                        case JpegRST0 ... JpegRST7:
                            continue;
                    }

                    break;
                }
                break;

            default:
                if (i + sizeof(uint16_t) >= size)
                    return 0;

                i += be16toh(*(uint16_t *)(void *)&jpeg[i]);
                break;

            case JpegEOI:
                return i;
        }
    }

    return 0;
}

static Retcode SaveMJPEG(void *data, size_t size, RtlTimeSpec *ts)
{
    size_t actualSize;
    char   filename[NAME_MAX];
    FILE  *file;
    size_t written;

    actualSize = GetJpegSize(data, size);
    if (actualSize == 0)
        return rcFail;

    snprintf(
        filename, sizeof(filename), "/img_%04ld_%03d.jpg", ts->sec, ts->nsec / RTL_NSEC_PER_MSEC);


    file = fopen(filename, "wb");
    if (!file)
    {
        perror("Failed to create file");
        return rcFail;
    }

    written = fwrite(data, 1, actualSize, file);
    fclose(file);

    if (written != actualSize)
        return rcFail;

    fprintf(stderr, "Saved image to '%s'\n", filename);

    return rcOk;
}

static Retcode MakeSnapshots(CameraHandle camera)
{
    Retcode    rc;
    CameraInfo info;
    char      *pixFmtName;
    size_t     i;
    uint32_t   dataSize;
    void      *data;

    rc = CameraGetInfo(camera, &info);
    if (rc != rcOk)
    {
        fprintf(stderr, "Failed to get info: " RETCODE_HR_FMT "\n", RETCODE_HR_PARAMS(rc));
        return rc;
    }

    switch (info.pFmt)
    {
        case CAMERA_PIX_FMT_YUYV:
            pixFmtName = "Uncompressed YUY2";
            break;
        case CAMERA_PIX_FMT_MJPEG:
            pixFmtName = "MJPEG";
            break;
        default:
            /* UVC cameras pretty much exclusively use MJPEG and/or YUY2. */
            fprintf(stderr, "Unsupported pixel format: %d\n", info.pFmt);
            return rcNoCapability;
    }

    fprintf(stderr, "Making %dx%d %s snapshots\n", info.resX, info.resY, pixFmtName);

    rc = CameraEnable(camera);
    if (rc != rcOk)
    {
        fprintf(stderr, "Failed to enable camera: " RETCODE_HR_FMT "\n", RETCODE_HR_PARAMS(rc));
        return rc;
    }

    /**
     * For uncompressed frames we allocate exactly as needed, for
     * MJPEG 1 byte per pixel should be generous enough.
     */
    dataSize = info.resX * info.resY * rtl_max(info.bpp, 1);
    data     = malloc(dataSize);

    for (i = 0; i < SNAPSHOT_COUNT; ++i)
    {
        RtlTimeSpec timestamp;

        /**
         * Some cameras need to "warm up" before they start the
         * stream, so we put the delay first here.
         */
        KnSleep(SNAPSHOT_DELAY_MS);

        rc = CameraReadFrame(camera, data, dataSize, &timestamp);
        if (rc != rcOk)
        {
            fprintf(
                stderr,
                "Failed to read frame[%ld]: " RETCODE_HR_FMT "\n",
                i,
                RETCODE_HR_PARAMS(rc));
            continue;
        }

        switch (info.pFmt)
        {
            case CAMERA_PIX_FMT_YUYV:
                rc = SaveYUYV(data, dataSize, &info, &timestamp);
                break;
            case CAMERA_PIX_FMT_MJPEG:
                rc = SaveMJPEG(data, dataSize, &timestamp);
                break;
            default:
                assert(false); /* Unreachable. */
        }
        if (rc != rcOk)
        {
            fprintf(
                stderr,
                "Failed to save frame[%ld]: " RETCODE_HR_FMT "\n",
                i,
                RETCODE_HR_PARAMS(rc));
        }
    }

    free(data);
    CameraDisable(camera);
    return rc;
}

static const char *CameraCtrlToStr(CameraCtrl v)
{
    switch (v)
    {
        case CAMERA_CTRL_BRIGHTNESS:
            return "BRIGHTNESS";
        case CAMERA_CTRL_CONTRAST:
            return "CONTRAST";
        case CAMERA_CTRL_SATURATION:
            return "SATURATION";
        case CAMERA_CTRL_SHARPNESS:
            return "SHARPNESS";
        case CAMERA_CTRL_GAIN:
            return "GAIN";
        case CAMERA_CTRL_ANALOGUE_GAIN:
            return "ANALOGUE_GAIN";
        case CAMERA_CTRL_EXPOSURE:
            return "EXPOSURE";
        case CAMERA_CTRL_HFLIP:
            return "HFLIP";
        case CAMERA_CTRL_VFLIP:
            return "VFLIP";
        default:
            return "(unknown)";
    }
}

static Retcode InterrogateCamera(CameraHandle camera)
{
    CameraInfo   info;
    Retcode      rc;
    unsigned int i;

    fprintf(stderr, "Camera controls:\n");
    for (i = 0; i <= CAMERA_CTRL_VFLIP; ++i)
    {
        CameraCtrlInfo ci;
        int            val;

        rc = CameraGetCtrlInfo(camera, i, &ci);
        if (rc == rcOk)
            rc = CameraGetCtrlValue(camera, i, &val);

        if (rc == rcNoCapability)
        {
            fprintf(stderr, " %15s: (No capability)\n", CameraCtrlToStr(i));
        }
        else if (rc != rcOk)
        {
            fprintf(
                stderr, " %15s: " RETCODE_HR_FMT "\n", CameraCtrlToStr(i), RETCODE_HR_PARAMS(rc));
        }
        else
        {
            fprintf(
                stderr,
                " %15s: % 6d ->% 6d [+%d]  == % 6d\n",
                CameraCtrlToStr(i),
                ci.min,
                ci.max,
                ci.step,
                val);
        }
    }

    fprintf(stderr, "\nCamera profiles:\n");

    i = 0;
    while ((rc = CameraEnumProfiles(camera, i, &info)) == rcOk)
    {
        fprintf(
            stderr,
            "  [%2d]   %dx%d@%d\t%dbpp\tfmt=%d\n",
            i,
            info.resX,
            info.resY,
            info.fps,
            info.bpp,
            info.pFmt);
        i++;
    }

    return rcOk;
}

static Retcode CaptureCameraSnapshots(void)
{
    Retcode      rc;
    char         cameraName[BSP_NAME_MAX];
    CameraHandle camera;

    fprintf(stderr, "Waiting for camera...\n");

    /* Wait for any camera to appear. */
    while ((rc = CameraEnumPorts(0, sizeof(cameraName), cameraName)) == rcResourceNotFound)
        KnSleep(500);

    if (rc != rcOk)
    {
        fprintf(stderr, "Failed to find camera: " RETCODE_HR_FMT "\n", RETCODE_HR_PARAMS(rc));
        return rc;
    }

    fprintf(stderr, "Detected camera '%s'\n", cameraName);

    rc = CameraOpenPort(cameraName, &camera);
    if (rc != rcOk)
    {
        fprintf(stderr, "Failed to open camera: " RETCODE_HR_FMT "\n", RETCODE_HR_PARAMS(rc));
        return rc;
    }

    rc = InterrogateCamera(camera);
    if (rc != rcOk)
        return rc;

    fprintf(stderr, "Trying to make MJPEG snapshot...\n");

    rc = ConfigureCamera(camera, MAX_HEIGHT_MJPEG, CAMERA_PIX_FMT_MJPEG);
    if (rc == rcResourceNotFound)
    {
        fprintf(stderr, "Camera doesn't support MJPEG, skipping.\n");
    }
    else if (rc == rcOk)
    {
        rc = MakeSnapshots(camera);
        if (rc != rcOk)
        {
            fprintf(
                stderr, "Failed to make snapshots: " RETCODE_HR_FMT "\n", RETCODE_HR_PARAMS(rc));
        }
    }

    fprintf(stderr, "Trying to make Uncompressed YUY2 snapshot...\n");

    rc = ConfigureCamera(camera, MAX_HEIGHT_YUYV, CAMERA_PIX_FMT_YUYV);
    if (rc == rcResourceNotFound)
    {
        fprintf(stderr, "Camera doesn't support YUY2, skipping.\n");
    }
    else if (rc == rcOk)
    {
        rc = MakeSnapshots(camera);
        if (rc != rcOk)
        {
            fprintf(
                stderr, "Failed to make snapshots: " RETCODE_HR_FMT "\n", RETCODE_HR_PARAMS(rc));
        }
    }

    return CameraClosePort(camera);
}

int main(void)
{
    Retcode rc;
    int     ret = EXIT_SUCCESS;

    fprintf(stderr, "Starting camera example.\n");

    rc = CameraInit();
    if (rc != rcOk)
    {
        fprintf(stderr, "CameraInit failed: " RETCODE_HR_FMT "\n", RETCODE_HR_PARAMS(rc));
        return EXIT_FAILURE;
    }

    rc = CaptureCameraSnapshots();
    if (rc != rcOk)
    {
        fprintf(stderr, "Failed to capture snapshots: " RETCODE_HR_FMT "\n", RETCODE_HR_PARAMS(rc));
        ret = EXIT_FAILURE;
    }

    fprintf(stderr, "Camera example done.\n");

    CameraFini();
    return ret;
}
