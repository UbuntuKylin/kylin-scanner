/*
* Copyright (C) 2020, KylinSoft Co., Ltd.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
*
*/
#include "scanner.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STRIP_HEIGHT 256
#define A4BRY 297 // Some scanner device will less than 297

typedef struct _Image {
    uint8_t *data;
    int width;
    int height;
    int x;
    int y;
} Image;

typedef struct _OptDescriptor {
    int numColorMode;
    int numSource;
    int numResolution;
    int numSizeBrX;
    int numSizeBrY;
} OptDescriptor;
static OptDescriptor g_optDesc = {2, 3, 6, 10, 11};

static double g_saneSizeA4BrY = 297;
static SANE_Handle g_device = nullptr;
static int g_verbose;
static SANE_Byte *g_buf;
static size_t g_BufSize;
static SANE_Device *g_saneDevice = nullptr;
const SANE_Device **g_deviceList = nullptr;

#define SET_1_BIT(n,i) ((1<<(i))|(n))
#define SET_0_BIT(n,i) ((~(1<<(i)))&(n))
#define SET_R_BIT(n,i) ((n)^(1<<(i)))
#define GET_i_BIT(n,i) (((n)>>(i))&1)


static void writePnmHeader (SANE_Frame format, int width, int height, int depth, FILE *ofp)
{
    switch (format) {
    case SANE_FRAME_RED:
    case SANE_FRAME_GREEN:
    case SANE_FRAME_BLUE:
    case SANE_FRAME_RGB:
        fprintf(ofp, "P6\n# SANE data follows\n%d %d\n%d\n",
                width, height, (depth <= 8) ? 255 : 65535);
        qInfo() << "#P6 SANE data follows width = " << width << "height = " << height
                << "depth = " << ((depth <= 8) ? 255 : 65535);
        break;
    default:
        if (depth == 1) {
            fprintf(ofp, "P4\n# SANE data follows\n%d %d\n", width, height);
            qInfo() << "#P4 SANE data follows width = " << width << "height = " << height;
        } else {
            fprintf(ofp, "P5\n# SANE data follows\n%d %d\n%d\n",
                    width, height, (depth <= 8) ? 255 : 65535);
            qInfo() << "#P5 SANE data follows width = " << width << "height = " << height
                    << "depth = " << ((depth <= 8) ? 255 : 65535);
        }
        break;
    }
}

static void *advance (Image *image)
{
    if (++image->x >= image->width) {
        image->x = 0;
        if ((++image->y >= image->height) || !image->data) {
            size_t old_size = 0, new_size;

            if (image->data)
                old_size = static_cast<size_t>(image->height * image->width);

            image->height += STRIP_HEIGHT;
            new_size = static_cast<size_t>(image->height * image->width);

            if (image->data) {
                image->data = (uint8_t *) realloc (image->data, new_size);
            } else {
                image->data = (uint8_t *) malloc (new_size);
            }

            if (image->data)
                memset (image->data + old_size, 0, new_size - old_size);
        }
    }

    if (!image->data)
        qInfo() << "Can't allocate image buffer, width = " << image->width
                << "height = " << image->height;

    return image->data;
}

static SANE_Status onScanning(FILE *ofp)
{
    int i, len, first_frame = 1, offset = 0, must_buffer = 0, hundred_percent;
    SANE_Byte min = 0xff, max = 0;
    SANE_Parameters parm;
    SANE_Status status;
    Image image = { nullptr, 0, 0, 0, 0 };
    SANE_Word total_bytes = 0;
    SANE_Int hang_over = -1;

    do {
        if (!first_frame) {
            status = sane_start (g_device);
            if (status != SANE_STATUS_GOOD)
                goto cleanup;
        }

        status = sane_get_parameters (g_device, &parm);
        qInfo() << "Parm : status = " << sane_strstatus(status)
                << "format = " << parm.format
                << "last_frame = " << parm.last_frame
                << "bytes_per_line = " << parm.bytes_per_line
                << "pixels_per_line = " << parm.pixels_per_line
                << "lines = " << parm.lines
                << "depth = " << parm.depth;

        if (status != SANE_STATUS_GOOD)
            goto cleanup;

        if (first_frame) {
            if (parm.lines >= 0) {
                qInfo() << "Image's size(pixels): " << parm.pixels_per_line << parm.lines
                        << "Bits/pixel: " <<  parm.depth * (SANE_FRAME_RGB == parm.format ? 3 : 1);
            } else {
                qInfo() << "Image's wide pixels: " << parm.pixels_per_line
                        << "Height for bits/pixel: " << parm.depth * (SANE_FRAME_RGB == parm.format ? 3 : 1);
            }

            switch (parm.format) {
            case SANE_FRAME_RED:
            case SANE_FRAME_GREEN:
            case SANE_FRAME_BLUE:
                assert (parm.depth == 8);
                must_buffer = 1;
                offset = parm.format - SANE_FRAME_RED;
                break;

            case SANE_FRAME_RGB:
                qInfo() << "SANE_FRAME_GRB";
            //assert ((parm.depth == 8) || (parm.depth == 16));

            case SANE_FRAME_GRAY:
                assert ((parm.depth == 1) || (parm.depth == 8) || (parm.depth == 16));
                if (parm.lines < 0) {
                    must_buffer = 1;
                    offset = 0;
                } else {
                    writePnmHeader (parm.format, parm.pixels_per_line, parm.lines, parm.depth, ofp);
                }
                break;
            default:
                break;
            }

            if (must_buffer) {
                /**
                 * We're either scanning a multi-frame image or the
                 * scanner doesn't know what the eventual image height
                 * will be (common for hand-held scanners).  In either
                 * case, we need to buffer all data before we can write
                 * the image.
                 */
                image.width = parm.bytes_per_line;
                if (parm.lines >= 0) {
                    image.height = parm.lines - STRIP_HEIGHT + 1;
                } else {
                    image.height = 0;
                }

                image.x = image.width - 1;
                image.y = -1;
                if (!advance (&image)) {
                    status = SANE_STATUS_NO_MEM;
                    goto cleanup;
                }
            }
        } else {
            assert (parm.format >= SANE_FRAME_RED && parm.format <= SANE_FRAME_BLUE);
            offset = parm.format - SANE_FRAME_RED;
            image.x = image.y = 0;
        }

        hundred_percent = parm.bytes_per_line * parm.lines \
                          *((parm.format == SANE_FRAME_RGB || parm.format == SANE_FRAME_GRAY) ? 1 : 3);

        while (1) {
            double progr;
            status = sane_read (g_device, g_buf, g_BufSize, &len);
            total_bytes += (SANE_Word) len;
            progr = ((total_bytes * 100.) / (double) hundred_percent);
            if (progr > 100.)
                progr = 100.;

            if (status != SANE_STATUS_GOOD) {
                if (status != SANE_STATUS_EOF)
                    return status;
                break;
            }

            if (must_buffer) {
                qInfo() << "must_buffer = " << must_buffer;
                switch (parm.format) {
                case SANE_FRAME_RED:
                case SANE_FRAME_GREEN:
                case SANE_FRAME_BLUE:
                    for (i = 0; i < len; ++i) {
                        image.data[offset + 3 * i] = g_buf[i];
                        if (!advance (&image)) {
                            status = SANE_STATUS_NO_MEM;
                            goto cleanup;
                        }
                    }
                    offset += 3 * len;
                    break;
                case SANE_FRAME_RGB:
                    for (i = 0; i < len; ++i) {
                        image.data[offset + i] = g_buf[i];
                        if (!advance (&image)) {
                            status = SANE_STATUS_NO_MEM;
                            goto cleanup;
                        }
                    }
                    offset += len;
                    break;
                case SANE_FRAME_GRAY:
                    for (i = 0; i < len; ++i) {
                        image.data[offset + i] = g_buf[i];
                        if (!advance (&image)) {
                            status = SANE_STATUS_NO_MEM;
                            goto cleanup;
                        }
                    }
                    offset += len;
                    break;
                default:
                    break;
                }
            } else { /* ! must_buffer */
                if ((parm.depth != 16)) {
                    fwrite (g_buf, 1, len, ofp);
                } else {
#if !defined(WORDS_BIGENDIAN)
                    int start = 0;
                    /* check if we have saved one byte from the last sane_read */
                    if (hang_over > -1) {
                        if (len > 0) {
                            fwrite (g_buf, 1, 1, ofp);
                            g_buf[0] = (SANE_Byte) hang_over;
                            hang_over = -1;
                            start = 1;
                        }
                    }
                    /* now do the byte-swapping */
                    for (i = start; i < (len - 1); i += 2) {
                        unsigned char LSB;
                        LSB = g_buf[i];
                        g_buf[i] = g_buf[i + 1];
                        g_buf[i + 1] = LSB;
                    }
                    /* check if we have an odd number of bytes */
                    if (((len - start) % 2) != 0) {
                        hang_over = g_buf[len - 1];
                        len--;
                    }
#endif
                    fwrite (g_buf, 1, len, ofp);
                }
            }

            if (g_verbose && parm.depth == 8) {
                for (i = 0; i < len; ++i)
                    if (g_buf[i] >= max) {
                        max = g_buf[i];
                    } else if (g_buf[i] < min) {
                        min = g_buf[i];
                    }
            }
        }
        first_frame = 0;
    } while (!parm.last_frame);

    if (must_buffer) {
        image.height = image.y;
        writePnmHeader (parm.format, parm.pixels_per_line, image.height, parm.depth, ofp);

#if !defined(WORDS_BIGENDIAN)
        if (parm.depth == 16) {
            for (i = 0; i < image.height * image.width; i += 2) {
                unsigned char LSB;
                LSB = image.data[i];
                image.data[i] = image.data[i + 1];
                image.data[i + 1] = LSB;
            }
        }
#endif
        fwrite (image.data, 1, image.height * image.width, ofp);
    }

    fflush( ofp );

cleanup:
    if (image.data)
        free (image.data);

    return status;
}


