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
#include <QProcess>

bool device = false;

KYCWidget::KYCWidget(QWidget *parent)
    : QWidget(parent)
    , style_settings (new QGSettings(ORG_UKUI_STYLE))
    , icon_theme_settings (new QGSettings(ORG_UKUI_STYLE))
    , pTitleBar (new KYCTitleBarDialog())
    , line (new QFrame())
    , pFuncBar (new  KYCFunctionBarWidget())
    , pScanSet (new KYCScanSettingsWidget())
    , pScandisplay (new KYCScanDisplayWidget())
    , pHboxLayout (new QHBoxLayout())
    , pLayout (new QVBoxLayout())
{
    setWindowTitle (tr("kylin-scanner")); // For system tray text
    setWindowIcon (QIcon::fromTheme("kylin-scanner"));

    //stylelist << STYLE_NAME_KEY_DARK << STYLE_NAME_KEY_BLACK << STYLE_NAME_KEY_DEFAULT;
    stylelist << STYLE_NAME_KEY_DARK << STYLE_NAME_KEY_BLACK;
    iconthemelist << ICON_THEME_KEY_BASIC << ICON_THEME_KEY_CLASSICAL << ICON_THEME_KEY_DEFAULT;

#ifdef DEBUG_EDIT
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    instance.setKylinSaneStatus (true);
#else
    thread.start();
    usbThread.start();
#endif

    resize(MAINWINDOW_WIDTH, MAINWINDOW_HEIGHT);
    pTitleBar->setMainWindowAttribute(this->width(), this->height());

    line->setObjectName(QString::fromUtf8("line"));
    line->setGeometry(QRect(0, 32, 860, 1));
    line->setFrameShape(QFrame::HLine);
    line->hide ();

#ifdef DEBUG_EDIT
#else
    pScanSet->setKylinScanSetNotEnable();
#endif

    qDebug() << "1 styleName = " << style_settings->get(STYLE_NAME).toString();
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        // Set window background
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

    pHboxLayout->setSpacing(0); // Need init setSpacing()，or addSpacing() is incorrect.
    pHboxLayout->addWidget(pScanSet);
    pHboxLayout->addWidget(pScandisplay);
    pHboxLayout->setContentsMargins(0, 0, 0, 0);

    pLayout->setSpacing(0);
    pLayout->addWidget(pTitleBar);
    pLayout->addWidget(line);
    pLayout->addWidget(pFuncBar);
    pLayout->addLayout(pHboxLayout);
    pLayout->setContentsMargins(0, 0, 0, 0);

    //setWindowBorderRadius();
    setLayout(pLayout);

    initConnect();
}

KYCWidget::~KYCWidget()
{

}

void KYCWidget::initConnect()
{
    // For window size change
    connect(pTitleBar, &KYCTitleBarDialog::isNormal, this, &KYCWidget::setWindowBorderRadius);
    connect(pTitleBar, &KYCTitleBarDialog::isMax, this, &KYCWidget::setMaskClear);

    // For white and black style
    connect(style_settings, SIGNAL(changed(QString)), this, SLOT(style_changed(QString)));

    // For icon theme change style
    connect(icon_theme_settings, SIGNAL(changed(QString)), this, SLOT(icon_theme_changed(QString)));

    // For save
    connect(pScanSet, &KYCScanSettingsWidget::saveImageSignal, this, &KYCWidget::saveImage);

    // Display all edit picture after scan end
    // connect(pFuncBar,&FuncBar::clickBtnScanEnd, pScandisplay, &ScanDisplay::switchPage);

    // For initiating scan
    connect(&thread, SIGNAL(initFindScanDevicesFinished(bool)), this, SLOT(scanResult(bool)));

    // For usb device
    connect(&usbThread, SIGNAL(usbRemove(QString)), this, SLOT(usbDeviceRemoved(QString)));
    connect(&usbThread, SIGNAL(usbAdd(QString)), this, SLOT(usbDeviceAdded(QString)));

    // In case of switching scanner
    connect(pScanSet, SIGNAL(openDeviceStatusSignal(bool)), this, SLOT(switchScanDeviceResult(bool)));

    // For send mail: send present pictures
    connect(pScanSet, &KYCScanSettingsWidget::sendMailSignal, pScandisplay, &KYCScanDisplayWidget::onSaveImageNow);

    // For scanning
    connect(pFuncBar, &KYCFunctionBarWidget::clickBtnScanStart, this, &KYCWidget::setOcrFlags);
    connect(pFuncBar, SIGNAL(clickBtnScanEnd(bool)), this, SLOT(setScanEndOperation(bool)));

    // For rectify
    connect(pFuncBar, &KYCFunctionBarWidget::sendRectifyBegin, pScandisplay, &KYCScanDisplayWidget::onRectify);
    connect(pFuncBar, &KYCFunctionBarWidget::sendRectifyEnd, pScandisplay, &KYCScanDisplayWidget::onRectify);

    // For beauty
    connect(pFuncBar, &KYCFunctionBarWidget::sendBeautifyBegin, pScandisplay, &KYCScanDisplayWidget::onBeautify);
    connect(pFuncBar, &KYCFunctionBarWidget::sendBeautifyEnd, pScandisplay, &KYCScanDisplayWidget::onBeautify);

    // For OCR
    connect(pFuncBar, &KYCFunctionBarWidget::sendOcrBegin, this, &KYCWidget::setOcrBeginOperation);
    connect(pFuncBar, &KYCFunctionBarWidget::sendOcrEnd, this, &KYCWidget::setOcrEndOperation);
}

