/*
* Copyright (C) 2020, Tianjin KYLIN Information Technology Co., Ltd.
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
#include "kylin_sane.h"
//KylinSane* KylinSane::instance=nullptr; //静态成员需要先初始化


#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  uint8_t *data;
  int width;
  int height;
  int x;
  int y;
}
Image;

#define STRIP_HEIGHT	256
static SANE_Handle device = nullptr;
static int verbose;
static SANE_Byte *buffer;
static size_t buffer_size;

const SANE_Device ** device_list = nullptr;

static int opt_num_color_mode = 2;
static int opt_num_source = 3;
static int opt_num_resolution = 6;
static int opt_num_size_br_x = 10;
static int opt_num_size_br_y = 11;

#define SET_1_BIT(n,i) ((1<<(i))|(n))
#define SET_0_BIT(n,i) ((~(1<<(i)))&(n))
#define SET_R_BIT(n,i) ((n)^(1<<(i)))
#define GET_i_BIT(n,i) (((n)>>(i))&1)


static void write_pnm_header (SANE_Frame format, int width, int height, int depth, FILE *ofp)
{
    switch (format)
    {
        case SANE_FRAME_RED:
        case SANE_FRAME_GREEN:
        case SANE_FRAME_BLUE:
        case SANE_FRAME_RGB:
            fprintf (ofp, "P6\n# SANE data follows\n%d %d\n%d\n", width, height, (depth <= 8) ? 255 : 65535);
            break;
        default:
            if (depth == 1)
            {
                fprintf (ofp, "P4\n# SANE data follows\n%d %d\n", width, height);
            }
            else
            {
                fprintf (ofp, "P5\n# SANE data follows\n%d %d\n%d\n", width, height,(depth <= 8) ? 255 : 65535);
            }
            break;
    }
}

static void * advance (Image * image)
{
    if (++image->x >= image->width)
    {
      image->x = 0;
      if (++image->y >= image->height || !image->data)
      {
          size_t old_size = 0, new_size;

          if (image->data)
            old_size = static_cast<size_t>(image->height * image->width);

          image->height += STRIP_HEIGHT;
          new_size = static_cast<size_t>(image->height * image->width);

          if (image->data)
          {
            image->data = (uint8_t *) realloc (image->data, new_size);
          }
          else
          {
            image->data = (uint8_t *) malloc (new_size);
          }

          if (image->data)
            memset (image->data + old_size, 0, new_size - old_size);
      }
    }

    if (image->data == NULL)
    {
        fprintf (stderr, "can't allocate image buffer (%dx%d)\n", image->width, image->height);
    }
    return image->data;
}

static SANE_Status scan_it (FILE *ofp)
{
    int i, len, first_frame = 1, offset = 0, must_buffer = 0, hundred_percent;
    SANE_Byte min = 0xff, max = 0;
    SANE_Parameters parm;
    SANE_Status status;
    Image image = { nullptr, 0, 0, 0, 0 };
    SANE_Word total_bytes = 0;
    SANE_Int hang_over = -1;

    do
    {
        if (!first_frame)
        {
            status = sane_start (device);
            if (status != SANE_STATUS_GOOD)
            {
                goto cleanup;
            }
        }

        status = sane_get_parameters (device, &parm);
        qDebug("Parm : stat=%s form=%d,lf=%d,bpl=%d,pixpl=%d,lin=%d,dep=%d\n",
			sane_strstatus (status),
			parm.format, parm.last_frame,
			parm.bytes_per_line, parm.pixels_per_line,
			parm.lines, parm.depth);

        if (status != SANE_STATUS_GOOD)
        {
          goto cleanup;
        }

        if (first_frame)
        {
            if (parm.lines >= 0)
            {
                 qDebug("scanning image of size %dx%d pixels at %d bits/pixel\n",
                      parm.pixels_per_line, parm.lines,
                      parm.depth * (SANE_FRAME_RGB == parm.format ? 3 : 1));
            }
            else
            {
                 qDebug("scanning image %d pixels wide and "
                      "variable height at %d bits/pixel\n",
                      parm.pixels_per_line,
                      parm.depth * (SANE_FRAME_RGB == parm.format ? 3 : 1));
            }

            switch (parm.format)
            {
                case SANE_FRAME_RED:
                case SANE_FRAME_GREEN:
                case SANE_FRAME_BLUE:
                  assert (parm.depth == 8);
                  must_buffer = 1;
                  offset = parm.format - SANE_FRAME_RED;
                  break;

                case SANE_FRAME_RGB:
                  qDebug("SANE_FRAME_RGB\n");
                  assert ((parm.depth == 8) || (parm.depth == 16));

                case SANE_FRAME_GRAY:
                    assert ((parm.depth == 1) || (parm.depth == 8) || (parm.depth == 16));
                    if (parm.lines < 0)
                    {
                        qDebug("parm.format = SANE_FRAME_GRAY, parm.lines < 0\n");
                        must_buffer = 1;
                        offset = 0;
                    }
                    else
                    {
                        qDebug("SANE_FRAME_GRAY\n");
                        write_pnm_header (parm.format, parm.pixels_per_line, parm.lines, parm.depth, ofp);
                    }
                  break;
                default:
                  break;
            }

            if (must_buffer)
            {
                /**
                 * We're either scanning a multi-frame image or the
                 * scanner doesn't know what the eventual image height
                 * will be (common for hand-held scanners).  In either
                 * case, we need to buffer all data before we can write
                 * the image.
                 */
                image.width = parm.bytes_per_line;
                if (parm.lines >= 0)
                    image.height = parm.lines - STRIP_HEIGHT + 1;
                else
                    image.height = 0;

                image.x = image.width - 1;
                image.y = -1;
                if (!advance (&image))
                {
                    status = SANE_STATUS_NO_MEM;
                    goto cleanup;
                }
            }
        }
        else
        {
            assert (parm.format >= SANE_FRAME_RED && parm.format <= SANE_FRAME_BLUE);
            offset = parm.format - SANE_FRAME_RED;
            image.x = image.y = 0;
        }

        hundred_percent = parm.bytes_per_line * parm.lines * ((parm.format == SANE_FRAME_RGB || parm.format == SANE_FRAME_GRAY) ? 1:3);

        while (1)
        {
            double progr;
            status = sane_read (device, buffer, buffer_size, &len);
            total_bytes += (SANE_Word) len;
            progr = ((total_bytes * 100.) / (double) hundred_percent);
            if (progr > 100.)
                progr = 100.;

            if (status != SANE_STATUS_GOOD)
            {
                if (status != SANE_STATUS_EOF)
                {
                    return status;
                }
                break;
            }

            if (must_buffer)
            {
                qDebug("must_buffer = %d\n", must_buffer);
                switch (parm.format)
                {
                    case SANE_FRAME_RED:
                    case SANE_FRAME_GREEN:
                    case SANE_FRAME_BLUE:
                        for (i = 0; i < len; ++i)
                        {
                            image.data[offset + 3 * i] = buffer[i];
                            if (!advance (&image))
                            {
                                status = SANE_STATUS_NO_MEM;
                                goto cleanup;
                            }
                        }
                        offset += 3 * len;
                        break;
                    case SANE_FRAME_RGB:
                        for (i = 0; i < len; ++i)
                        {
                            image.data[offset + i] = buffer[i];
                            if (!advance (&image))
                            {
                                status = SANE_STATUS_NO_MEM;
                                goto cleanup;
                            }
                        }
                        offset += len;
                        break;
                    case SANE_FRAME_GRAY:
                        for (i = 0; i < len; ++i)
                        {
                            image.data[offset + i] = buffer[i];
                            if (!advance (&image))
                            {
                                status = SANE_STATUS_NO_MEM;
                                goto cleanup;
                            }
                        }
                        offset += len;
                        break;
                    default:
                        break;
                }
            }
            else			/* ! must_buffer */
            {
                if ((parm.depth != 16)) 
                    fwrite (buffer, 1, len, ofp);
                else
                {
#if !defined(WORDS_BIGENDIAN)
                    int i, start = 0;
                    /* check if we have saved one byte from the last sane_read */
                    if (hang_over > -1)
                    {
                        if (len > 0)
                        {
                            fwrite (buffer, 1, 1, ofp);
                            buffer[0] = (SANE_Byte) hang_over;
                            hang_over = -1;
                            start = 1;
                        }
                    }
                    /* now do the byte-swapping */
                    for (i = start; i < (len - 1); i += 2)
                    {
                        unsigned char LSB;
                        LSB = buffer[i];
                        buffer[i] = buffer[i + 1];
                        buffer[i + 1] = LSB;
                    }
                    /* check if we have an odd number of bytes */
                    if (((len - start) % 2) != 0)
                    {
                        hang_over = buffer[len - 1];
                        len--;
                    }
#endif
                    fwrite (buffer, 1, len, ofp);
                }
            }

            if (verbose && parm.depth == 8)
            {
              for (i = 0; i < len; ++i)
                if (buffer[i] >= max)
                    max = buffer[i];
                else if (buffer[i] < min)
                    min = buffer[i];
            }
        }
        first_frame = 0;
    }while (!parm.last_frame);

    if (must_buffer)
    {
        image.height = image.y;
        write_pnm_header (parm.format, parm.pixels_per_line,image.height, parm.depth, ofp);

#if !defined(WORDS_BIGENDIAN)
        if (parm.depth == 16)
        {
            int i;
            for (i = 0; i < image.height * image.width; i += 2)
            {
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


SANE_Status kylin_sane_get_parameters(SANE_Handle device)
{
    SANE_Status status = SANE_STATUS_INVAL;
    SANE_Parameters parm;

    status = sane_get_parameters (device, &parm);
    qDebug("Parm : stat=%s form=%d,lf=%d,bpl=%d,pixpl=%d,lin=%d,dep=%d\n",
        sane_strstatus (status),
        parm.format, parm.last_frame,
        parm.bytes_per_line, parm.pixels_per_line,
        parm.lines, parm.depth);

    return status;
}

SANE_Status do_scan(const char *fileName)
{
    SANE_Status status = SANE_STATUS_GOOD;
    FILE *ofp = nullptr;
	char path[PATH_MAX];
	char part_path[PATH_MAX];
	buffer_size = (32 * 1024);
    buffer = static_cast<SANE_Byte *>(malloc(buffer_size));

    string dir = "/tmp/scanner/";
    if (access(dir.c_str(), 0) == -1)
    {
        cout << dir << "is not existing, now create it." << endl;
        int flag=mkdir(dir.c_str(), 0777);
        if (flag == 0)
        {
            cout << "create successfully!" << endl;
        }
        else
        {
            cout << "create failed!" << endl;
        }
    }

	do
	{
        sprintf (path, "%s%s.pnm", dir.c_str(), fileName);
        strcpy (part_path, path);
        strcat (part_path, ".part");

        qDebug("picture name: %s\n", path);

		status = sane_start (device);
        qDebug() << "status error: " << sane_strstatus(status);
        if (status != SANE_STATUS_GOOD)
		{
			break;
		}

        if (nullptr == (ofp = fopen (part_path, "w")))
        {
            status = SANE_STATUS_ACCESS_DENIED;
            break;
        }

		status = scan_it (ofp);

		switch (status)
		{
			case SANE_STATUS_GOOD:
			case SANE_STATUS_EOF:
                 {
                      status = SANE_STATUS_GOOD;
                      if (!ofp || 0 != fclose(ofp))
                      {
                          status = SANE_STATUS_ACCESS_DENIED;
                          break;
                      }
                      else
                      {
                          ofp = nullptr;
                          if (rename (part_path, path))
                          {
                              status = SANE_STATUS_ACCESS_DENIED;
                              break;
                          }
                      }
                  }
				  break;
			default:
                  break;
		}
	}while (0);

    if (SANE_STATUS_GOOD != status)
    {
        sane_cancel (device);
    }
    if (ofp)
    {
        fclose (ofp);
        ofp = nullptr;
    }
    if (buffer)
    {
        free (buffer);
        buffer = nullptr;
    }

    return status;
}

static void auth_callback (SANE_String_Const resource, SANE_Char * username, SANE_Char * password)
{
    qDebug() << "auth_callback" << resource << username << password;
}

/**
 * @brief init 扫描设备初始化
 */
void init()
{
    SANE_Int version_code = 0;

	sane_init (&version_code, auth_callback);

    qDebug("SANE version code: %d\n", version_code);
}

/**
 * @brief get_devices 查询所有连接设备
 * 注意： 这里会很耗时
 *
 * @param device_list 设备链表
 * @return 查询可连接设备结果
 */
SANE_Status get_devices(const SANE_Device ***device_list)
{
    SANE_Status status = SANE_STATUS_GOOD;

    qDebug("Get all devices...\n");

    status = sane_get_devices (device_list, SANE_FALSE);

    if (status)
	{
        qDebug("sane_get_devices status: %s\n", sane_strstatus(status));
	}	
    return status;
}

/**
 * @brief open_sane_device 使用设备名字打开设备
 * @param device 设备结构体字段
 * @param sane_handle 扫描句柄
 * @return 返回打开设备结果
 */
SANE_Status open_sane_device(SANE_Device *device, SANE_Handle *sane_handle)
{
    SANE_Status status = SANE_STATUS_INVAL;

    qDebug("Name: %s, vendor: %s, model: %s, type: %s\n",
		 device->name, device->model, device->vendor, device->type);

    status = sane_open(device->name, sane_handle);
    if (status)
    {
        qDebug("sane_open status: %s\n", sane_strstatus(status));
    }

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
SANE_Status get_option_colors(SANE_Handle sane_handle, int optnum)
{
    KylinSane& instance = KylinSane::getInstance();
    QStringList colors;

    const SANE_Option_Descriptor *opt;
    SANE_Status status = SANE_STATUS_INVAL;

    qDebug("begin get option[%d] colors\n", optnum);

    opt = sane_get_option_descriptor(sane_handle, optnum);

    qDebug("begin print all colors:\n");
    for (int i=0; opt->constraint.string_list[i] != nullptr; i++)
	{
        const char *tmp = *(opt->constraint.string_list+i);
        status = SANE_STATUS_GOOD;
        qDebug("optnum[%d] colors string: %s \n", optnum, *(opt->constraint.string_list+i));

        if (!strcmp("Color", tmp))
        {
            colors << QObject::tr("Color");
        }
        if (!strcmp("Gray", tmp))
        {
            colors << QObject::tr("Gray");
        }
        if (!strcmp("Lineart", tmp))
        {
            colors << QObject::tr("Lineart");
        }
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
SANE_Status set_option_colors(SANE_Handle sane_handle, SANE_String val_color)
{
    SANE_Status status = SANE_STATUS_INVAL;

    qDebug("\nbegin set option[2] color: %s \n", val_color);

    status = sane_control_option(sane_handle, opt_num_color_mode, SANE_ACTION_SET_VALUE, val_color, nullptr);
    if (status != SANE_STATUS_GOOD)
	{
        qDebug("Option did not set\n");
        return status;
    }

    qDebug("set color option success!\n\n");
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
SANE_Status get_option_sources(SANE_Handle sane_handle, int optnum)
{
    KylinSane& instance = KylinSane::getInstance();
    QString type;

    SANE_Status status = SANE_STATUS_INVAL; // 没有读取到设备类型
    const SANE_Option_Descriptor *opt;
    int flag = 0;

    qDebug("begin get option[%d] sources\n", optnum);

    opt = sane_get_option_descriptor(sane_handle, optnum);

    qDebug("begin print all sources:\n");
    for (int i=0; opt->constraint.string_list[i] != nullptr; i++)
	{
        const char *tmp = *(opt->constraint.string_list+i);
        status = SANE_STATUS_GOOD;

        qDebug("optnum[%d] sources string: %s \n", optnum, tmp);
        if (!strcmp("Flatbed", tmp))
        {
            type = QObject::tr("Flatbed"); //平板式
            flag = 1;
        }
    }
    if (flag == 0)
    {
        type = QObject::tr("Transparency Adapter"); //馈纸式
    }

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
SANE_Status set_option_sources(SANE_Handle sane_handle, int optnum, SANE_String val_source)
{
    SANE_Status status = SANE_STATUS_GOOD;

    qDebug("begin set option[%d] source: %s \n", optnum, val_source);

    status = sane_control_option(sane_handle, optnum, SANE_ACTION_SET_VALUE, val_source, nullptr);
    if (status != SANE_STATUS_GOOD)
	{
        qDebug("Option did not set\n");
        return status;
    }

    qDebug("set source option success!\n\n");
    return status;
}

/**
  * @brief get_option_resolutions 获取扫描软件的分辨率
  * optnum = 3 | 6
  * 可以获取扫描设备支持的所有分辨率，如下：
  * 2400, 1200, 600, 300, 150, 100, 75
  *
  * @param sane_handle 扫描句柄
  * @param optnum 分辨率扫描选项，此处为6
  * @return 返回扫描软件分辨率获取情况
  */
SANE_Status get_option_resolutions(SANE_Handle sane_handle, int optnum)
{
    KylinSane& instance = KylinSane::getInstance();
    QStringList resolutions;
    SANE_Status status = SANE_STATUS_INVAL;
    const SANE_Option_Descriptor *opt;

    qDebug("begin get option[%d] resolution \n", optnum);

    opt = sane_get_option_descriptor(sane_handle, optnum);

    qDebug("begin print all resolutions:\n");
    for (int i=0; opt->constraint.word_list[i]; i++)
    {
        int res = *(opt->constraint.word_list+i);
        status = SANE_STATUS_GOOD;
        qDebug("optnum[%d] resolutions int: %d \n", optnum, res);

        switch (res)
        {
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
    resolutions << QObject::tr("auto");

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
SANE_Status set_option_resolutions(SANE_Handle sane_handle, SANE_Int val_resolution)
{
    SANE_Status status = SANE_STATUS_GOOD;

    qDebug("\nbegin set option 6 resolution: %d \n", val_resolution);

    status = sane_control_option(sane_handle, opt_num_resolution, SANE_ACTION_SET_VALUE, &val_resolution, nullptr);
    if (status != SANE_STATUS_GOOD)
	{
        qDebug("Option did not set\n");
        return status;
    }

    qDebug("set resolution option success!\n\n");
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
SANE_Status get_option_sizes(SANE_Handle sane_handle, int optnum)
{
    const SANE_Option_Descriptor *opt;
    SANE_Status status = SANE_STATUS_GOOD;
    int res = 0;

    qDebug("begin get option[%d] size \n", optnum);

    opt = sane_get_option_descriptor(sane_handle, optnum);

    qDebug("begin print all sizes:\n");
    for (int i=0; opt->constraint.word_list[i]; i++)
	{
        res = *(opt->constraint.word_list+i);
        qDebug("optnum[%d] sizes int: %d \n", optnum, res);
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
SANE_Status set_option_sizes(SANE_Handle sane_handle, int optnum, SANE_Int val_size)
{
    SANE_Status status = SANE_STATUS_GOOD;

    qDebug("\nbegin set option[%d] size: %d \n", optnum, val_size);

    status = sane_control_option(sane_handle, optnum, SANE_ACTION_SET_VALUE, &val_size, nullptr);
    if (status != SANE_STATUS_GOOD)
	{
        qDebug("Option did not set\n");
        return status;
    }

    qDebug("set size option success!\n\n");
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
SANE_Status set_option_sizes_real(SANE_Handle sane_handle, SANE_Int val_size_br_x, SANE_Int val_size_br_y)
{
    SANE_Status status = SANE_STATUS_GOOD;
    qDebug("size Bottom-right xy=[%d, %d]\n", val_size_br_x, val_size_br_y);

    status = set_option_sizes(sane_handle, opt_num_size_br_x, SANE_FIX(val_size_br_x));
    status = set_option_sizes(sane_handle, opt_num_size_br_y, SANE_FIX(val_size_br_y));

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
SANE_Status set_option_sizes_all(SANE_Handle sane_handle, int type)
{
    SANE_Status status = SANE_STATUS_GOOD;

    switch (type)
    {
        case A2:
            status = set_option_sizes_real(sane_handle, 420, 594);
            break;
        case A3:
            status = set_option_sizes_real(sane_handle, 297, 420);
            break;
        case A4:
            status = set_option_sizes_real(sane_handle, 210, 297);
            break;
        case A5:
            status = set_option_sizes_real(sane_handle, 148, 210);
            break;
        case A6:
            status = set_option_sizes_real(sane_handle, 105, 144);
            break;
        default:
            status = SANE_STATUS_UNSUPPORTED;
    }

    return status;
}

/*--------------------------------------------------------------------------*/

#define GUARDS_SIZE 4			/* 4 bytes */
#define GUARD1 ((SANE_Word)0x5abf8ea5)
#define GUARD2 ((SANE_Word)0xa58ebf5a)

/* Allocate the requested memory plus enough room to store some guard bytes. */
static void *guards_malloc(size_t size)
{
	unsigned char *ptr;

	size += 2*GUARDS_SIZE;
    ptr = static_cast<unsigned char *>(malloc(size));

	assert(ptr);

	ptr += GUARDS_SIZE;

	return(ptr);
}

/* Free some memory allocated by guards_malloc. */
static void guards_free(void *ptr)
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
static const SANE_Option_Descriptor *get_optdesc_by_name(SANE_Handle device, const char *name, int *option_num)
{
	const SANE_Option_Descriptor *opt;
	SANE_Int num_dev_options;
	SANE_Status status;	
	
	/* Get the number of options. */
    status = sane_control_option (device, 0, SANE_ACTION_GET_VALUE, &num_dev_options, nullptr);
    qDebug("get option %s value (%s)\n", name, sane_strstatus(status));

	for (*option_num = 0; *option_num < num_dev_options; (*option_num)++) {

		/* Get the option descriptor */
		opt = sane_get_option_descriptor (device, *option_num);
	
		if (opt->name && strcmp(opt->name, name) == 0) {
            qDebug("get option descriptor for option %d, opt->name=%s name=%s\n", *option_num, opt->name, name);
			return(opt);
		}
	}
    return(nullptr);
}

void display_option_value(SANE_Handle device, int optnum)
{
    const SANE_Option_Descriptor *opt;

    opt = sane_get_option_descriptor(device, optnum);

    qDebug("\n\nGet options %d:\n", optnum);
    qDebug("opt name: %s\n",opt->name);
    qDebug("opt title: %s\n",opt->title);
    qDebug("opt type: %d \n",opt->type);
    qDebug("opt description: %s\n",opt->desc);
    qDebug("opt cap: %d \n", opt->cap);
    qDebug("opt size: %d \n", opt->size);
    qDebug("opt unit: %d \n", opt->unit);
    qDebug("\n");
}


/**
 * @brief get_option_value 统一获取扫描参数
 * @param device 设备句柄
 * @param option_name 获取的扫描参数选项
 * @return 返回获取的扫描参数
 */
SANE_Status get_option_value(SANE_Handle device, const char *option_name)
{
    KylinSane& instance = KylinSane::getInstance();
    QStringList sizes;

    SANE_Status status = SANE_STATUS_GOOD;

    const SANE_Option_Descriptor *opt;
    void *optval; //扫描选项值
    int optnum;
    static char str[100];

    SANE_String val_string_source; // 来源
    SANE_String val_string_color; // 颜色
    SANE_Word val_size; // 尺寸
    SANE_Word val_resolution; // 分辨率

    opt = get_optdesc_by_name(device, option_name, &optnum);
    qDebug("optname = %s optnum = %d\n", option_name, optnum);

    if (opt)
    {
        optval = guards_malloc(opt->size);
        /* Get default optval(different format) */
        status = sane_control_option (device, optnum, SANE_ACTION_GET_VALUE, optval, nullptr);

        if (opt->desc != NULL)
        {
            qDebug() << opt->desc;
        }

        qDebug("opt->type = %d\n", opt->type);
        switch (opt->type)
        {
            case SANE_TYPE_INT:
                qDebug() << "type = int" << "size = " << opt->size;

                if (!strcmp(option_name, SANE_NAME_SCAN_RESOLUTION))
                {
                    val_resolution = *(SANE_Word*)optval;

                    opt_num_resolution = optnum;
                    if (opt->constraint_type == SANE_CONSTRAINT_WORD_LIST)
                    {
                        status = get_option_resolutions(device, optnum);
                    }
                    qDebug("optnum=%d resolution = %d constraint_type=%d\n",opt_num_resolution, val_resolution, opt->constraint_type);
                }
                break;
            case SANE_TYPE_BOOL:
                if (*(SANE_Word*) optval == SANE_FALSE)
                {
                    strcpy(str, "FALSE");
                }
                else
                {
                    strcpy(str, "TRUE");
                }
                qDebug() << "type = bool" << "size = " << opt->size << str;
                break;
            case SANE_TYPE_FIXED:
                qDebug() << "type = fixed" << "size = " << opt->size;

                val_size = SANE_UNFIX(*(SANE_Word*) optval);
                if (opt->constraint_type == SANE_CONSTRAINT_RANGE)
                {
                    status = get_option_sizes(device, optnum);
                }

                qDebug() << "str_status = " << sane_strstatus(status);

                if (!strcmp(option_name, SANE_NAME_SCAN_TL_X))
                {
                    qDebug("size tl_x = %d constraint_type=%d\n", val_size, opt->constraint_type);
                }
                else if (!strcmp(option_name, SANE_NAME_SCAN_TL_Y))
                {
                    qDebug("size tl_y= %d constraint_type=%d\n", val_size, opt->constraint_type);
                }
                else if (!strcmp(option_name, SANE_NAME_SCAN_BR_X))
                {
                    opt_num_size_br_x = optnum;
                    // Via br_x to decide scan sizes
                    int size_range = static_cast<int>( SANE_UNFIX(opt->constraint.range->max) - SANE_UNFIX(opt->constraint.range->min));
                    qDebug() << "min = " << SANE_UNFIX(opt->constraint.range->min) \
                             << "max = " << SANE_UNFIX(opt->constraint.range->max) \
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
                    qDebug("size optnum=%d br_x = %d constraint_type=%d\n",opt_num_size_br_x, val_size, opt->constraint_type);
                }
                else if (!strcmp(option_name, SANE_NAME_SCAN_BR_Y))
                {
                    opt_num_size_br_y = optnum;
                    qDebug("size opt_num=%d br_y = %d constraint_type=%d\n",opt_num_size_br_y, val_size, opt->constraint_type);
                }

                break;
            case SANE_TYPE_STRING:
                qDebug() << "type = string" << "size = " << opt->size;

                if (!strcmp(option_name, SANE_NAME_SCAN_MODE))
                {
                    val_string_color = static_cast<SANE_String>(optval);
                    opt_num_color_mode = optnum;
                    status = get_option_colors(device, optnum);
                    qDebug("Default optnum=%d color= %s constraint_type=%d\n",opt_num_color_mode, val_string_color, opt->constraint_type);
                }
                else if (!strcmp(option_name, SANE_NAME_SCAN_SOURCE))
                {
                    val_string_source = static_cast<SANE_String>(optval);
                    opt_num_source = optnum;
                    status = get_option_sources(device, optnum);
                    qDebug("Default optnum=%d source= %s constraint_type=%d\n",opt_num_source, val_string_source, opt->constraint_type);
                }
                else
                {
                    // Canon Lide 400 本地连接失败，参数非法
                    status = SANE_STATUS_INVAL;
                }
                break;
            case SANE_TYPE_BUTTON:
                qDebug() << "type = button" << "size = " << opt->size;
                break;
            case SANE_TYPE_GROUP:
                qDebug() << "type = button" << "size = " << opt->size;
                break;
            default:
                qDebug() << "type = %d" << opt->type << "size = " << opt->size;
                break;
        }

        switch (opt->unit)
        {
            case SANE_UNIT_NONE:
                qDebug() << "unit = none";
                break;
            case SANE_UNIT_PIXEL:
                qDebug() << "unit = pixel";
                break;
            case SANE_UNIT_BIT:
                qDebug() << "unit = bit";
                break;
            case SANE_UNIT_MM:
                qDebug() << "unit = mm";
                break;
            case SANE_UNIT_DPI:
                qDebug() << "unit = dpi";
                break;
            case SANE_UNIT_PERCENT:
                qDebug() << "unit = percent";
                break;
            case SANE_UNIT_MICROSECOND:
                qDebug() << "unit = microsecond";
                break;
            default:
                qDebug() << "unit = " << opt->unit;
                break;
        }

        switch (opt->constraint_type)
        {
            case SANE_CONSTRAINT_RANGE:
                if (opt->type == SANE_TYPE_FIXED)
                {
                    qDebug() << "min = " << SANE_UNFIX(opt->constraint.range->min) \
                             << "max = " << SANE_UNFIX(opt->constraint.range->max) \
                             << "quant = " << opt->constraint.range->quant;
                }
                else
                {
                    qDebug() << "min = " << opt->constraint.range->min \
                             << "max = " << opt->constraint.range->max \
                             << "quant = " << SANE_UNFIX(opt->constraint.range->quant);
                }
                break;
            case SANE_CONSTRAINT_WORD_LIST:
                for (int i=0; i<opt->constraint.word_list[0]; i++)
                {
                    if (opt->type == SANE_TYPE_INT)
                    {
                        qDebug() << opt->constraint.word_list[i+1];
                    }
                    else
                    {
                        qDebug() << SANE_UNFIX(opt->constraint.word_list[i+1]);
                    }
                }
                break;
            case SANE_CONSTRAINT_STRING_LIST:
                for (int i=0; opt->constraint.string_list[i] != NULL; i++)
                {
                    qDebug() << opt->constraint.string_list[i];
                }
                break;
            default:
                qDebug() << "case = %d not found" ;
                break;
        }

        if(opt->cap != 0)
        {
            //if (opt->cap & sane_cap)
        }

        guards_free(optval);
    }
    else
    {
        /* The option does not exists. */
        strcpy(str, "backend default");
    }

    qDebug() << "option_name = " << option_name << "str = " << str;

    return status;
}

/**
 * @brief kylin_display_scan_parameters 连接设备后，显示扫描参数
 * @param device 扫描句柄
 * @return 从扫描设备上获取的扫描设置默认信息状态
 */
SANE_Status kylin_display_scan_parameters(SANE_Handle device)
{
    static char str[150];
    char *p = str;

    SANE_Status  status = SANE_STATUS_GOOD;

    *p = 0;

    // Default source
    status = get_option_value(device, SANE_NAME_SCAN_SOURCE);
    if (status != SANE_STATUS_GOOD)
    {
        status = SANE_STATUS_INVAL;
        qDebug() << "source parameters error!";
        return status;
    }

    // Default color mode
    status = get_option_value(device, SANE_NAME_SCAN_MODE);
    if (status != SANE_STATUS_GOOD)
    {
        status = SANE_STATUS_INVAL;
        qDebug() << "color mode parameters error!";
        return status;
    }

    // Default resolution
    status = get_option_value(device, SANE_NAME_SCAN_RESOLUTION);
    if (status != SANE_STATUS_GOOD)
    {
        status = SANE_STATUS_INVAL;
        qDebug() << "resolution parameters error!";
        return status;
    }

    // Default size coordination, top_left(x, y)
    get_option_value(device, SANE_NAME_SCAN_TL_X);
    if (status != SANE_STATUS_GOOD)
    {
        status = SANE_STATUS_INVAL;
        qDebug() << "tl_x parameters error!";
        return status;
    }

    get_option_value(device, SANE_NAME_SCAN_TL_Y);
    if (status != SANE_STATUS_GOOD)
    {
        status = SANE_STATUS_INVAL;
        qDebug() << "tl_y parameters error!";
        return status;
    }

    /* Default size coordination, botton_right(x, y)
     *
     * Refer to backend/sharp.c, for A4 size:
     * s->val[OPT_BR_X].w = SANE_FIX(210);
     * s->val[OPT_BR_Y].w = SANE_FIX(297);
     */
    get_option_value(device, SANE_NAME_SCAN_BR_X);
    if (status != SANE_STATUS_GOOD)
    {
        status = SANE_STATUS_INVAL;
        qDebug() << "br_x parameters error!";
        return status;
    }

    status = get_option_value(device, SANE_NAME_SCAN_BR_Y);
    if (status != SANE_STATUS_GOOD)
    {
        status = SANE_STATUS_INVAL;
        qDebug() << "br_y parameters error!";
        return status;
    }

    return status;
}

/**
 * @brief start_scan 开始扫描
 * @param sane_handle 扫描句柄
 * @param fileName 扫描文件名
 * @return 扫描结果
 */
SANE_Status start_scan(SANE_Handle sane_handle, SANE_String_Const fileName)
{
    device = sane_handle; //全局静态变量初始化

    return do_scan(fileName);
}

/**
 * @brief cancle_scan  结束扫描
 * @param sane_handle 扫描句柄
 */
void cancle_scan(SANE_Handle sane_handle)
{
    sane_cancel(sane_handle);
}

/**
 * @brief close_device 关闭扫描设备
 * @param sane_handle 扫描句柄
 */
void close_device(SANE_Handle sane_handle)
{
    sane_close(sane_handle);
}

/**
 * @brief my_sane_exit 释放所有扫描资源
 */
void my_sane_exit()
{
    sane_exit();
}

/**
 * @brief check_search_scan_devices 可以借此整理出未识别设备的情况
 * @return 检查情况返回值
 */
int check_search_scan_devices()
{
    // 1. initialize SANE
    qDebug("SANE Init\n");
    init();

    // 2. get all devices
    const SANE_Device ** device_list = nullptr;
    SANE_Status sane_status;
    sane_status = get_devices(&device_list);
    if (sane_status)
    {
        // 6. release resources
        qDebug("Exit\n");
        my_sane_exit();
        return -1;
    }	
    return 0;
}

/**
 * @brief kylinNorScanFindDevice 查询扫描设备统一接口
 * 提供给页面调用
 */
void kylinNorScanFindDevice()
{
    KylinSane& instance = KylinSane::getInstance();
    QStringList names;

    SANE_Status sane_status;
    char name[512] = {0};

    // 1. initialize SANE
    qDebug("SANE Init\n");
    init();

    do
    {
        // 2. get all devices
        //const SANE_Device ** device_list = nullptr;
        sane_status = get_devices((&device_list));
        if (sane_status)
        {
            instance.setKylinSaneStatus(false);
            break;
        }

        // display all devices
        int i = 0;
        unsigned int column = 80;

        for (i = 0; device_list[i]; ++i)
        {
            if (column + strlen (device_list[i]->name) + 1 >= 80)
            {
                qDebug ("\n    ");
                column = 4;
            }
            if (column > 4)
            {
                fputc (' ', stdout);
                column += 1;
            }
            fputs (device_list[i]->name, stdout);
            column += strlen (device_list[i]->name);
        }
        fputc ('\n', stdout);

        for (i = 0; device_list[i]; ++i)
        {
            qDebug ("device `%s' is a %s %s %s\n",
                     device_list[i]->name, device_list[i]->vendor,
                     device_list[i]->model, device_list[i]->type);

            // just for one scan device
            snprintf(name, 512, "%s %s", device_list[i]->vendor, device_list[i]->model);
            qDebug() << "device name:  " << name;

            names << name;

            //instance.setKylinSaneName(device_list[i]->name);
            if (i == 0)
            {
                // Same device have same type
                instance.setKylinSaneType(device_list[i]->type);
            }
            //break;
        }

        // For devices name
        qDebug() << names;
        instance.setKylinSaneNames(names);

        if (!device_list[0])
        {
            qDebug ("no SANE devices found\n");
            sane_status = SANE_STATUS_UNSUPPORTED;
            instance.setKylinSaneStatus(false);
            break;
        }

        sane_status = SANE_STATUS_GOOD;

    }while(0);

    if (sane_status)
    {
        instance.setKylinSaneStatus(false);
        qDebug() << "find device set status false";
    }
    else
    {
        instance.setKylinSaneStatus(true);
    }
}

void kylinNorScanOpenDevice(int index)
{
    KylinSane& instance = KylinSane::getInstance();
    QStringList names;

    SANE_Status sane_status;
    char name[512] = {0};

    do
    {
        for (int i = 0; device_list[i]; ++i)
        {
            qDebug ("device `%s' is a %s %s %s\n",
                     device_list[i]->name, device_list[i]->vendor,
                     device_list[i]->model, device_list[i]->type);

            // just for one scan device
            snprintf(name, 512, "%s %s", device_list[i]->vendor, device_list[i]->model);
            qDebug() << "device name:  " << name;

            names << name;

            //instance.setKylinSaneName(device_list[i]->name);
            if (i == 0)
            {
                // Same device have same type
                instance.setKylinSaneType(device_list[i]->type);
            }
            //break;
        }

        // For devices name
        qDebug() << names;
        instance.setKylinSaneNames(names);

        if (!device_list[0])
        {
            qDebug ("no SANE devices found\n");
            sane_status = SANE_STATUS_UNSUPPORTED;
            instance.setKylinSaneStatus(false);
            break;
        }

        // 3. open a device
        qDebug("Open a device\n");
        SANE_Handle sane_handle;
        SANE_Device *device = const_cast<SANE_Device *>(*(device_list + index));
        if (!device)
        {
            qDebug("No device connected!\n");
            sane_status = SANE_STATUS_UNSUPPORTED;
            instance.setKylinSaneStatus(false);
            break;
        }

        sane_status = open_sane_device(device, &sane_handle);
        if (sane_status)
        {
            qDebug("Open device failed!\n");
            instance.setKylinSaneStatus(false);
            break;
        }
        instance.handle = sane_handle;

        // 4. start scanning
        qDebug("Scanning...\n");

        // 此处可以获取页面设置所需的扫描信息
        sane_status = kylin_display_scan_parameters(sane_handle);

        cancle_scan(sane_handle);

        //sane_status = SANE_STATUS_GOOD;
    }while(0);

    if (sane_status)
    {
        instance.setKylinSaneStatus(false);
        qDebug() << "open device set status false";
    }
    else
    {
        instance.setKylinSaneStatus(true);
    }
}
#ifdef __cplusplus
}
#endif

KylinSane::KylinSane(QWidget *parent) : QWidget(parent)
{

}

KylinSane::~KylinSane()
{

}

bool KylinSane::getKylinSaneStatus()
{
    return devicesInfo.status;
}

QStringList KylinSane::getKylinSaneNames()
{
    return devicesInfo.name;
}

QString KylinSane::getKylinSaneType()
{
    return devicesInfo.type;
}

QStringList KylinSane::getKylinSaneResolutions()
{
    return  devicesInfo.resolution;
}

QStringList KylinSane::getKylinSaneSizes()
{
    return devicesInfo.size;
}

QStringList KylinSane::getKylinSaneColors()
{
    return devicesInfo.color;
}

void KylinSane::setKylinSaneStatus(bool status)
{
    devicesInfo.status = status;
}

void KylinSane::setKylinSaneType(QString type)
{
    devicesInfo.type = type;
}

void KylinSane::setKylinSaneNames(QStringList name)
{
    devicesInfo.name = name;
}

void KylinSane::setKylinSaneResolutions(QStringList resolution)
{
    devicesInfo.resolution = resolution;
}

void KylinSane::setKylinSaneSizes(QStringList size)
{
    devicesInfo.size = size;
}

void KylinSane::setKylinSaneColors(QStringList color)
{
    devicesInfo.color = color;
}

/**
 * @brief KylinSane::find_device 统一接口供页面调用，找到扫描设备
 * @return 返回找到扫描设备的扫描参数情况
 */
device_info KylinSane::find_device()
{
    KylinSane& instance = KylinSane::getInstance();
    kylinNorScanFindDevice();

    return instance.devicesInfo;
}

/**
 * @brief KylinSane::open_device 统一接口供页面调用，找到扫描设备
 * @return 返回找到扫描设备的扫描参数情况
 */
device_info KylinSane::open_device(int index)
{
    KylinSane& instance = KylinSane::getInstance();
    kylinNorScanOpenDevice(index);

    return instance.devicesInfo;
}

/**
 * @brief KylinSane::start_scanning 统一接口供页面调用，开始扫描
 * 根据扫描设置页面情况info字段进行设置扫描设备参数，并进行扫描
 *
 * @param info 扫描设置页面传递的需要设置的扫描设备参数
 * @return 返回扫描情况
 */
int KylinSane::start_scanning(user_selected_info info)
{
    KylinSane& instance = KylinSane::getInstance();
    int ret = 0;
    SANE_Status status = SANE_STATUS_GOOD;
    string strColor, strResolution, strSize;
    SANE_String s_color, s_resolution, s_size;

    strColor = info.color.toStdString();
    strResolution = info.resolution.toStdString();
    strSize = info.size.toStdString();

    // For colors
    s_color = const_cast<SANE_String>(strColor.c_str());
    status = set_option_colors(instance.handle, s_color);
    if (status != SANE_STATUS_GOOD)
    {
      qDebug("cannot set option 2 to %s (%s)\n",s_color, sane_strstatus(status));
    }

    // For resolutions
    s_resolution = const_cast<SANE_String>(strResolution.c_str());
    SANE_Int i_resolution;
    if (!strcmp("auto", s_resolution) || !strcmp("自动", s_resolution))
    {
        i_resolution = 300; //自动时设置为300
    }
    else
    {
        i_resolution = static_cast<SANE_Int>(atoi(s_resolution));
    }

    status = set_option_resolutions(instance.handle, i_resolution);
    if (status != SANE_STATUS_GOOD)
    {
      qDebug("cannot set option 2 to %s (%s)\n",s_resolution, sane_strstatus(status));
    }

    // For sizes
    int type = 0;
    s_size = const_cast<SANE_String>(strSize.c_str());
    if (!strcmp("A2", s_size))
    {
        type = A2;
    }
    else if (!strcmp("A3", s_size))
    {
        type = A3;
    }
    else if (!strcmp("A4", s_size))
    {
        type = A4;
    }
    else if (!strcmp("A5", s_size))
    {
        type = A5;
    }
    else if (!strcmp("A6", s_size))
    {
        type = A6;
    }
    status = set_option_sizes_all(instance.handle, type);
    if (status != SANE_STATUS_GOOD)
    {
      qDebug("cannot set option 2 to %s (%s)\n",s_size, sane_strstatus(status));
    }

    ret = start_scan(instance.handle, "scan");

    cancle_scan(instance.handle);

    return ret;
}