SANE_Status getSaneParameters(SANE_Handle device)
{
    SANE_Status status = SANE_STATUS_INVAL;
    SANE_Parameters parm;

    status = sane_get_parameters (device, &parm);
    qInfo() << "Parm : status = " << sane_strstatus(status)
            << "format = " << parm.format
            << "last_frame = " << parm.last_frame
            << "bytes_per_line = " << parm.bytes_per_line
            << "pixels_per_line = " << parm.pixels_per_line
            << "lines = " << parm.lines
            << "depth = " << parm.depth;

    return status;
}

SANE_Status doScan(const char *fileName)
{
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();

    SANE_Status status = SANE_STATUS_GOOD;
    FILE *ofp = nullptr;
    char path[PATH_MAX] = {0};
    char part_path[PATH_MAX] = {0};
    g_BufSize = (32 * 1024);
    g_buf = static_cast<SANE_Byte *>(malloc(g_BufSize));

    string dir = "/tmp/scanner/";
    if (access(dir.c_str(), 0) == -1) {
        qInfo() << " /tmp/scanner/ is not existing, now create it.";
        int flag = mkdir(dir.c_str(), 0777);
        if (flag == 0) {
            qInfo() << "create successfully!" ;
        } else {
            qInfo() << "create failed!" ;
        }
    }

    do {
        sprintf (path, "%s%s.pnm", dir.c_str(), fileName);
        strncpy (part_path, path, sizeof(part_path) - 1);
        strcat (part_path, ".part");
        qInfo() << "part_path = " << part_path;

        status = sane_start (g_device);
        qInfo() << "`sane_start` status: " << sane_strstatus(status);
        if (status != SANE_STATUS_GOOD) {
            qInfo() << "Cannot start scan devices, sane_status = " << status;
            instance.setKylinSaneStatus(false);
            break;
        }

        if (nullptr == (ofp = fopen (part_path, "w"))) {
            status = SANE_STATUS_ACCESS_DENIED;
            break;
        }

        status = onScanning(ofp);

        switch (status) {
        case SANE_STATUS_GOOD:
        case SANE_STATUS_EOF: {
            status = SANE_STATUS_GOOD;
            if (!ofp || (0 != fclose(ofp))) {
                status = SANE_STATUS_ACCESS_DENIED;
                break;
            } else {
                ofp = nullptr;
                if (rename (part_path, path)) {
                    status = SANE_STATUS_ACCESS_DENIED;
                    break;
                }
            }
        }
        break;
        default:
            break;
        }
    } while (0);

#if 1
    if (SANE_STATUS_GOOD != status)
        sane_cancel (g_device);
#endif

    //sane_cancel (gs_device);
    if (ofp) {
        fclose (ofp);
        ofp = nullptr;
    }

    if (g_buf) {
        free (g_buf);
        g_buf = nullptr;
    }

    return status;
}

static void authCallback (SANE_String_Const resource, SANE_Char *username, SANE_Char *password)
{
    qInfo() << "auth_callback" << resource << username << password;
}

/**
 * @brief saneInit 扫描设备初始化
 */
static void saneInit()
{
    SANE_Int version_code = 0;

    sane_init (&version_code, authCallback);

    qInfo() << "version_code = " << version_code;
}

/**
 * @brief getSaneDevices 查询所有连接设备
 * 注意： 这里会很耗时
 *
 * @param device_list 设备链表
 * @return 查询可连接设备结果
 */
