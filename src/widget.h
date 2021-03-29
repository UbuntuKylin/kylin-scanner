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
#ifndef WIDGET_H
#define WIDGET_H

#include <QPdfWriter>
#include <QMap>
#include <QIcon>
#include <QSplitter>
#include <QLine>
#include <QString>
#include "display.h"
#include "settings.h"
#include "titlebar.h"
#include "theme.h"
#include "usbhotplug.h"

#define MAINWINDOW_WIDTH 860
#define MAINWINDOW_HEIGHT 680

class KYCCommonScanThread : public QThread
{
    Q_OBJECT
public:
    // 将连接设备，获取设备信息放在线程中，因为这个过程耗费资源
    void run() Q_DECL_OVERRIDE;

Q_SIGNALS:
    // 此结束信号，用来处理是否获取到设备信息的界面操作
    void scanFinished(bool);
};

class KYCWidget : public QWidget
{
    Q_OBJECT

public:
    KYCWidget(QWidget *parent = 0);
    ~KYCWidget();

    void setPdfSize(QPdfWriter *pdfWriter, QString size);
    void saveToPdf(QImage img, QString pathName);
    void resultDetail(bool ret);
    int messageScanFinishedSave(QString pathName);
    void warnMsg(QString msg);
    QString getScannerPath(QString str);

private:
    QGSettings *style_settings;
    QGSettings *icon_theme_settings;
    QStringList stylelist;
    QStringList iconthemelist;
    KYCTitleBarDialog *pTitleBar;
    QFrame *line;
    KYCFunctionBarWidget *pFuncBar;
    KYCScanSettingsWidget *pScanSet;
    KYCScanDisplayWidget *pScandisplay;
    QHBoxLayout *pHboxLayout;
    QVBoxLayout *pLayout;

    KYCCommonScanThread thread;
    KYCUsbHotplugThread usbThread;

Q_SIGNALS:
    void openScanDeviceFinished(bool);
    void sendMailInitSignal();

private slots:
    void saveImage(QString fileName);
    void setScanSetBtnEnable(bool ret);
    void saveScanFile(bool ret);
    void scanResult(bool ret);
    void swichScanDeviceResult(bool ret);
    void scanningResultDetail(bool ret);
    void sendMailPrepare();
    void setMaskClear();
    void setWindowBorderRadius();
    void style_changed(QString); // 系统主题风格变化
    void icon_theme_changed(QString); // 系统图标主题风格变化
    void usbDeviceAdded(QString recvData);
    void usbDeviceRemoved(QString recvData);
    void scanListResult(int ret);
};

#endif // WIDGET_H
