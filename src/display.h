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

#ifndef DISPLAY_H
#define DISPLAY_H

#include <qmath.h>
#include <QStackedLayout>
#include <QInputDialog>
#include <QScrollArea>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QToolTip>
#include <QSize>
#include <QTextEdit>
#include <QTimer>
#include <QObject>
#include "tailor.h"
#include "watermark.h"
#include "functionbar.h"
#include "deskew.h"
#include "beauty.h"
#include "scanner.h"
#include "timewait.h"
#include "interrupt.h"
#include "theme.h"
#include "svghandler.h"

/**
 * @brief The KYCOcrThread class
 * Meet large image, ocr operation will take a long time to run.
 * So, avoid blocking MainWindow UI, handle it with thread.
 * this still block GUI
 */
class KYCOcrThread : public QThread
{
    Q_OBJECT
public:
    void run() Q_DECL_OVERRIDE;

signals:
    void ocrFinished();
};

/*
class MKYCOcrThread: public QWidget
{
    Q_OBJECT

public:
    explicit MKYCOcrThread(QObject *parent = 0)
    {
        ocrTimer.setInterval(5000);

        connect(&ocrTimer, SIGNAL(timeout()), this, SLOT(ocrTask()));
    }

public slots:
    void ocrTask();

signals:
    void ocrFinished();

private:
    QTimer ocrTimer;
};
*/

/**
 * @brief The KYCRectifyThread class
 * Meet large image, ImageRectify() will take a long time to run.
 * So, avoid blocking MainWindow UI, handle it with thread.
 */
class KYCRectifyThread : public QThread
{
    Q_OBJECT
public:
    void run() Q_DECL_OVERRIDE;

signals:
    void rectifyFinished();
};

/**
 * @brief The KYCBeautyThread class
 * Meet large image, oneClickBeauty() will take a long time to run.
 * So, avoid blocking MainWindow UI, handle it with thread.
 */
class KYCBeautyThread : public QThread
{
    Q_OBJECT
public:
    void run() Q_DECL_OVERRIDE;

signals:
    void beautyFinished();
};

class KYCEditBarWidget  :   public QWidget
{
    Q_OBJECT

public:
    KYCEditBarWidget(QWidget *parent = nullptr);

    void setEditBarWindowBorderRadius();
    bool getBtnTailorClickedStatus();
    void setBtnTailorClickedStatus(bool clicked);
//private:

    bool m_clicked = false;

    QStringList stylelist;
    QStringList iconthemelist;

    SVGHandler  *svghandler;

    QGSettings *style_settings;
    QGSettings *icon_theme_settings;

    QPushButton *btnTailor;
    QPushButton *btnRotate;
    QPushButton *btnSymmetry;
    QPushButton *btnWatermark;
    QVBoxLayout *vBoxEditBar;

Q_SIGNALS:
    void btnTailorClicked();
    void btnTailorClickedEnd();

private slots:
    void onBtnTailorClicked();
    void onEditBarThemeChanged(QString);
};

class KYCScanDisplayWidget  : public QWidget
{
    Q_OBJECT

public:
    KYCScanDisplayWidget(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *e);
    void timerEvent(QTimerEvent *e);

    void initWindow();
    void initLayout();
    void initStyle();
    void initConnect();

    void setNoDevice();
    void setInitDevice();
    void setOcrThreadQuit();
    void setPixmap(QImage img, QLabel *lab);
    float setPixmapScaled(QImage img, QLabel *lab);
    void updateWindowSize();
    void setOcrFlagStatus();

    void initBeforeScanAgain();
    void initStyleOcr();
    void initStyleTailor();
    void initSavePresentImage();
    void onOcr();
    void onScan(bool ret);

    void onSaveImageNow();
    void setNormalImage();
    void rectifyThreadQuit();
    void beautifyThreadQuit();

    QImage *imageSave(QString fileName);

private:
    int flagBeautify = 0; //一键美化标志
    int flagRectify = 0; //智能纠偏标志
    int flagOcr = 0; //文字识别标志
    int flagOcrInit = 0; // 文字识别已出现样式标志
    int flagTailor = 0; // 用于裁切的主题切换问题
    int flagWaterMark = 0; // 用于扫描完成后添加水印时的覆盖问题
    float scaledNum = 1; //缩放倍数
    int index = 0; // 控制工具栏的打开和关闭
    int m_timerNum = 0; // 计时器执行次数
    int m_timeScanId; // 正常扫描结束后显示扫描结果定时器id
    QStack<QImage> stack;        //用于保存图片
    QList<QString> list;

    SVGHandler *svghandler;
    QStringList stylelist;
    QStringList iconthemelist;
    QGSettings *style_settings;
    QGSettings *icon_theme_settings;
    QTimer *timerScan; // 实时显示扫描结果定时器
    QLabel *labInit; // 初始化界面，即空白界面
    QLabel *labConnectError; // 连接或者打开扫描仪出错界面
    QLabel *labConnectErrorText; //连接或者打开扫描仪出错界面
    QLabel *labNormalLeft;       //正常显示界面左部分
    QLabel *labNormalRight;      //正常显示界面右部分
    QLabel *labEditLayout;       //编辑栏展开界面的显示部分
    KYCTailorLabel *labTailor;         //编辑栏
    QLabel *labOcrLeft;          //文字识别图片显示部分
    QLabel *labOcrRight;         //文字识别文字显示部分
    QPushButton *btnNormal;      //正常显示界面按钮
    QPushButton *btnEditLayout;  //编辑栏展开界面按钮
    QPushButton *btnTailorLayout;      //裁剪界面按钮

    QImage *imgConnectError;     // 连接或者打开扫描仪出错界面显示图片
    QImage *imgEditLayout;       //编辑栏展开界面图片
    QImage *imgTailor;           //裁剪界面图片
    QImage *imgStack;            //栈中保存图片
    QImage *imgNormal;           //正常显示界面图片
    QImage *imgBackup;           //添加水印时图片
    QImage *imgBeautify;         //一键美化图片
    QImage *imgRectify;          //智能纠偏图片

    QVBoxLayout *vBoxConnectError;
    QVBoxLayout *vBoxScanSet;
    QVBoxLayout *vBoxOcr;
    QHBoxLayout *hBoxNormal;
    QHBoxLayout *hBoxEditLayout;
    QHBoxLayout *hBoxTailor;
    QHBoxLayout *hBoxOcr;
    QWidget *widgetConnectError;
    QWidget *widgetNormal;
    QWidget *widgetEditLayout;
    QWidget *widgetTailor;
    QWidget *widgetOcr;
    QStackedLayout *vStackedLayout;
    KYCEditBarWidget *editLayout; // 未点击裁切的工具栏界面
    KYCEditBarWidget *editLayoutTailor; // 开始裁切时的工具栏界面
    QScrollArea *scrollArea;

    int widgetindex;
    KYCOcrThread ocrThread;
    KYCRectifyThread rectifyThread;
    KYCBeautyThread beautyThread;
    /*
    QThread *m_ocrThread;
    MKYCOcrThread *m_ocrTask;
    */

public slots:
    void onBtnRectifyBegin();
    void onBtnRectifyEnd();
    void onBtnBeautifyBegin();
    void onBtnBeautifyEnd();
    void switchPage();
    void timerScanUpdate();

private slots:
    void onTailor();
    void onTailorEnd();
    void rotating();
    void symmetry();
    void addWatermark();
    void ocrText();
    void rectifyEnd();
    void beautyEnd();
    void scandisplay_theme_changed(QString);

signals:
    void scanTimerFinished();
};

#endif // DISPLAY_H