static SANE_Status getSaneDevices(const SANE_Device ***device_list)
{
    SANE_Status status = SANE_STATUS_GOOD;

    qInfo() << "Get all scan devices, please waiting ...";

    status = sane_get_devices (device_list, SANE_FALSE);
    qInfo() << "status = " << status;

    if (status)
        qInfo() << "status = " << sane_strstatus(status);

    return status;
}

/**
 * @brief openSaneDevice 使用设备名字打开设备
 * @param device 设备结构体字段
 * @param sane_handle 扫描句柄
 * @return 返回打开设备结果
 */
SANE_Status openSaneDevice(SANE_Device *device, SANE_Handle *sane_handle)
{
    SANE_Status status = SANE_STATUS_INVAL;
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();

    qInfo() << "name = " << device->name
            << "model = " << device->model
            << "vendor = " << device->vendor
            << "type = " << device->type;

    // just for one scan device
    char name[512] = {0};
    snprintf(name, 512, "%s %s", device->vendor, device->model);
    QString openname = QString(QLatin1String(name));
    qInfo() << "device name:  " << openname;
    instance.setKylinSaneOpenName(openname);

    status = sane_open(device->name, sane_handle);
    if (status)
        qInfo() << "status = " << sane_strstatus(status);

    return status;
}

/**
 * @brief get_option_colors 获取色彩选项
 * optnum = 2
 * 可以输出扫描设备支持的所有色彩值：
 * Color => 彩色  Gray => 灰度  Lineart => 黑白
 *
 * @param sane_handle 扫描句柄
 * @param optnum 扫描选项值
 * @return 获取色彩选项返回值
 */
SANE_Status getOptionColors(SANE_Handle sane_handle, int optnum)
{
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    QStringList colors;

    const SANE_Option_Descriptor *opt;
    SANE_Status status = SANE_STATUS_INVAL;

    qInfo() << "Get colors option = " << optnum;

    opt = sane_get_option_descriptor(sane_handle, optnum);

    for (int i = 0; opt->constraint.string_list[i] != nullptr; ++i) {
        const char *tmp = *(opt->constraint.string_list + i);
        status = SANE_STATUS_GOOD;
        qInfo() << "color strings = " << *(opt->constraint.string_list + i);

        if (!strcmp("Color", tmp))
            colors << QObject::tr("Color");
        if (!strcmp("Gray", tmp))
            colors << QObject::tr("Gray");
        if (!strcmp("Lineart", tmp))
            colors << QObject::tr("Lineart");
    }

    instance.setKylinSaneColors(colors);
    return status;
}

/**
 * @brief set_option_colors 设置扫描设备颜色
 * 通过传入参数val_color进行设置扫描设备的颜色
 *
 * @param sane_handle 扫描句柄
 * @param val_color 需要设置的颜色字段，Lineart, Gray, Color等
 * @return 返回设置颜色的情况
 */
SANE_Status setOptionColors(SANE_Handle sane_handle, SANE_String val_color)
{
    SANE_Status status = SANE_STATUS_INVAL;

    qInfo() << "option: " << g_optDesc.numColorMode
            << "Set color option = " << val_color;
#if 0
    int i = 0;
    const SANE_Option_Descriptor *opt;


    qDebug() << "handle: " << sane_handle;
    opt = sane_get_option_descriptor(sane_handle, g_optDesc.numColorMode);
    for (int i = 0; opt->constraint.string_list[i] != nullptr; ++i) {
        const char *tmp = *(opt->constraint.string_list + i);
        status = SANE_STATUS_GOOD;
        qInfo() << "color strings = " << *(opt->constraint.string_list + i);
    }
    qDebug() << "1111";
#endif

    status = sane_control_option(sane_handle,  static_cast<SANE_Int>(g_optDesc.numColorMode),
                                 SANE_ACTION_SET_VALUE, val_color, nullptr);

    if (status != SANE_STATUS_GOOD) {
        qInfo() << "status = " << status << "desc: " << sane_strstatus(status);
        return status;
    }

    qInfo() << "Set color success.";

    return status;
}

/**
 * @brief get_option_sources 获取扫描设备的资源情况
 * optnum = 3
 * 可以获取扫描设备的设备分类， 平板式(Flatbed)，其他为馈纸式
 *
 * @param sane_handle 扫描句柄
 * @param optnum 扫描选项参数，资源种类为3
 * @return 返回资源获取情况
 */
SANE_Status getOptionSources(SANE_Handle sane_handle, int optnum)
{
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    QString type;

    SANE_Status status = SANE_STATUS_INVAL; // 没有读取到设备类型
    const SANE_Option_Descriptor *opt;
    int flag = 0;

    qInfo() << "Get source option = " << optnum;

    opt = sane_get_option_descriptor(sane_handle, optnum);

    for (int i = 0; opt->constraint.string_list[i] != nullptr; ++i) {
        const char *tmp = *(opt->constraint.string_list + i);
        status = SANE_STATUS_GOOD;

        qInfo() << "source string = " << tmp;
        if (!strcmp("Flatbed", tmp)
                || strstr(tmp, "flatbed")
                || strstr(tmp, "multi-function peripheral")) {
            type = QObject::tr("Flatbed"); //平板式
            flag = 1;
        }
    }
    if (flag == 0)
        type = QObject::tr("Transparency Adapter"); //馈纸式

    instance.setKylinSaneType(type);
    return status;
}

/**
 * @brief set_option_sources 设置扫描资源类型
 * 通过页面传的val_source可以设置扫描设备的种类
 *
 * @param sane_handle 扫描句柄
 * @param optnum 扫描选项，此处为3
 * @param val_source 需要设置的扫描设备种类
 * @return 扫描设备设置返回情况
 */
SANE_Status setOptionSources(SANE_Handle sane_handle, int optnum, SANE_String val_source)
{
    SANE_Status status = SANE_STATUS_GOOD;

    qInfo() << "Set source option = " << val_source;

    status = sane_control_option(sane_handle, optnum,
                                 SANE_ACTION_SET_VALUE, val_source, nullptr);
    if (status != SANE_STATUS_GOOD) {
        qInfo() << "status = " << status << "desc: " << sane_strstatus(status);
        return status;
    }

    return status;
}

/**
  * @brief getOptionResolutions获取扫描软件的分辨率
  * optnum = 3 | 6
  * 可以获取扫描设备支持的所有分辨率，如下：
  * 2400, 1200, 600, 300, 150, 100, 75
  *
  * @param sane_handle 扫描句柄
  * @param optnum 分辨率扫描选项，此处为6
  * @return 返回扫描软件分辨率获取情况
  */
