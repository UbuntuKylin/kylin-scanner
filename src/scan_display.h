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
    void updateWindowSize();
    int flagBeautify = 0; //一键美化标志
    int flagRectify = 0; //智能纠偏标志
    int flagOrc = 0; //文字识别标志
    float scaledNum = 1; //缩放倍数
    int index = 0;
    int n = 0;

private:
    QLabel *labInit; // 初始化界面，即空白界面
    QLabel *labConnectError; // 连接或者打开扫描仪出错界面
    QLabel *labConnectErrorText; //连接或者打开扫描仪出错界面
    QLabel *labNormalLeft;       //正常显示界面左部分
    QLabel *labNormalRight;      //正常显示界面右部分
    QLabel *labEditLayout;       //编辑栏展开界面的显示部分
    my_label *labTailor;         //编辑栏
    QLabel *labOrcLeft;          //文字识别图片显示部分
    QLabel *labOrcRight;         //文字识别文字显示部分
    QPushButton *btnNormal;      //正长显示界面按钮
    QPushButton *btnEditLayout;  //编辑栏展开界面按钮
    QPushButton *btnTailor;      //裁剪界面按钮

    QImage *imgConnectError;     // 连接或者打开扫描仪出错界面显示图片
    QImage *imgEditLayout;       //编辑栏展开界面图片
    QImage *imgTailor;           //裁剪界面图片
    QImage *imgStack;            //栈中保存图片
    QImage *imgNormal;           //正常显示界面图片
    QImage *imgBackup;           //添加水印时图片
    QImage *imgBeautify;         //一键美化图片
    QImage *imgRectify;          //智能纠偏图片
    QStack<QImage> stack;        //用于保存图片
    QVBoxLayout *vBoxConnectError;
    QVBoxLayout *vBoxScanSet;
    QVBoxLayout *vBoxOrc;
    QHBoxLayout *hBoxNormal;
    QHBoxLayout *hBoxEditLayout;
    QHBoxLayout *hBoxTailor;
    QHBoxLayout *hBoxOrc;
    QWidget *widgetConnectError;
    QWidget *widgetNormal;
    QWidget *widgetEditLayout;
    QWidget *widgetTailor;
    QWidget *widgetOrc;
    QStackedLayout *vStackedLayout;
    edit_bar *editLayout;
    edit_bar *editLayoutTailor;
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
    void tailor();
    void rotating();
    void symmetry();
    void addmark();
    void orcText();
};

#endif // SCAN_DISPLAY_H
