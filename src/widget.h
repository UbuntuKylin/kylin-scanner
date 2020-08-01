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
#include "scanDisplay.h"
#include "scanSet.h"
#include "titleBar.h"

class CommonScanThread : public QThread
{
    Q_OBJECT
public:
    // 将连接设备，获取设备信息放在线程中，因为这个过程耗费资源
    void run() Q_DECL_OVERRIDE;

Q_SIGNALS:
    // 此结束信号，用来处理是否获取到设备信息的界面操作
    void scanFinished(bool);
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

    void setPdfSize(QPdfWriter *pdfWriter,QString size);
    void saveToPdf(QImage img, QString pathName);
    void resultDetail(bool ret);

private:
    TitleBar *pTitleBar;
    QFrame *line;
    FuncBar *pFuncBar;
    ScanSet *pScanSet;
    ScanDisplay *pScandisplay;
    QHBoxLayout *pHboxLayout;
    QVBoxLayout *pLayout;
    CommonScanThread thread;

Q_SIGNALS:
    void openScanDeviceFinished(bool);

private slots:
    void saveImage(QString fileName);
    void saveScanFile();
    void scanResult(bool ret);
    void scanResultDetail(bool ret);
    void setMaskClear();
    void setWindowBorderRadius();
};

#endif // WIDGET_H
