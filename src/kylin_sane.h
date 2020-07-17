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
#ifndef KYLIN_SANE_H
#define KYLIN_SANE_H

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QString>
#include <QStringList>

#include "sane/sane.h"
#include "sane/saneopts.h"

#include <iostream>
using namespace std;

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

struct device_info
{
    bool status;			// 是否存在设备，存在status值为true，否则为false
    QStringList name;		// 设备名
    QString type;			// 设备类型
    QStringList color;		// 设备支持的色彩
    QStringList resolution;	// 设备支持的分辨率
    QStringList size; 		// 设备支持的尺寸
};

struct user_selected_info
{
    QString name;			// 用户选择设备名
    QString color;			// 用户选择色彩
    QString resolution;		// 用户选择分辨率
    QString size;			// 用户选择尺寸
};

enum sizes_type
{
    A2 = 1,
    A3,
    A4,
    A5,
    A6
};

class  KylinSane: public QWidget
{
    Q_OBJECT
public:
    explicit KylinSane(QWidget *parent = nullptr);
    ~KylinSane();
    KylinSane(const KylinSane &) = delete ;
    KylinSane &operator = (const KylinSane &) = delete ;

    // 线程安全,不用生成对象，防止内存泄漏，是一种很好的懒汉单例模式
    static KylinSane& getInstance()
    {
        static KylinSane instance; //局部静态变量
        return instance;
    }

    bool getKylinSaneStatus();
    QStringList getKylinSaneNames();
    QString getKylinSaneType();
    QStringList getKylinSaneResolutions();
    QStringList getKylinSaneSizes();
    QStringList getKylinSaneColors();

    void setKylinSaneStatus(bool status);
    void setKylinSaneNames(QStringList name);
    void setKylinSaneType(QString type);
    void setKylinSaneResolutions(QStringList resolution);
    void setKylinSaneSizes(QStringList size);
    void setKylinSaneColors(QStringList color);

    device_info find_device();
    device_info open_device(int index);
    int start_scanning(user_selected_info info);

    SANE_Handle handle;
    struct user_selected_info userInfo;

private:
    struct device_info devicesInfo;

public slots:
};


#ifdef __cplusplus
extern "C" {
#endif

void init(); // init scan

SANE_Status get_devices(const SANE_Device ***device_list); // Get all devices

SANE_Status open_sane_device(SANE_Device *device, SANE_Handle *sane_handle); // Open a device

SANE_Status start_scan(SANE_Handle sane_handle, SANE_String_Const fileName); // start scanning

void cancle_scan(SANE_Handle sane_handle); // cancel scanning

void close_device(SANE_Handle sane_handle); // close scan device

void my_sane_exit(); // Release scan resources

SANE_Status kylin_display_scan_parameters(SANE_Handle device); // display parameters

void kylinNorScanFindDevice(); // open scan device


#ifdef __cplusplus
}
#endif

#endif
