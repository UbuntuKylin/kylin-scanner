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
#include "common.h"
#include "about.h"
#include "interrupt.h"

class KYCCommonScanThread : public QThread
{
    Q_OBJECT
public:
    void run() Q_DECL_OVERRIDE;

Q_SIGNALS:
    // Whether find scanners in the first time
    void initFindScanDevicesFinished(bool);
};

class KYCWidget : public QWidget
{
    Q_OBJECT

public:
    KYCWidget(QWidget *parent = 0);
    ~KYCWidget();

    void initWindow();
    void initLayout();
    void initStyle();
    void initSetting();
    void initConnect();

    void setPdfSize(QPdfWriter *pdfWriter, QString size);
    void saveToPdf(QImage img, QString pathName);
    void resultDetail(bool ret);
    void setScanSetBtnEnable(bool ret);
    void saveScanFile(bool ret);
    void warnMsg(QString msg);
    void scanningResultDetail(bool ret);
    void sendMailPrepare();
    void setUsbThreadQuit();

    int messageScanFinishedSave(QString pathName);
    QString getScannerPath(QString str);

private:
    QGSettings *style_settings;
    QGSettings *icon_theme_settings;
    QStringList stylelist;
    QStringList iconthemelist;
    KYCTitleBarDialog *pTitleBar;
    KYCAboutDialog *m_pAbout;
    //KYCInterruptDialog *m_pAbout;
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
    void scanResult(bool ret);
    void switchScanDeviceResult(bool ret);
    void setMaskClear();
    void showAboutDialogCenter();
    void setWindowBorderRadius();
    void style_changed(QString); // 系统主题风格变化
    void icon_theme_changed(QString); // 系统图标主题风格变化
    void usbDeviceAdded(QString recvData);
    void usbDeviceRemoved(QString recvData);
    void scanListResult(int ret);
    void setScanEndOperation(bool ret);
    void setOcrFlags();
    void setOcrBeginOperation();
    void setOcrEndOperation();
};

#endif // WIDGET_H
