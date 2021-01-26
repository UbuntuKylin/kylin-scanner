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

#include "widget.h"
#include <QApplication>
#include <QLabel>
#include <QTranslator>
#include <QMessageBox>

bool device = false;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , style_settings (new QGSettings(ORG_UKUI_STYLE))
    , icon_theme_settings (new QGSettings(ORG_UKUI_STYLE))
    , pTitleBar (new TitleBar())
    , line (new QFrame())
    , pFuncBar (new  FuncBar())
    , pScanSet (new ScanSet())
    , pScandisplay (new ScanDisplay())
    , pHboxLayout (new QHBoxLayout())
    , pLayout (new QVBoxLayout())
{
    setWindowTitle (tr("kylin-scanner")); // For system tray text
    setWindowIcon (QIcon::fromTheme("kylin-scanner"));

    //stylelist << STYLE_NAME_KEY_DARK << STYLE_NAME_KEY_BLACK << STYLE_NAME_KEY_DEFAULT;
    stylelist << STYLE_NAME_KEY_DARK << STYLE_NAME_KEY_BLACK;
    iconthemelist << ICON_THEME_KEY_BASIC << ICON_THEME_KEY_CLASSICAL << ICON_THEME_KEY_DEFAULT;

#ifdef DEBUG_EDIT
    KylinSane &instance = KylinSane::getInstance();
    instance.setKylinSaneStatus (true);
#else
    thread.start();
#endif

    resize(MAINWINDOW_WIDTH, MAINWINDOW_HEIGHT);
    pTitleBar->setMainWindowAttribute(this->width(), this->height());

    line->setObjectName(QString::fromUtf8("line"));
    line->setGeometry(QRect(0, 32, 860, 1));
    line->setFrameShape(QFrame::HLine);
    line->hide ();

#ifdef DEBUG_EDIT
#else
        // 未扫描时，左下角的发送邮件和另存为等所有设置都不能点击
        pScanSet->setKylinScanSetNotEnable();
#endif

    qDebug() << "1 styleName = " << style_settings->get(STYLE_NAME).toString();
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
    // 设置窗口背景
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(47, 44, 43));
        setAutoFillBackground(true);
        setPalette(pal);

        line->setStyleSheet("QFrame{color:#201E1D}");
    } else {
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(255, 255, 255));
        setAutoFillBackground(true);
        setPalette(pal);

        line->setStyleSheet("QFrame{color:#DCDCDC}");
    }

    installEventFilter(pTitleBar);
    installEventFilter(pFuncBar);
    installEventFilter(pScanSet);

    pHboxLayout->setSpacing(0); // 需要先清空spacing，不然addSpacing会出问题
    pHboxLayout->addWidget(pScanSet);
    pHboxLayout->addWidget(pScandisplay);
    pHboxLayout->setContentsMargins(0,0,0,0); // 设置窗口左上右下边距

    pLayout->setSpacing(0);
    pLayout->addWidget(pTitleBar);
    pLayout->addWidget(line);
    pLayout->addWidget(pFuncBar);
    pLayout->addLayout(pHboxLayout);
    pLayout->setContentsMargins(0, 0, 0, 0);

    // 设置窗口圆角
    //setWindowBorderRadius();
    setLayout(pLayout);

    // For save
    connect(pScanSet,&ScanSet::saveImageSignal,this,&Widget::saveImage);

    // For ORC
    connect(pFuncBar,&FuncBar::sendOrcBegin,pScandisplay,&ScanDisplay::onOrc);
    connect(pFuncBar,&FuncBar::sendOrcEnd,pScandisplay,&ScanDisplay::onOrc);
    connect(pFuncBar,&FuncBar::sendOrcBegin,pScanSet,&ScanSet::modifyBtnSave);
    connect(pFuncBar,&FuncBar::sendOrcEnd,pScanSet,&ScanSet::modifyBtnSave);

    // 文件扫描成功后默认显示全部编辑框
    //connect(pFuncBar,&FuncBar::sendScanEnd, pScandisplay, &ScanDisplay::switchPage);

    // For scan
    connect(&thread,SIGNAL(scanFinished(bool)),this,SLOT(scanResult(bool)));
    connect(pScanSet,SIGNAL(openDeviceStatusSignal(bool)),this,SLOT(scanResultDetail(bool)));

    connect(pFuncBar,SIGNAL(sendScanEnd(bool)),pScandisplay,SLOT(onScan(bool)));
    connect(pFuncBar,SIGNAL(sendScanEnd(bool)),this,SLOT(setScanSetBtnEnable(bool)));
    connect(pFuncBar,SIGNAL(sendScanEnd(bool)),this,SLOT(saveScanFile(bool)));
    connect(pFuncBar,SIGNAL(sendScanEnd(bool)),this,SLOT(scanningResultDetail(bool)));

    // For rectify
    connect(pFuncBar,&FuncBar::sendRectifyBegin,pScandisplay,&ScanDisplay::onRectify);
    connect(pFuncBar,&FuncBar::sendRectifyEnd,pScandisplay,&ScanDisplay::onRectify);

    // For beauty
    connect(pFuncBar, &FuncBar::sendBeautifyBegin, pScandisplay, &ScanDisplay::onBeautify);
    connect(pFuncBar, &FuncBar::sendBeautifyEnd, pScandisplay, &ScanDisplay::onBeautify);

    connect(pTitleBar,&TitleBar::isNormal,this,&Widget::setWindowBorderRadius);
    connect(pTitleBar,&TitleBar::isMax,this,&Widget::setMaskClear);

    // For white and black style
    connect(style_settings,SIGNAL(changed(QString)),this,SLOT(style_changed(QString)));

    // For icon theme change style
    connect(icon_theme_settings,SIGNAL(changed(QString)),this,SLOT(icon_theme_changed(QString)));
}