static SANE_Status getOptionResolutions(SANE_Handle sane_handle, int optnum)
{
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    QStringList resolutions;
    QStringList reverseResolutions;
    SANE_Status status = SANE_STATUS_INVAL;
    const SANE_Option_Descriptor *opt;

    qInfo() << "Get resolution option = " << optnum;

    opt = sane_get_option_descriptor(sane_handle, optnum);

    for (int i = 0; opt->constraint.word_list[i]; ++i) {
        int res = *(opt->constraint.word_list + i);
        status = SANE_STATUS_GOOD;
        qInfo() << "resolution int = " << res;

        switch (res) {
        case 4800:
            resolutions << "4800";
            break;
        case 2400:
            resolutions << "2400";
            break;
        case 1200:
            resolutions << "1200";
            break;
        case 600:
            resolutions << "600";
            break;
        case 300:
            resolutions << "300";
            break;
        case 200:
            resolutions << "200"; // Scanner support: 200 dpi
            break;
        case 150:
            resolutions << "150";
            break;
        case 100:
            resolutions << "100";
            break;
        case 75:
            resolutions << "75";
            break;
        default:
            break;
        }
    }

    // 进行反向排序，有利于用户选择较低分辨率
    qSort(resolutions.begin(), resolutions.end(), [](const QString & s1, const QString & s2) {
        return s1.toInt() < s2.toInt();
    });
    resolutions << QObject::tr("Auto");

    instance.setKylinSaneResolutions(resolutions);

    return status;
}

/**
* @brief set_option_resolutions 设置扫描设备的分辨率
* 根据传入参数val_resolution进行设置扫描软件的分辨率
*
* @param sane_handle 扫描句柄
* @param val_resolution 需要设置的扫描设备分辨率
* @return 返回扫描设备设置分辨率情况
*/
SANE_Status setOptionResolutions(SANE_Handle sane_handle, SANE_Int val_resolution)
{
    SANE_Status status = SANE_STATUS_GOOD;

    qInfo() << "Set resolution option = " << val_resolution;

    status = sane_control_option(sane_handle, g_optDesc.numResolution,
                                 SANE_ACTION_SET_VALUE, &val_resolution, nullptr);
    if (status != SANE_STATUS_GOOD) {
        qInfo() << "status = " << status << "desc: " << sane_strstatus(status);
        return status;
    }

    return status;
}


/**
 * @brief get_option_sizes 获取扫描设备尺寸
 * 获取扫描尺寸根据不同的扫描选项值进行获取左上和右下坐标，从而确定扫描尺寸
 * Option 8(Top-left x),9(Top-left y),10(Bottom-right x),11(Bottom-right y)
 * get all sizes[8,9,10,11] option
 * print all size: tl-x tl-y br-x br-y (etc.)
 * A3: 0 0 297 420
 * A4: 0 0 210 297 (canon 210=> 0 0 216 297)
 * Because sizes just can read normal, so we could directly to set general sizes
 *
 * @param sane_handle 扫描句柄
 * @param optnum 尺寸扫描选项参数
 * 8 => 扫描设备左上角中的x坐标tl-x
 * 9 => 扫描设备左上角中的y坐标tl-y
 * 10 => 扫描设备右下角中的x坐标br-x
 * 11 => 扫描设备右下角中的y坐标br-y
 *
 * @return 获取设备尺寸结果
 */
SANE_Status getOptionSizes(SANE_Handle sane_handle, int optnum)
{
    const SANE_Option_Descriptor *opt;
    SANE_Status status = SANE_STATUS_GOOD;

    qInfo() << "Get size option = " << optnum;

    opt = sane_get_option_descriptor(sane_handle, optnum);

    for (int i = 0; opt->constraint.word_list[i]; ++i) {
        qInfo() << "int sizes = " << *(opt->constraint.word_list + i);
    }
    return status;
}

/**
 * @brief set_option_sizes 设置扫描设备的尺寸，也就是坐标
 * 根据扫描选项参数optnum进行判断设置的坐标值是x或者y，val_size为设置的坐标值
 *
 * @param sane_handle 扫描句柄
 * @param optnum 扫描选项参数
 * 10 => 右下角坐标的x  11 => 右下角坐标的y
 *
 * @param val_size 将要设置的扫描尺寸坐标值
 * @return 设置扫描尺寸的返回情况
 */
SANE_Status setOptionSizes(SANE_Handle sane_handle, int optnum, SANE_Int val_size)
{
    SANE_Status status = SANE_STATUS_GOOD;

    qInfo() << "Set size option = " << val_size;

    status = sane_control_option(sane_handle, optnum,
                                 SANE_ACTION_SET_VALUE, &val_size, nullptr);
    if (status != SANE_STATUS_GOOD) {
        qInfo() << "status = " << status << "desc: " << sane_strstatus(status);
        return status;
    }

    return status;
}

/**
 * @brief set_option_sizes_real 统一设置扫描设备尺寸
 * 这里可以根据传入参数val_size_br_x和val_size_br_y直接进行设置扫描设备的尺寸
 *
 * @param sane_handle 扫描句柄
 * @param val_size_br_x 扫描设备右下角的x坐标
 * @param val_size_br_y 扫描设备右下角的y坐标
 * @return 返回扫描设备设置尺寸的情况
 */
SANE_Status setOptionSizesReal(SANE_Handle sane_handle, SANE_Int val_size_br_x,
                                  SANE_Int val_size_br_y)
{
    SANE_Status status = SANE_STATUS_GOOD;
    qInfo() << "Size bottom-right location(xy) = " << val_size_br_x << val_size_br_y;

    status = setOptionSizes(sane_handle, g_optDesc.numSizeBrX, SANE_FIX(val_size_br_x));
    if (status != SANE_STATUS_GOOD) {
        qInfo() << "status = " << sane_strstatus(status);
        return status;
    }

    status = setOptionSizes(sane_handle, g_optDesc.numSizeBrY, SANE_FIX(val_size_br_y));
    if (status != SANE_STATUS_GOOD) {
        qInfo() << "status = " << sane_strstatus(status);
        return status;
    }

    return status;
}

/**
 * @brief set_option_sizes_all 根据不同的页面尺寸进行设置扫描设备尺寸
 * 根据选择的不同的页面尺寸进行对应设置扫描设备尺寸
 *
 * @param sane_handle 扫描句柄
 * @param type 不同的页面尺寸
 * @return 设置扫描尺寸返回情况
 */
SANE_Status setOptionSizesAll(SANE_Handle sane_handle, int type)
{
    SANE_Status status = SANE_STATUS_GOOD;

    switch (type) {
    case A2:
        status = setOptionSizesReal(sane_handle, 420, 594);
        break;
    case A3:
        status = setOptionSizesReal(sane_handle, 297, 420);
        break;
    case A4:
        status = setOptionSizesReal(sane_handle, 210, g_saneSizeA4BrY);
        break;
    case A5:
        status = setOptionSizesReal(sane_handle, 148, 210);
        break;
    case A6:
        status = setOptionSizesReal(sane_handle, 105, 144);
        break;
    default:
        status = SANE_STATUS_UNSUPPORTED;
    }

    return status;
}

