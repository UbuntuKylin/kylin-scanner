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
#include "widget.h"
#include <QVBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QSplitter>
#include <QLine>
#include <QFrame>
#include <QDebug>
#include <QBitmap>
#include "kylin_sane.h"

bool device = true;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());

    thread.start();

    pTitleBar = new TitleBar(this);
    installEventFilter(pTitleBar);
    resize(860, 680);

//    setStyleSheet("QWidget{border-bottom-left-radius:5px;border-bottom-right-radius:5px;}");
    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(47, 44, 43));
    setAutoFillBackground(true);
    setPalette(pal);

    line =  new QFrame();
    line->setObjectName(QString::fromUtf8("line"));
    line->setGeometry(QRect(0, 32, 860, 1));
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("QFrame{color:rgb(32,30,29)}");


    pFuncBar = new  FuncBar();
    installEventFilter(pFuncBar);
    pScanSet = new ScanSet() ;
    installEventFilter(pScanSet);
    pScandisplay = new scan_display();

    pHboxLayout = new QHBoxLayout();
    pHboxLayout->setSpacing(0);
    pHboxLayout->addWidget(pScanSet);
    pHboxLayout->addWidget(pScandisplay);
    pHboxLayout->setContentsMargins(0,0,0,0);

    pLayout = new QVBoxLayout();
    pLayout->setSpacing(0);
    pLayout->addWidget(pTitleBar);
    pLayout->addWidget(line);
    pLayout->addWidget(pFuncBar);
    pLayout->addLayout(pHboxLayout);
    pLayout->setContentsMargins(0, 0, 0, 0);

    set_mask();
    setLayout(pLayout);

    // For save
    connect(pScanSet,&ScanSet::save_image_signal,this,&Widget::save_image);

    // For ORC
    connect(pFuncBar,&FuncBar::send_Orc_Begin,pScandisplay,&scan_display::orc);
    connect(pFuncBar,&FuncBar::send_Orc_End,pScandisplay,&scan_display::orc);
    connect(pFuncBar,&FuncBar::send_Orc_Begin,pScanSet,&ScanSet::modify_save_button);
    connect(pFuncBar,&FuncBar::send_Orc_End,pScanSet,&ScanSet::modify_save_button);

    // For scan
    connect(&thread,SIGNAL(scan_finished(bool)),this,SLOT(scan_result(bool)));
    connect(pScanSet, SIGNAL(open_device_status(bool)), this, SLOT(scan_result_detail(bool)));
    connect(pFuncBar,&FuncBar::send_Scan_End,pScandisplay,&scan_display::scan);
    connect(pFuncBar,&FuncBar::send_Scan_End,this,&Widget::save_scan_file);

    // For rectify
    connect(pFuncBar,&FuncBar::send_Rectify_Begin,pScandisplay,&scan_display::rectify);
    connect(pFuncBar,&FuncBar::send_Rectify_End,pScandisplay,&scan_display::rectify);

    // For beauty
    connect(pFuncBar, &FuncBar::send_Beautify_Begin, pScandisplay, &scan_display::beautify);
    connect(pFuncBar, &FuncBar::send_Beautify_End, pScandisplay, &scan_display::beautify);
    connect(pTitleBar,&TitleBar::isNormal,this,&Widget::set_mask);
    connect(pTitleBar,&TitleBar::isMax,this,&Widget::set_mask_clear);
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

void Widget::set_pdf_size(QPdfWriter *pdfWriter, QString size)
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

void Widget::save_to_pdf(QImage img, QString pathName)
{

    QFile pdfFile(pathName);
    pdfFile.open(QIODevice::WriteOnly);
    QPdfWriter *pdfWriter = new QPdfWriter(&pdfFile);
    set_pdf_size(pdfWriter,pScanSet->getTextSize());
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
void Widget::result_detail(bool ret)
{
    qDebug()<<"result_detail"<<endl;

    if(ret)
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
        pScandisplay->set_no_device();
        pFuncBar->setKylinScanSetNotEnable();
        pScanSet->setKylinScanSetNotEnable();
    }


}

void Widget::save_image(QString fileName)
{
    QImage *img = NULL;
    img = pScandisplay->imageSave(fileName);
    if(img)
        save_to_pdf(*img,fileName);
}