Widget::~Widget()
{

}

int toUnicode(QString str)
{
    char*  ch;
    QByteArray ba = str.toLatin1();
    ch=ba.data();
    return ch[0] + (ch[1] ? toUnicode(ch + 1) : 0);
}

constexpr inline int U(const char* str)
{
    return str[0] + (str[1] ? U(str + 1) : 0);
}

void Widget::setPdfSize(QPdfWriter *pdfWriter, QString size)
{
    switch (toUnicode(size))
    {
    case U("A0"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A0));
        break;
    case U("A1"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A1));
        break;
    case U("A2"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A2));
        break;
    case U("A3"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A3));
        break;
    case U("A4"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A4));
        break;
    case U("A5"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A5));
        break;
    case U("A6"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A6));
        break;
    case U("A7"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A7));
        break;
    case U("A8"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A8));
        break;
    case U("A9"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A9));
        break;
    default:
        pdfWriter->setPageSize(QPageSize(QPageSize::A4));
        break;
    }
}

void Widget::saveToPdf(QImage img, QString pathName)
{

    QFile pdfFile(pathName);
    pdfFile.open(QIODevice::WriteOnly);
    QPdfWriter *pdfWriter = new QPdfWriter(&pdfFile);
    setPdfSize(pdfWriter,pScanSet->getTextSize());
    int resolution = pScanSet->getTextResolution().toInt();
    pdfWriter->setResolution(resolution);//像素

    int pageMargin = 0;
    pdfWriter->setPageMargins(QMarginsF(pageMargin, pageMargin, pageMargin, pageMargin));

    QPainter *pdfPainter = new QPainter(pdfWriter);

    int yCurrentP = 0;
    int xCurrentP = 0;


//    std::string titleImageFileName = "/tmp/scanner/scan.png";
    QString titleImageFileName = "/tmp/scanner/scan.png";
    QPixmap titleImage;
//    titleImage.load(QString::fromStdString(titleImageFileName));
    titleImage.load(titleImageFileName);
    QPixmap pixmap = QPixmap::fromImage(img);
    pdfPainter->drawPixmap(xCurrentP, yCurrentP, pixmap.width(), pixmap.height(), pixmap);

    delete pdfPainter;
    delete pdfWriter;
    pdfFile.close();
}

/**
 * @brief Widget::result_detail 处理初始化时设备选择组合框情况
 * 初始化时，应该根据获取的扫描信息进行设置textDevice，所以应该为setKylinComboBox 的参数为 false
 * @param ret 获取的扫描结果状态
 */
