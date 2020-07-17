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
#ifndef SCAN_DISPLAY_H
#define SCAN_DISPLAY_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QImage>
#include <QPushButton>
#include <QPixmap>
#include <QPainter>
#include <QString>
#include <QInputDialog>
#include <qmath.h>
#include <QStack>
#include <QThread>
#include <QScrollArea>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include "my_label.h"
#include "mark_dialog.h"
#include "func_bar.h"
#include <QWidget>
#include "rectify.h"
#include "embelish.h"

class myThread : public QThread
{
    Q_OBJECT
public:
    void run() Q_DECL_OVERRIDE;
signals:
    void orcFinished();
};


class edit_bar  :   public QWidget
{
    Q_OBJECT
public:
    edit_bar(QWidget *parent = nullptr);
//private:
    QPushButton *btnTailor;
    QPushButton *btnRotate;
    QPushButton *btnSymmetry;
    QPushButton *btnWatermark;
    QVBoxLayout *vBoxEditBar;
};

class scan_display  : public QWidget
{
    Q_OBJECT
public:
    scan_display(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *e);
    QImage *imageSave(QString fileName);
    void set_no_device();
    void set_init_device();
    void set_pixmap(QImage img, QLabel *lab);
    float pixmap_scaled(QImage img, QLabel *lab);
    int flagBeautify = 0; //一键美化标志
    int flagRectify = 0; //智能纠偏标志
    int flagOrc = 0; //文字识别标志
    float scaledNum = 1; //缩放倍数
    int index = 0;
    int n = 0;

private:
    QLabel *labDisplayInit; // 初始化界面，即空白界面
    QLabel *labDisplayConnectError; // 连接或者打开扫描仪出错界面
    QLabel *labDisplay3;
    QLabel *labDisplay4;
    QLabel *labDisplay5;
    QLabel *labDisplay6;
    QLabel *labDisplay7;
    my_label *labDisplay8;
    QLabel *labDisplay9;
    QLabel *labDisplay10;
    QPushButton *btnTool;
    QPushButton *btnTool1;
    QPushButton *btnTool2;

    QImage *img;
    QImage *img1;
    QImage *img2;
    QImage *img3;
    QImage *img4;
    QImage *img5;
    QImage *img6;
    QImage *img7;
    QImage *imgBeautify;
    QImage *imgRectify;
    QStack<QImage> stack;
    QVBoxLayout *vBoxScanSet;
    QVBoxLayout *vBoxScanSet1;
    QVBoxLayout *vBoxScanSet2;
    QVBoxLayout *vBoxScanSet3;
    QHBoxLayout *hBoxScanSet;
    QHBoxLayout *hBoxScanSet1;
    QHBoxLayout *hBoxScanSet2;
    QHBoxLayout *hBoxScanSet3;
    QWidget *myWidget;
    QWidget *myWidget1;
    QWidget *myWidget2;
    QWidget *myWidget3;
    QWidget *myWidget4;
    QStackedLayout *vStackedLayout;
    edit_bar *editlayout;
    edit_bar *editlayout1;
    edit_bar *editlayout2;
    myThread thread;
    QScrollArea *scrollArea;
    int widgetindex;
    QList<QString> list;

public slots:
    void orc();
    void scan();
    void rectify();
    void beautify();

private slots:
    void switchPage();
    void switchPage1();
    void rotating();
    void symmetry();
    void addmark();
    void orcText();
};

#endif // SCAN_DISPLAY_H