/*--------------------------------------------------------------------------*/

#define GUARDS_SIZE 4 /* 4 bytes */
#define GUARD1 ((SANE_Word)0x5abf8ea5)
#define GUARD2 ((SANE_Word)0xa58ebf5a)

/* Allocate the requested memory plus enough room to store some guard bytes. */
static void *guardsMalloc(size_t size)
{
    unsigned char *ptr;

    size += 2 * GUARDS_SIZE;
    ptr = static_cast<unsigned char *>(malloc(size));

    assert(ptr);

    ptr += GUARDS_SIZE;

    return (ptr);
}

/* Free some memory allocated by guards_malloc. */
static void guardsFree(void *ptr)
{
    unsigned char *p = static_cast<unsigned char *>(ptr);

    p -= GUARDS_SIZE;
    free(p);
}

/**
 * @brief get_optdesc_by_name 根据扫描参数名称获取选项描述参数句柄
 * @param device 扫描句柄
 * @param name 扫描名称
 * @param option_num 扫描参数句柄值
 * @return 扫描选项描述句柄
 */
static const SANE_Option_Descriptor *getOptdescByName(SANE_Handle device, const char *name,
                                                         int *option_num)
{
    SANE_Int num_dev_options;
    SANE_Status status;

    /* Get the number of options. */
    status = sane_control_option (device, 0,
                                  SANE_ACTION_GET_VALUE, &num_dev_options, nullptr);
    qInfo() << "Get option name = " << name << "status = " << status;

    for (*option_num = 0; *option_num < num_dev_options; (*option_num)++) {
        const SANE_Option_Descriptor *opt;

        /* Get the option descriptor */
        opt = sane_get_option_descriptor (device, *option_num);

        if (opt->name && strcmp(opt->name, name) == 0) {
            qInfo() << "Get option desc for " << *option_num << "opt->name = " << opt->name << "name" << name;
            return (opt);
        }
    }
    return (nullptr);
}

void displayOptionValue(SANE_Handle device, int optnum)
{
    const SANE_Option_Descriptor *opt;

    opt = sane_get_option_descriptor(device, optnum);

    qInfo() << "Get options: optnum = " << optnum
            << "name = " << opt->name
            << "title = " << opt->title
            << "type = " << opt->type
            << "desc = " << opt->desc
            << "cap = " << opt->cap
            << "size = " << opt->size
            << "unit = " << opt->unit;
}


/**
 * @brief getOptionValue 统一获取扫描参数
 * @param device 设备句柄
 * @param option_name 获取的扫描参数选项
 * @return 返回获取的扫描参数
 */