void Widget::resultDetail(bool ret)
{
#ifdef DEBUG_EDIT
    ret = true;
#endif

    if (ret)
    {
        device = true;
        pScanSet->setKylinComboBox(false);
        pScanSet->setKylinLable();
        pFuncBar->setBtnScanEnable();
        pScanSet->setKylinScanSetEnable();
    }
    else
    {
        device = false;
        pScandisplay->setNoDevice();
        pFuncBar->setKylinScanSetNotEnable();
        pScanSet->setKylinScanSetNotEnable();
    }
}

int Widget::messageScanFinishedSave(QString pathName)
{
    KylinSane &instance = KylinSane::getInstance();
    bool retStatus = instance.getKylinSaneStatus();
    if (!retStatus)
        return 0;

#ifdef DEBUG_EDIT
    return 0;
#endif

    QFileInfo fileInfo(pathName);
    if (! fileInfo.exists()) // Not exists, so save it
        return 1;

    QMessageBox::StandardButton msgBox;
    QString msg = pathName + tr(" already exist, do you want to overwrite it?");
    msgBox = QMessageBox::question(0, "Question", msg);

    if (msgBox == QMessageBox::Yes)
        return 1;
    else
        return 0;
}

void Widget::saveImage(QString fileName)
{
    qDebug() << "Save filename: " << fileName;
    QImage *img = NULL;
    img = pScandisplay->imageSave(fileName);
    if (img)
        saveToPdf(*img,fileName);
}

/**
 * @brief Widget::setScanSetBtnEnable
 * 设置发邮件和另存为控件可用
 */
void Widget::setScanSetBtnEnable(bool ret)
{
    if (!ret)
        return;

    pScanSet->setKylinScanSetBtnEnable();
    pScanSet->setBtnSaveText();
}

/**
 * @brief Widget::save_scan_file存储为不同的格式
 */
void Widget::saveScanFile(bool ret)
{
    if (!ret)
        return;

    QImage img;

    pFuncBar->setKylinScanSetEnable();
    pFuncBar->setStackClear();
#ifdef DEBUG_EDIT
    img.load("/tmp/scanner/scan.pnm");
#else
    img.load("/tmp/scanner/scan.pnm");
#endif
    QString pathName = pScanSet->getTextLocation() + "/" + pScanSet->getTextName();
    qDebug() <<"pathName:"<<pathName;
    QString format = pScanSet->getTextFormat();

    if ((format == "jpg") || (format == "png") || (format == "bmp"))
    {
        QString newformat = "." + format;
        qDebug() <<"newformat:"<<newformat;
        if (pathName.endsWith(newformat,Qt::CaseSensitive))
        {
            qDebug() <<"pathName:"<<pathName;
            if (messageScanFinishedSave(pathName))
            {
                QFile::remove(pathName);
                img.save(pathName);
            } else return;
        }
        else
        {
            pathName += newformat;
            qDebug() <<"pathName:"<<pathName;
            if (messageScanFinishedSave(pathName))
            {
                QFile::remove(pathName);
                img.save(pathName);
            } else return;
        }
    }
    else if (format == "pdf")
    {
        QString newformat = "." + format;
        qDebug() <<"newformat:"<<newformat;
        if (!pathName.endsWith(newformat,Qt::CaseSensitive))
        {
            pathName += newformat;
            qDebug() <<"pathName:"<<pathName;
        }

        if (messageScanFinishedSave(pathName))
        {
            QFile::remove(pathName);
            saveToPdf(img,pathName);
        } else return;
    }
}

/**
 * @brief Widget::scan_result 处理初始化设备的控件可用情况
 * 当ret为true时，表示在处理线程中连接上了设备，此时扫描设置等按钮可用，
 * 否则，按钮不可用。
 *
 * @param ret 线程的处理结果
 */
void Widget::scanResult(bool ret)
{
    qDebug() <<"ret = " << ret;
    KylinSane &instance = KylinSane::getInstance();

#ifdef DEBUG_EDIT
    {
        device = true;

        pScanSet->setKylinComboBoxScanDeviceName();
        instance.openScanDevice(0);

        bool retStatus = instance.getKylinSaneStatus();
        resultDetail(retStatus);
    }
#else
    if (ret)
    {
        device = true;

        pScanSet->setKylinComboBoxScanDeviceName();
        instance.openScanDevice(0);

        bool retStatus = instance.getKylinSaneStatus();
        resultDetail(retStatus);
    }
    else
    {
        device = false;
        pScandisplay->setNoDevice();
        pFuncBar->setKylinScanSetNotEnable();
        pScanSet->setKylinScanSetNotEnable();
    }
#endif
}