/**
 * @brief Widget::save_scan_file存储为不同的格式
 */
void Widget::save_scan_file()
{
    QImage img;

    pFuncBar->setKylinScanSetEnable();
    pFuncBar->setStackClear();
    img.load("/tmp/scanner/scan.pnm");
    QString pathName = pScanSet->getTextLocation() + "/" + pScanSet->getTextName();
    qDebug()<<"pathName:"<<pathName;
    QString format = pScanSet->getTextFormat();
    if ((format == "jpg") || (format == "png") || (format == "bmp"))
    {
        QString newformat = "." + format;
        qDebug()<<"newformat:"<<newformat;
        if (pathName.endsWith(newformat,Qt::CaseSensitive))
        {
            img.save(pathName);
        }
        else
        {
            pathName += newformat;
            qDebug()<<"pathName:"<<pathName;
            img.save(pathName);
        }
    }
    else if (format == "pdf")
    {
        QString newformat = "." + format;
        qDebug()<<"newformat:"<<newformat;
        if (!pathName.endsWith(newformat,Qt::CaseSensitive))
        {
            pathName += newformat;
            qDebug()<<"pathName:"<<pathName;
        }
        save_to_pdf(img,pathName);
    }
}

/**
 * @brief Widget::scan_result 处理初始化设备的控件可用情况
 * 当ret为true时，表示在处理线程中连接上了设备，此时扫描设置等按钮可用，
 * 否则，按钮不可用。
 *
 * @param ret 线程的处理结果
 */
void Widget::scan_result(bool ret)
{
    qDebug()<<"scan_result"<<endl;
    KylinSane &instance = KylinSane::getInstance();

    if(ret)
    {
        device = true;

        pScanSet->setKylinComboBoxScanDeviceName();
        instance.open_device(0);

        bool ret = instance.getKylinSaneStatus();
        result_detail(ret);

        /*
        pScanSet->setKylinComboBox();
        pScanSet->setKylinLable();
        pFuncBar->setBtnScanEnable();
        pScanSet->setKylinScanSetEnable();
        */
    }
    else
    {
        device = false;
        pScandisplay->set_no_device();
        pFuncBar->setKylinScanSetNotEnable();
        pScanSet->setKylinScanSetNotEnable();
    }

}

/**
 * @brief Widget::scan_result_detail 设置扫描信息可见情况
 * 初始化时，应该根据获取的扫描信息进行设置textDevice，
 * 此时不应该重新设置该字段，所以应该为setKylinComboBox 的参数为 true 进行跳过设置该字段
 * @param ret
 */
void Widget::scan_result_detail(bool ret)
{
    qDebug()<<"scan_result_detail"<<endl;

    if(ret)
    {
        device = true;
        pScandisplay->set_init_device();
        pScanSet->setKylinComboBox(true);
        pScanSet->setKylinLable();
        pFuncBar->setBtnScanEnable();
        pScanSet->setKylinScanSetEnable();
    }
    else
    {
        device = false;
        pScandisplay->set_no_device();
        pFuncBar->setKylinScanSetNotEnable();
        pScanSet->setKylinScanSetNotEnable();
    }

}

void Widget::set_mask_clear()
{
    clearMask();

}
void Widget::set_mask()
{
        clearMask();
        QBitmap bitMap(860,680); // A bit map has the same size with current widget

        bitMap.fill();

        QPainter painter(&bitMap);

        painter.setBrush(Qt::black);

        painter.setPen(Qt::NoPen); // Any color that is not QRgb(0,0,0) is right

        painter.setRenderHint(QPainter::Antialiasing, true);

        painter.drawRoundedRect(bitMap.rect(),6,6); //设置圆角弧度

        setMask(bitMap);
}

void scanThread::run()
{
    KylinSane &instance = KylinSane::getInstance();
again:
    do {
        instance.find_device();

        //instance.open_device(0);

        //qDebug() << instance.getKylinSaneResolutions();
        if(instance.getKylinSaneStatus() == false)
        {
            emit scan_finished(false);
            qDebug() << "scan finished!";
        }
        else
        {
            emit scan_finished(true);
        }
    } while (!instance.getKylinSaneStatus());

    quit();
}