static SANE_Status getOptionValue(SANE_Handle device, const char *option_name)
{
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    QStringList sizes;

    SANE_Status status = SANE_STATUS_GOOD;

    const SANE_Option_Descriptor *opt;
    int optnum;
    static char str[100];

    SANE_String val_string_source; // 来源
    SANE_String val_string_color; // 颜色
    SANE_Word val_size; // 尺寸
    SANE_Word val_resolution; // 分辨率

    opt = getOptdescByName(device, option_name, &optnum);
    qInfo() << "optname = " << option_name << "optnum = " << optnum;

    if (opt) {
        void *optval; //扫描选项值
        optval = guardsMalloc(opt->size);
        /* Get default optval(different format) */
        status = sane_control_option (device, optnum,
                                      SANE_ACTION_GET_VALUE, optval, nullptr);

        if (opt->desc)
            qInfo() << opt->desc;

        qInfo() << "opt->type = " << opt->type;
        switch (opt->type) {
        case SANE_TYPE_INT:
            qInfo() << "type = int" << "size = " << opt->size;

            if (!strcmp(option_name, SANE_NAME_SCAN_RESOLUTION)) {
                val_resolution = *(SANE_Word *)optval;

                g_optDesc.numResolution = optnum;
                if (opt->constraint_type == SANE_CONSTRAINT_WORD_LIST)
                    status = getOptionResolutions(device, optnum);

                qInfo() << "optnum = " << g_optDesc.numResolution
                        << "resolution = " << val_resolution
                        << "constraint_type = " << opt->constraint_type;
            }
            break;
        case SANE_TYPE_BOOL:
            if (*(SANE_Word *) optval == SANE_FALSE) {
                strcpy(str, "FALSE");
            } else {
                strcpy(str, "TRUE");
            }
            qInfo() << "type = bool" << "size = " << opt->size << str;
            break;
        case SANE_TYPE_FIXED:
            qInfo() << "type = fixed" << "size = " << opt->size;

            val_size = SANE_UNFIX(*(SANE_Word *) optval);
            if (opt->constraint_type == SANE_CONSTRAINT_RANGE)
                status = getOptionSizes(device, optnum);

            qInfo() << "str_status = " << sane_strstatus(status);

            if (!strcmp(option_name, SANE_NAME_SCAN_TL_X)) {
                qInfo() << "size tl_x = " << val_size << "constraint_type = " << opt->constraint_type;
            } else if (!strcmp(option_name, SANE_NAME_SCAN_TL_Y)) {
                qInfo() << "size tl_y = " << val_size << "constraint_type = " << opt->constraint_type;
            } else if (!strcmp(option_name, SANE_NAME_SCAN_BR_X)) {
                g_optDesc.numSizeBrX = optnum;
                // Via br_x to decide scan sizes
                int size_range = static_cast<int>( SANE_UNFIX(opt->constraint.range->max) \
                                                   - SANE_UNFIX(opt->constraint.range->min));
                qInfo() << "min = " << SANE_UNFIX(opt->constraint.range->min)
                        << "max = " << SANE_UNFIX(opt->constraint.range->max)
                        << "size_range = " << size_range;
                {
                    if (val_size >= 420)
                        sizes << "A2";
                    if (size_range >= 297)
                        sizes << "A3";
                    if (size_range >= 210)
                        sizes << "A4";
                    if (size_range >= 148)
                        sizes << "A5";
                    if (size_range >= 105)
                        sizes << "A6";
                }
                instance.setKylinSaneSizes(sizes);
                qInfo() << "size optnum = " << g_optDesc.numSizeBrX
                        << "br_x" << val_size
                        << "constraint_type = " << opt->constraint_type;
            } else if (!strcmp(option_name, SANE_NAME_SCAN_BR_Y)) {
                g_optDesc.numSizeBrY = optnum;
                // Need via br_y to decide scan sizes after br_x, in case of (215.9, 296.926) to A4, which should be A5
                int size_range = static_cast<int>( SANE_UNFIX(opt->constraint.range->max) \
                                                   - SANE_UNFIX(opt->constraint.range->min));
                double min = SANE_UNFIX(opt->constraint.range->min);
                double max = SANE_UNFIX(opt->constraint.range->max);
                qInfo() << "min = " << min
                        << "max = " << max
                        << "size_range = " << size_range;
                // Judge max(296.926) less than A4BRY(297)
                if ((qCeil(max) == A4BRY) && (qFloor(max) == A4BRY - 1)) {
                    qDebug() << "A4 br_y max = " << max;
                    g_saneSizeA4BrY = max;
                } else {
                    g_saneSizeA4BrY = A4BRY;
                }
                qInfo() << "size optnum = " << g_optDesc.numSizeBrY
                        << "br_y" << val_size
                        << "g_saneSizeA4Bry = " <<g_saneSizeA4BrY
                        << "constraint_type = " << opt->constraint_type;
            }

            break;
        case SANE_TYPE_STRING:
            qInfo() << "type = string" << "size = " << opt->size;

            if (!strcmp(option_name, SANE_NAME_SCAN_MODE)) {
                val_string_color = static_cast<SANE_String>(optval);
                g_optDesc.numColorMode = optnum;
                status = getOptionColors(device, optnum);
                qInfo() << "Default optnum = " << g_optDesc.numColorMode
                        << "color = " << val_string_color
                        << "constraint_type = " << opt->constraint_type;
            } else if (!strcmp(option_name, SANE_NAME_SCAN_SOURCE)) {
                val_string_source = static_cast<SANE_String>(optval);
                g_optDesc.numSource = optnum;
                status = getOptionSources(device, optnum);
                qInfo() << "Default optnum = " << g_optDesc.numSource
                        << "source = " << val_string_source
                        << "constraint_type = " << opt->constraint_type;
            } else {
                // Canon Lide 400 本地连接失败，参数非法
                status = SANE_STATUS_INVAL;
            }
            break;
        case SANE_TYPE_BUTTON:
            qInfo() << "type = button" << "size = " << opt->size;
            break;
        case SANE_TYPE_GROUP:
            qInfo() << "type = button" << "size = " << opt->size;
            break;
        default:
            qInfo() << "type = %d" << opt->type << "size = " << opt->size;
            break;
        }

        switch (opt->unit) {
        case SANE_UNIT_NONE:
            qInfo() << "unit = none";
            break;
        case SANE_UNIT_PIXEL:
            qInfo() << "unit = pixel";
            break;
        case SANE_UNIT_BIT:
            qInfo() << "unit = bit";
            break;
        case SANE_UNIT_MM:
            qInfo() << "unit = mm";
            break;
        case SANE_UNIT_DPI:
            qInfo() << "unit = dpi";
            break;
        case SANE_UNIT_PERCENT:
            qInfo() << "unit = percent";
            break;
        case SANE_UNIT_MICROSECOND:
            qInfo() << "unit = microsecond";
            break;
        default:
            qInfo() << "unit = " << opt->unit;
            break;
        }

        switch (opt->constraint_type) {
        case SANE_CONSTRAINT_RANGE:
            if (opt->type == SANE_TYPE_FIXED) {
                qInfo() << "min = " << SANE_UNFIX(opt->constraint.range->min) \
                        << "max = " << SANE_UNFIX(opt->constraint.range->max) \
                        << "quant = " << opt->constraint.range->quant;
            } else {
                qInfo() << "min = " << opt->constraint.range->min \
                        << "max = " << opt->constraint.range->max \
                        << "quant = " << SANE_UNFIX(opt->constraint.range->quant);
            }
            break;
        case SANE_CONSTRAINT_WORD_LIST:
            for (int i = 0; i < opt->constraint.word_list[0]; ++i) {
                if (opt->type == SANE_TYPE_INT) {
                    qInfo() << opt->constraint.word_list[i + 1];
                } else {
                    qInfo() << SANE_UNFIX(opt->constraint.word_list[i + 1]);
                }
            }
            break;
        case SANE_CONSTRAINT_STRING_LIST:
            for (int i = 0; opt->constraint.string_list[i] != NULL; ++i) {
                qInfo() << opt->constraint.string_list[i];
            }
            break;
        default:
            qInfo() << "case = %d not found" ;
            break;
        }

        guardsFree(optval);
    } else {
        /* The option does not exists. */
        strcpy(str, "backend default");
    }

    qInfo() << "option_name = " << option_name << "str = " << str;

    return status;
}

/**
 * @brief  kylinDisplayAllScanParms连接设备后，显示扫描参数
 * @param device 扫描句柄
 * @return 从扫描设备上获取的扫描设置默认信息状态
 */
static SANE_Status kylinDisplayAllScanParams(SANE_Handle device)
{
    static char str[150];
    char *p = str;

    SANE_Status  status = SANE_STATUS_GOOD;

    *p = 0;

    // Default source
    status = getOptionValue(device, SANE_NAME_SCAN_SOURCE);
    if (status != SANE_STATUS_GOOD) {
        status = SANE_STATUS_INVAL;
        qInfo() << "source parameters error!";
        return status;
    }

    // Default color mode
    status = getOptionValue(device, SANE_NAME_SCAN_MODE);
    if (status != SANE_STATUS_GOOD) {
        status = SANE_STATUS_INVAL;
        qInfo() << "color mode parameters error!";
        return status;
    }

    // Default resolution
    status = getOptionValue(device, SANE_NAME_SCAN_RESOLUTION);
    if (status != SANE_STATUS_GOOD) {
        status = SANE_STATUS_INVAL;
        qInfo() << "resolution parameters error!";
        return status;
    }

    // Default size coordination, top_left(x, y)
    getOptionValue(device, SANE_NAME_SCAN_TL_X);
    if (status != SANE_STATUS_GOOD) {
        status = SANE_STATUS_INVAL;
        qInfo() << "tl_x parameters error!";
        return status;
    }

    getOptionValue(device, SANE_NAME_SCAN_TL_Y);
    if (status != SANE_STATUS_GOOD) {
        status = SANE_STATUS_INVAL;
        qInfo() << "tl_y parameters error!";
        return status;
    }

    /* Default size coordination, botton_right(x, y)
     *
     * Refer to backend/sharp.c, for A4 size:
     * s->val[OPT_BR_X].w = SANE_FIX(210);
     * s->val[OPT_BR_Y].w = SANE_FIX(297);
     */
    getOptionValue(device, SANE_NAME_SCAN_BR_X);
    if (status != SANE_STATUS_GOOD) {
        status = SANE_STATUS_INVAL;
        qInfo() << "br_x parameters error!";
        return status;
    }

    status = getOptionValue(device, SANE_NAME_SCAN_BR_Y);
    if (status != SANE_STATUS_GOOD) {
        status = SANE_STATUS_INVAL;
        qInfo() << "br_y parameters error!";
        return status;
    }

    return status;
}