int toUnicode(QString str)
{
    char  *ch;
    QByteArray ba = str.toLatin1();
    ch = ba.data();
    return ch[0] + (ch[1] ? toUnicode(ch + 1) : 0);
}

constexpr inline int U(const char *str)
{
    return str[0] + (str[1] ? U(str + 1) : 0);
}

void KYCWidget::setPdfSize(QPdfWriter *pdfWriter, QString size)
{
    switch (toUnicode(size)) {
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

void KYCWidget::saveToPdf(QImage img, QString pathName)
{

    QFile pdfFile(pathName);
    pdfFile.open(QIODevice::WriteOnly);
    QPdfWriter *pdfWriter = new QPdfWriter(&pdfFile);
    setPdfSize(pdfWriter, pScanSet->getTextSize());
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
void KYCWidget::resultDetail(bool ret)
{
#ifdef DEBUG_EDIT
    ret = true;
#endif

    if (ret) {
        device = true;
        pScanSet->setKylinComboBox(false);
        pScanSet->setKylinLable();
        pFuncBar->setBtnScanEnable();
        pScanSet->setKylinScanSetEnable();
    } else {
        device = false;
        pScandisplay->setNoDevice();
        pFuncBar->setKylinScanSetNotEnable();
        pScanSet->setKylinScanSetNotEnable();
    }
}

int KYCWidget::messageScanFinishedSave(QString pathName)
{
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
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
    msgBox = QMessageBox::question(0, tr("Question"), msg);

    if (msgBox == QMessageBox::Yes)
        return 1;
    else
        return 0;
}

void KYCWidget::warnMsg(QString msg)
{
    QMessageBox msgBox(QMessageBox::Warning, QObject::tr("warning"), msg);
    msgBox.setWindowIcon(QIcon::fromTheme("kylin-scanner"));
    msgBox.exec();
}

/**
 * @brief Widget::getScannerPath
 * Get scanner path from (add|remove)@/devices/pci0000:00/0000:00:11.0/0000:02:03.0/usb1/1-1/1-2:1.0
 * to /sys/devices/pci0000:00/0000:00:11.0/0000:02:03.0/usb1/1-1/1-2:1.0
 * @param str
 * @return
 */
QString KYCWidget::getScannerPath(QString str)
{
    QStringList list = str.split("@");
    //path = /devices/pci0000:00/0000:00:11.0/0000:02:03.0/usb1/1-1/1-2:1.0
    QString path = list[1];
    QString realPath = "/sys" + path;
    qDebug() << "path = " << path
             << "realPath = " << realPath;
    return realPath;
}

void KYCWidget::saveImage(QString fileName)
{
    qDebug() << "Save filename: " << fileName;
    QImage *img = NULL;
    img = pScandisplay->imageSave(fileName);
    if (img)
        saveToPdf(*img, fileName);
}

/**
 * @brief Widget::setScanSetBtnEnable
 * 设置发邮件和另存为控件可用
 */
void KYCWidget::setScanSetBtnEnable(bool ret)
{
    if (!ret)
        return;

    pScanSet->setKylinScanSetBtnEnable();
    pScanSet->setBtnSaveText();

    // 打开扫描设备进行扫描成功
    device = true;
    //pScandisplay->setInitDevice();

    // Do not change scan color mode while scan again
    pScanSet->setKylinLable();
    pFuncBar->setBtnScanEnable();
    pScanSet->setKylinScanSetEnable();
}

/**
 * @brief Widget::save_scan_file存储为不同的格式
 */
void KYCWidget::saveScanFile(bool ret)
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
    qDebug() << "pathName:" << pathName;
    QString format = pScanSet->getTextFormat();

    if ((format == "jpg") || (format == "png") || (format == "bmp")) {
        QString newformat = "." + format;
        qDebug() << "newformat:" << newformat;
        if (pathName.endsWith(newformat, Qt::CaseSensitive)) {
            qDebug() << "pathName:" << pathName;
            if (0 == messageScanFinishedSave(pathName)) {
                return;
            }
            QFile::remove(pathName);
            img.save(pathName);
        } else {
            pathName += newformat;
            qDebug() << "pathName:" << pathName;
            if (0 == messageScanFinishedSave(pathName)) {
                return;
            }
            QFile::remove(pathName);
            img.save(pathName);
        }
    } else if (format == "pdf") {
        QString newformat = "." + format;
        qDebug() << "newformat:" << newformat;
        if (!pathName.endsWith(newformat, Qt::CaseSensitive)) {
            pathName += newformat;
            qDebug() << "pathName:" << pathName;
        }

        if (0 == messageScanFinishedSave(pathName)) {
            return;
        }
        QFile::remove(pathName);
        saveToPdf(img, pathName);
    }
}

/**
 * @brief Widget::scan_result 处理初始化设备的控件可用情况
 * 当ret为true时，表示在处理线程中连接上了设备，此时扫描设置等按钮可用，
 * 否则，按钮不可用。
 *
 * @param ret 线程的处理结果
 */
void KYCWidget::scanResult(bool ret)
{
    qDebug() << "ret = " << ret;
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();

#ifdef DEBUG_EDIT
    {
        device = true;

        pScanSet->setKylinComboBoxScanDeviceName();
        //resultDetail(true);
    }
#else
    if (ret) {
        device = true;

        pScanSet->setKylinComboBoxScanDeviceName();
        instance.openScanDevice(0);

        bool retStatus = instance.getKylinSaneStatus();
        resultDetail(retStatus);

        pScanSet->setFlagTextDeviceChangedWork();
    } else {
        device = false;
        // free resource for sane devices
        //instance.saneExit();
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
void KYCWidget::switchScanDeviceResult(bool ret)
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
    if (ret) {
        device = true;
        pScandisplay->setInitDevice();
        pScanSet->setKylinComboBox(true);
        pScanSet->setKylinLable();
        pFuncBar->setBtnScanEnable();
        pScanSet->setKylinScanSetEnable();
    } else {
        device = false;
        pScandisplay->setNoDevice();
        pFuncBar->setKylinScanSetNotEnable();
        pScanSet->setKylinScanSetNotEnable();
    }
#endif
}

void KYCWidget::scanningResultDetail(bool ret)
{
    qDebug() << "scanningResultDetail ret = " << ret;

#ifdef DEBUG_EDIT
    {
        device = true;
        pScandisplay->setInitDevice();
        //pScanSet->setKylinComboBox(true);
        //pScanSet->setKylinLable();
        //pFuncBar->setBtnScanEnable();
        //pScanSet->setKylinScanSetEnable();
    }
#else
    if (!ret) {
        // 可以查找到扫描设备，但打开扫描设备进行扫描失败
        device = false;
        pScandisplay->setNoDevice();
        // scan error, so set scan statu false
        pScanSet->setkylinScanStatus(false);
        pFuncBar->setKylinScanSetNotEnable();
        pScanSet->setKylinScanSetNotEnable();
        QString msg;
        msg = tr("scan process is failed, please check your scanner by connect it again.");
        warnMsg(msg);
    }
#endif
}

void KYCWidget::sendMailPrepare()
{
    pScandisplay->initSavePresentImage();
}

void KYCWidget::setUsbThreadQuit()
{
    usbThread.quit();
}

void KYCWidget::setMaskClear()
{
    clearMask();
    pScandisplay->updateWindowSize();

    pTitleBar->setMainWindowAttribute(this->width(), this->height());
}

/**
 * @brief Widget::set_mask 设置窗口圆角
 */
void KYCWidget::setWindowBorderRadius()
{
    clearMask();
    pScandisplay->updateWindowSize();

    pTitleBar->setMainWindowAttribute(this->width(), this->height());
}

void KYCWidget::style_changed(QString)
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
    if (device) {
        pScanSet->setKylinLable();
        pFuncBar->setBtnScanEnable();
        pScanSet->setKylinScanSetEnable();
    } else {
        pScanSet->setKylinScanSetNotEnable();
        pFuncBar->setKylinScanSetNotEnable();
    }
}

void KYCWidget::icon_theme_changed(QString)
{
    qDebug() << "icon_theme_changed";
    if (iconthemelist.contains (icon_theme_settings->get(ICON_THEME_NAME).toString())) {
        qDebug() << "icon-theme: " << icon_theme_settings->get(ICON_THEME_NAME).toString();
        //setWindowIcon (QIcon::fromTheme("kylin-scanner"));
    } else {
        qDebug() << "default eeed";
    }
}

void KYCWidget::usbDeviceAdded(QString recvData)
{
    qDebug() << "USB Add: " << recvData;
}

void KYCWidget::usbDeviceRemoved(QString recvData)
{
    qDebug() << "USB Remove: " << recvData;
    QProcess *scanList = new QProcess(this);
    QStringList  argvList;
    argvList.append("-L");
    scanList->start("scanimage", argvList);
    //connect(scanList, SIGNAL(finished(int), this,  SLOT(scanListResult(int));
    connect(scanList, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
    [ = ](int exitCode, QProcess::ExitStatus exitStatus) {
        qDebug() << "USB Remove exitCode = " << exitCode
                 << "exitStatus = " << exitStatus;
        if (0 == exitCode) {
            //QProcess *pProces = (QProcess *)sender();
            QString result = scanList->readAll();
            qDebug() << "result = " << result;
            KYCSaneWidget &instance = KYCSaneWidget::getInstance();
            QStringList strListDevice;
            strListDevice = instance.getKylinSaneNames();
            qDebug() << "current sane names: " << strListDevice
                     << "userInfo.name= " << instance.userInfo.name
                     << "size = " << strListDevice.size();
            for (int i = 0; i < strListDevice.size(); ++i) {
                QString str = strListDevice.at(i).toLocal8Bit().constData();
                qDebug() << "i=" << i << "str = " << str;
                if (str == tr("No available device"))
                    break;
                // There are two cases that we cannot find scanners throught `scanimage -L`:
                // case 1. no this scanner in system, which means this scanner has been disconnect
                // case 2. this scanner has been connect in kylin-scanner by sane_init
                if (!result.contains(str, Qt::CaseInsensitive)) {
                    QString msg;
                    bool retStatus;
                    msg = tr("device ") + str + tr(" has been disconnect.");
                    //warnMsg(msg);
                    if (instance.userInfo.name == str) {
                        qDebug() << "The user choose device: " << str << "has been disconnect!";
                        // get argument again, because it cannot search using scanner while has opened it

                        instance.openScanDevice(i);
                        QString deviceName = instance.getKylinSaneOpenName();
                        msg = tr("device ") + deviceName + tr(" has been disconnect.");
                        retStatus = instance.getKylinSaneStatus();

                        qDebug() << "test scanning end, status = " << retStatus;

                        if (!retStatus) {
                            warnMsg(msg);
                            resultDetail(retStatus);
                        }
                    }
                }
            }
        }
    });
}

void KYCWidget::scanListResult(int ret)
{
    if (0 == ret) {
        qDebug() << "test";
    }
}

/**
 * @brief KYCWidget::setScanEndOperation
 * Operations after scan end.
 * Forbid call sane api error after click btnScan
 * @param retScan
 * scan finished status
 */
void KYCWidget::setScanEndOperation(bool retScan)
{
    pScandisplay->onScan(retScan);
    setScanSetBtnEnable(retScan);
    saveScanFile(retScan);
    scanningResultDetail(retScan);
    sendMailPrepare();
}

void KYCWidget::setOcrFlags()
{
    pScandisplay->setOcrFlagStatus();
    pScanSet->setOcrFlagInit();
}

void KYCWidget::setOcrBeginOperation()
{
    pScandisplay->onOcr();
    pScanSet->modifyBtnSave();
}

void KYCWidget::setOcrEndOperation()
{
    pScandisplay->onOcr();
    pScanSet->modifyBtnSave();
}

/**
 * @brief KYCCommonScanThread::run
 * Put findScanDevice in thread, because it will take a long time
 */
void KYCCommonScanThread::run()
{
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    qDebug() << "get sane status: " << instance.getKylinSaneStatus();
    do {
        qDebug() << "begin findScanDevice()";
        instance.findScanDevice();

        //instance.open_device(0);

        if (!instance.getKylinSaneStatus()) {
            emit initFindScanDevicesFinished(false);
            qDebug() << "scan finished!";
        } else {
            emit initFindScanDevicesFinished(true);
        }
    } while (!instance.getKylinSaneStatus ());

    quit();
}