/**
 * @brief Widget::scan_result_detail 设置扫描信息可见情况
 * 初始化时，应该根据获取的扫描信息进行设置textDevice，
 * 此时不应该重新设置该字段，所以应该为setKylinComboBox 的参数为 true 进行跳过设置该字段
 * @param ret
 */
void Widget::scanResultDetail(bool ret)
{
    qDebug() << "ret = " << ret;

#ifdef DEBUG_EDIT
    {
        device = true;
        pScandisplay->setInitDevice();
        pScanSet->setKylinComboBox(true);
        pScanSet->setKylinLable();
        pFuncBar->setBtnScanEnable();
        pScanSet->setKylinScanSetEnable();
    }
#else
    if (ret)
    {
        device = true;
        pScandisplay->setInitDevice();
        pScanSet->setKylinComboBox(true);
        pScanSet->setKylinLable();
        pFuncBar->setBtnScanEnable();
        pScanSet->setKylinScanSetEnable();
    }
    else
    {
        device = false;
        pScandisplay->setNoDevice();
        pFuncBar->setKylinScanSetNotEnable();
        pScanSet->setKylinScanSetNotEnable();
    }
#endif
}

void Widget::scanningResultDetail(bool ret)
{
    qDebug() << "ret = " << ret;

#ifdef DEBUG_EDIT
    {
        device = true;
        pScandisplay->setInitDevice();
        pScanSet->setKylinComboBox(true);
        pScanSet->setKylinLable();
        pFuncBar->setBtnScanEnable();
        pScanSet->setKylinScanSetEnable();
    }
#else
    if (!ret)
    {
        device = true;
        pScandisplay->setInitDevice();
        pScanSet->setKylinComboBox(true);
        pScanSet->setKylinLable();
        pFuncBar->setBtnScanEnable();
        pScanSet->setKylinScanSetEnable();
    }
#endif
}

void Widget::setMaskClear()
{
    clearMask();
    pScandisplay->updateWindowSize();

    pTitleBar->setMainWindowAttribute(this->width(), this->height());
}

/**
 * @brief Widget::set_mask 设置窗口圆角
 */
void Widget::setWindowBorderRadius()
{
    clearMask();
    pScandisplay->updateWindowSize();

    pTitleBar->setMainWindowAttribute(this->width(), this->height());
}

void Widget::style_changed(QString)
{
    qDebug() << "style_changed";
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        // 黑色主题或默认主题
        qDebug() << "ukui-black";

        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(47, 44, 43));
        setAutoFillBackground(true);
        setPalette(pal);

        line->setStyleSheet("QFrame{color:#201E1D}");

    } else {
        // 白色主题
        qDebug() << "ukui-white";
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(255, 255, 255));
        setAutoFillBackground(true);
        setPalette(pal);

        line->setStyleSheet("QFrame{color:#DCDCDC}");
    }
    if (device)
    {
        //pScanSet->setKylinComboBox(false);
        pScanSet->setKylinLable();
        pFuncBar->setBtnScanEnable();
        pScanSet->setKylinScanSetEnable();
    } else {
        pScanSet->setKylinScanSetNotEnable();
        pFuncBar->setKylinScanSetNotEnable();
    }
}

void Widget::icon_theme_changed(QString)
{
    qDebug() << "icon_theme_changed";
    if (iconthemelist.contains (icon_theme_settings->get(ICON_THEME_NAME).toString())) {
        qDebug() << "icon-theme: " << icon_theme_settings->get(ICON_THEME_NAME).toString();
        //setWindowIcon (QIcon::fromTheme("kylin-scanner"));
    } else {
        qDebug() << "default eeed";
    }
}

void CommonScanThread::run()
{
    KylinSane &instance = KylinSane::getInstance();
    do {
        qDebug() << "begin findScanDevice()";
        instance.findScanDevice();

        //instance.open_device(0);

        if (instance.getKylinSaneStatus() == false)
        {
            emit scanFinished(false);
            qDebug() << "scan finished!";
        }
        else
        {
            emit scanFinished(true);
        }
    } while (!instance.getKylinSaneStatus ());

    quit();
}