/**
 * @brief startSaneScan 开始扫描
 * @param sane_handle 扫描句柄
 * @param fileName 扫描文件名
 * @return 扫描结果
 */
static SANE_Status startSaneScan(SANE_Handle sane_handle, SANE_String_Const fileName)
{
    g_device = sane_handle; //全局静态变量初始化

    SANE_Status status = SANE_STATUS_GOOD;
    status = doScan(fileName);

    qInfo() << "doScan() status = " << status;

    return status;
}

/**
 * @brief sane_cancel 结束扫描
 * 用于结束当前扫描的sane_handle句柄，避免下一次扫描出现BUSY的情况
 * @param sane_handle 扫描句柄
 */
static void saneCancel(SANE_Handle sane_handle)
{
    qDebug() << "saneCancel()";
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();

    if (instance.getKylinSaneHaveHandle()) {
        qDebug() << "sane_cancel()";
        sane_cancel(sane_handle);

        qDebug() << "sane_close()";
        sane_close(sane_handle);
        instance.setKylinSaneHaveHandle(false);
    }
}

/**
 * @brief kylinNorScanFindDevice 查询扫描设备统一接口
 * 提供给页面调用
 */
void kylinNorScanFindDevice()
{
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    QStringList names;
    QString type;

    SANE_Status sane_status;
    char name[512] = {0};

    // 1. initialize SANE
    qInfo() << "sane init";
    saneInit();

    do {
        // 2. get all devices
        //const SANE_Device ** device_list = nullptr;
        sane_status = getSaneDevices((&g_deviceList));
        if (sane_status) {
            qInfo() << "Cannot get scan devices, sane_status = " << sane_status;
            instance.setKylinSaneStatus(false);
            break;
        }

        // display all devices
        int i = 0;
        unsigned int column = 80;

        for (i = 0; g_deviceList[i]; ++i) {
            if (column + strlen (g_deviceList[i]->name) + 1 >= 80)
                column = 4;
            if (column > 4)
                column += 1;

            column += strlen (g_deviceList[i]->name);
            qInfo() << "gs_deviceList->name" << g_deviceList[i]->name
                    << "column = " << column;
        }

        for (i = 0; g_deviceList[i]; ++i) {
            qInfo() << "deviceName = " << g_deviceList[i]->name
                    << "deviceVendor = " << g_deviceList[i]->vendor
                    << "deviceModel = " << g_deviceList[i]->model
                    << "deviceType = " << g_deviceList[i]->type;

            // just for one scan device
            snprintf(name, 512, "%s %s",
                     g_deviceList[i]->vendor, g_deviceList[i]->model);
            qInfo() << "device name:  " << name;

            names << name;

            //instance.setKylinSaneName(device_list[i]->name);
            if (i == 0) {
                // Same device have same type
                qInfo() << "i = " << i << "deviceType = " << g_deviceList[i]->type;
                if (!strcmp("Flatbed", g_deviceList[i]->type)
                        || strstr(g_deviceList[i]->type, "flatbed")
                        || strstr(g_deviceList[i]->type, "multi-function peripheral")) {
                    type = QObject::tr("Flatbed"); //平板式
                } else {
                    type = QObject::tr("Transparency Adapter"); //馈纸式
                }
                instance.setKylinSaneType(type);

            }
            //break;
        }

        // For devices name
        qInfo() << names;
        instance.setKylinSaneNames(names);

        if (!g_deviceList[0]) {
            qInfo() << "No scan devices !";
            sane_status = SANE_STATUS_UNSUPPORTED;
            instance.setKylinSaneStatus(false);
            break;
        }

        sane_status = SANE_STATUS_GOOD;

    } while (0);

    if (sane_status) {
        instance.setKylinSaneStatus(false);
        qInfo() << "find device set status false";
    } else {
        instance.setKylinSaneStatus(true);
    }
    //sane_cancel(instance.handle);
}

void kylinNorScanOpenDevice(int index)
{
    qDebug() << "begin open device";
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    QStringList names;
    QString type;

    SANE_Status sane_status;
    char name[512] = {0};

    do {
        /**
         * Deal with open same scanner device again meeting `SANE_STATUS_DEVICE_BUSY`,
         * Thus, should `sane_close` the open handle before next `sane_open`
         */
        if (instance.getKylinSaneHaveHandle()) {
            /// 每次打开扫描设备，都会验证是否已有sane_handle句柄，如果存在句柄则取消句柄，
            /// 让 openSaneDevice 可以重新获取sane_handle句柄，用于后续的扫描操作，可以避免BUSY状态
            qDebug() << "begin to sane_close()";;
            sane_close(instance.handle);
            instance.setKylinSaneHaveHandle(false);
        }
        instance.setKylinSaneHaveHandle(false);

        for (int i = 0; g_deviceList[i]; ++i) {
            qInfo() << "deviceName = " << g_deviceList[i]->name
                    << "deviceVendor = " << g_deviceList[i]->vendor
                    << "deviceModel = " << g_deviceList[i]->model
                    << "deviceType = " << g_deviceList[i]->type;

            // just for one scan device
            snprintf(name, 512, "%s %s",
                     g_deviceList[i]->vendor, g_deviceList[i]->model);
            qInfo() << "device name:  " << name;

            names << name;

            //instance.setKylinSaneName(device_list[i]->name);
            if (i == 0) {
                // Same device have same type
                qInfo() << "i = " << i << "deviceType = " << g_deviceList[i]->type;
                if (!strcmp("Flatbed", g_deviceList[i]->type)
                        || strstr(g_deviceList[i]->type, "flatbed")
                        || strstr(g_deviceList[i]->type, "multi-function peripheral")) {
                    type = QObject::tr("Flatbed"); //平板式
                } else {
                    type = QObject::tr("Transparency Adapter"); //馈纸式
                }
                instance.setKylinSaneType(type);
            }
        }

        // For devices name
        if (names.isEmpty()) {
            qInfo() << "No scan devices !";

        }
        qInfo() << names;
        instance.setKylinSaneNames(names);

        if (!g_deviceList[0]) {
            qInfo() << "No scan devices !";
            sane_status = SANE_STATUS_UNSUPPORTED;
            instance.setKylinSaneStatus(false);
            break;
        }

        // 3. open a device
        qInfo() << "Open a scan device, plese waiting ...";
        SANE_Handle sane_handle;
        g_saneDevice = const_cast<SANE_Device *>(*(g_deviceList + index));
        if (!g_saneDevice) {
            qInfo() << "No device connected!";
            sane_status = SANE_STATUS_UNSUPPORTED;
            instance.setKylinSaneStatus(false);
            break;
        }

        sane_status = openSaneDevice(g_saneDevice, &sane_handle);
        if (sane_status) {
            qInfo() << "First open a device failed!";
            instance.setKylinSaneStatus(false);

            // have_handle = false, 代表此时不可以sane_close
            instance.setKylinSaneHaveHandle(false);
            break;
        }
        instance.handle = sane_handle;
        instance.setKylinSaneHaveHandle(true);

        // 4. start scanning
        qInfo() << "Start scanning, please waiting ...";

        // 此处可以获取页面设置所需的扫描信息
        qDebug() << "================================================================================";
        sane_status = kylinDisplayAllScanParams(sane_handle);
        qDebug() << "================================================================================";
    } while (0);

    if (sane_status) {
        instance.setKylinSaneStatus(false);
        qInfo() << "open device set status false";
    } else {
        instance.setKylinSaneStatus(true);
    }
}
#ifdef __cplusplus
}
#endif

KYCSaneWidget::KYCSaneWidget(QWidget *parent) : QWidget(parent)
{
    devicesInfo.have_handle = false;
    devicesInfo.status = false;
    devicesInfo.name << "";
    devicesInfo.type = "";
    devicesInfo.color << "";
    devicesInfo.resolution << "";
    devicesInfo.size << "";
}

KYCSaneWidget::~KYCSaneWidget()
{

}

bool KYCSaneWidget::getKylinSaneStatus()
{
    return devicesInfo.status;
}

bool KYCSaneWidget::getKylinSaneHaveHandle()
{
    return devicesInfo.have_handle;
}

QStringList KYCSaneWidget::getKylinSaneNames()
{
    return devicesInfo.name;
}

QString KYCSaneWidget::getKylinSaneType()
{
    return devicesInfo.type;
}

QStringList KYCSaneWidget::getKylinSaneResolutions()
{
    return  devicesInfo.resolution;
}

QStringList KYCSaneWidget::getKylinSaneSizes()
{
    return devicesInfo.size;
}

QStringList KYCSaneWidget::getKylinSaneColors()
{
    return devicesInfo.color;
}

QString KYCSaneWidget::getKylinSaneOpenName()
{
    return openDeviceInfo.openName;
}

void KYCSaneWidget::setKylinSaneHaveHandle(bool have_handle)
{
    devicesInfo.have_handle = have_handle;
}

void KYCSaneWidget::setKylinSaneStatus(bool status)
{
    devicesInfo.status = status;
}

void KYCSaneWidget::setKylinSaneType(QString type)
{
    devicesInfo.type = type;
}

void KYCSaneWidget::setKylinSaneNames(QStringList name)
{
    devicesInfo.name = name;
}

void KYCSaneWidget::setKylinSaneResolutions(QStringList resolution)
{
    devicesInfo.resolution = resolution;
}

void KYCSaneWidget::setKylinSaneSizes(QStringList size)
{
    devicesInfo.size = size;
}

void KYCSaneWidget::setKylinSaneColors(QStringList color)
{
    devicesInfo.color = color;
}

void KYCSaneWidget::setKylinSaneOpenName(QString name)
{
    openDeviceInfo.openName = name;
}

/** free sane resource
 * Neglecting to call this function may result in some resources not being released properly.
 */
void KYCSaneWidget::saneExit()
{
    sane_exit();
}

/**
 * @brief KylinSane::findScanDevice 统一接口供页面调用，找到扫描设备
 * @return 返回找到扫描设备的扫描参数情况
 */
ScanDeviceInfo KYCSaneWidget::findScanDevice()
{
    const KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    kylinNorScanFindDevice();

    return instance.devicesInfo;
}

/**
 * @brief KylinSane::open_device 统一接口供页面调用，找到扫描设备
 * @return 返回找到扫描设备的扫描参数情况
 */
ScanDeviceInfo KYCSaneWidget::openScanDevice(int index)
{
    const KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    kylinNorScanOpenDevice(index);

    return instance.devicesInfo;
}

/**
 * @brief KylinSane::startScanning 统一接口供页面调用，开始扫描
 * 根据扫描设置页面情况info字段进行设置扫描设备参数，并进行扫描
 *
 * @param info 扫描设置页面传递的需要设置的扫描设备参数
 * @return 返回扫描情况
 */
int KYCSaneWidget::startScanning(UserSelectedInfo info)
{
    qInfo() << "startScanning";
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();

    int ret = 0;
    SANE_Status status = SANE_STATUS_GOOD;
    string strColor, strResolution, strSize;
    SANE_String s_color, s_resolution, s_size;

    strColor = info.color.toStdString();
    strResolution = info.resolution.toStdString();
    strSize = info.size.toStdString();

    int index = instance.userInfo.deviceNameIndex;

#if 1
    instance.openScanDevice(index);
    bool retStatus = instance.getKylinSaneStatus();
    if (retStatus) {
        qDebug() << "open_device true";
    } else {
        status = SANE_STATUS_COVER_OPEN;
        qDebug() << "open_device false";
        if (instance.getKylinSaneHaveHandle()) {
            saneCancel(instance.handle);
        }
        return status;
    }
#endif


    // For colors
    s_color = const_cast<SANE_String>(strColor.c_str());
    status = setOptionColors(instance.handle, s_color);
    if (status != SANE_STATUS_GOOD) {
        qInfo() << "Failed to set option color: " << s_color
                << "status = " << sane_strstatus(status);
    }

    // For resolutions
    s_resolution = const_cast<SANE_String>(strResolution.c_str());
    SANE_Int i_resolution;
    if (!strcmp("Auto", s_resolution) || !strcmp("自动", s_resolution)) {
        i_resolution = 300; //自动时设置为300
    } else {
        i_resolution = static_cast<SANE_Int>(atoi(s_resolution));
    }

    status = setOptionResolutions(instance.handle, i_resolution);
    if (status != SANE_STATUS_GOOD) {
        qInfo() << "Failed to set option resolution: " << s_resolution
                << "status = " << sane_strstatus(status);
    }

    // For sizes
    int type = 0;
    s_size = const_cast<SANE_String>(strSize.c_str());
    if (!strcmp("A2", s_size)) {
        type = A2;
    } else if (!strcmp("A3", s_size)) {
        type = A3;
    } else if (!strcmp("A4", s_size)) {
        type = A4;
    } else if (!strcmp("A5", s_size)) {
        type = A5;
    } else if (!strcmp("A6", s_size)) {
        type = A6;
    }
    status = setOptionSizesAll(instance.handle, type);
    if (status != SANE_STATUS_GOOD) {
        qInfo() << "Failed to set option size: " << s_size
                << "status = " << sane_strstatus(status);
    }

    qInfo() << "Start scanning, please waiting ...";
    ret = startSaneScan(instance.handle, "scan");

    if (instance.getKylinSaneHaveHandle()) {
        saneCancel(instance.handle);
    }

    return ret;
}

bool KYCSaneWidget::testScannerIsAlive(QString deviceName)
{
    qDebug() << "deviceName= " << deviceName;
    return true;
}
