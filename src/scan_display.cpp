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
#include "scan_display.h"
#include "func_bar.h"
#include <QDebug>
#include <QToolTip>
#include <QTextEdit>
#include "kylin_sane.h"
QString outText;
scan_display::scan_display(QWidget *parent)
    : QWidget(parent)
{

    setMinimumSize(600,567);
    labInit = new QLabel();
    labConnectError = new QLabel();
    labConnectErrorText = new QLabel();
    labNormalLeft = new QLabel();
    labNormalRight = new QLabel();
    labEditLayout = new QLabel();

    btnNormal = new QPushButton();
    btnEditLayout = new QPushButton();

    widgetConnectError = new QWidget();
    widgetNormal = new QWidget();
    widgetEditLayout = new QWidget();

    imgConnectError = new QImage();
    imgEditLayout = new QImage();
    imgStack = new QImage();
    imgNormal = new QImage();
    imgBeautify = new QImage();
    imgRectify = new QImage();
    vBoxConnectError = new QVBoxLayout();
    vBoxScanSet = new QVBoxLayout(this);
    hBoxNormal = new QHBoxLayout();
    hBoxEditLayout = new QHBoxLayout();

    vStackedLayout = new QStackedLayout();

    labConnectError->setParent(widgetConnectError);
    labConnectErrorText->setParent(widgetConnectError);
    labInit->setMinimumSize(600,567);
    labConnectError->setMinimumSize(600,320);
    labConnectErrorText->setMinimumSize(600,231);
    labNormalLeft->setMinimumSize(360,490);
    labNormalRight->setFixedWidth(40);
    labEditLayout->setMinimumSize(360,490);


    labInit->setText(" ");


    imgConnectError->load(":/icon/icon/connect_device.png");
    labConnectError->setPixmap(QPixmap::fromImage(*imgConnectError));
    labConnectError->setAlignment(Qt::AlignHCenter|Qt::AlignBottom);


    QFont ft;
    ft.setPointSize(24);
    labConnectErrorText->setFont(ft);
    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(232,232,232));
    labConnectErrorText->setPalette(pa);
    labConnectErrorText->setText(tr("Please connect to a scan device firstly !"));
    labConnectErrorText->setAlignment(Qt::AlignHCenter|Qt::AlignTop);

    vBoxConnectError->setSpacing(0);
    vBoxConnectError->addStretch();
    vBoxConnectError->addWidget(labConnectError);
    vBoxConnectError->addSpacing(16);
    vBoxConnectError->addWidget(labConnectErrorText);
    vBoxConnectError->addStretch();
    vBoxConnectError->setContentsMargins(0,0,0,0);
    widgetConnectError->setLayout(vBoxConnectError);

    labNormalLeft->setParent(widgetNormal);
    labNormalRight->setParent(widgetNormal);
    btnNormal->setParent(widgetNormal);
    btnNormal->setFixedSize(12,30);
    btnNormal->setStyleSheet("QPushButton{border-image: url(:/icon/icon/toolbutton.png);border:none;background-color:#0f0801;border-radius:0px;}");

    labNormalLeft->setAlignment(Qt::AlignCenter);
    labNormalRight->setStyleSheet("QLabel{background-color:#0f0801;}");

    hBoxNormal->setSpacing(0);
    hBoxNormal->addSpacing(93);
    hBoxNormal->addStretch();
    hBoxNormal->addWidget(labNormalLeft);
    hBoxNormal->addStretch();
    hBoxNormal->addSpacing(93);
    hBoxNormal->addWidget(labNormalRight);
    hBoxNormal->addSpacing(2);
    hBoxNormal->addWidget(btnNormal);
    hBoxNormal->setContentsMargins(0,45,0,32);
    widgetNormal->setLayout(hBoxNormal);


    editLayout = new edit_bar();
    labEditLayout->setParent(widgetEditLayout);
    btnEditLayout->setParent(widgetEditLayout);
    editLayout->setParent(widgetEditLayout);


    labEditLayout->setAlignment(Qt::AlignCenter);


    btnEditLayout->setFixedSize(12,30);
    btnEditLayout->setStyleSheet("QPushButton{border-image: url(:/icon/icon/toolbutton1.png);border:none;background-color:#0f0801;border-radius:0px;}");

    hBoxEditLayout->setSpacing(0);
    hBoxEditLayout->addSpacing(93);
    hBoxEditLayout->addStretch();
    hBoxEditLayout->addWidget(labEditLayout);
    hBoxEditLayout->addStretch();
    hBoxEditLayout->addSpacing(93);
    hBoxEditLayout->addWidget(editLayout);
    hBoxEditLayout->addSpacing(2);
    hBoxEditLayout->addWidget(btnEditLayout);
    hBoxEditLayout->setContentsMargins(0,45,0,32);
    widgetEditLayout->setLayout(hBoxEditLayout);


    vStackedLayout->addWidget(widgetNormal);
    vStackedLayout->addWidget(widgetEditLayout);
    vStackedLayout->addWidget(widgetConnectError);
    vStackedLayout->addWidget(labInit);

    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(15, 8, 1));
    setAutoFillBackground(true);
    setPalette(pal);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    vBoxScanSet->setSpacing(0);
    vBoxScanSet->addLayout(vStackedLayout);
    vBoxScanSet->setContentsMargins(0,0,0,0);
    setLayout(vBoxScanSet);
    vStackedLayout->setCurrentWidget(labInit);
    vStackedLayout->setContentsMargins(0,0,0,0);

    // For switch page
    connect(btnNormal,SIGNAL(clicked()),this,SLOT(switchPage()));
    connect(btnEditLayout,SIGNAL(clicked()),this,SLOT(switchPage()));

    //For rotate
    connect(editLayout->btnRotate,SIGNAL(clicked()),this,SLOT(rotating()));

    // For tailor
    connect(editLayout->btnTailor,SIGNAL(clicked()),this,SLOT(tailor()));

    // For symmetry
    connect(editLayout->btnSymmetry,SIGNAL(clicked()),this,SLOT(symmetry()));

    // For watermark
    connect(editLayout->btnWatermark,SIGNAL(clicked()),this,SLOT(addmark()));

    // For ORC
    connect(&thread,SIGNAL(orcFinished()),this,SLOT(orcText()));
}

void scan_display::keyPressEvent(QKeyEvent *e)
{
    if (((e->key() == Qt::Key_Return) || (e->key() == 0x20)) && (vStackedLayout->currentWidget() == widgetTailor))
    {
        QImage newimage;
        int x1,y1,x2,y2;
        if(labTailor->getX1() <= labTailor->getX2())
        {
             x1 = labTailor->getX1() - ((labTailor->width() - imgEditLayout->width()*scaledNum) / 2);
             x2 = labTailor->getX2() - ((labTailor->width() - imgEditLayout->width()*scaledNum) / 2);
        }
        else
        {
            x1 = labTailor->getX2() - ((labTailor->width() - imgEditLayout->width()*scaledNum) / 2);
            x2 = labTailor->getX1() - ((labTailor->width() - imgEditLayout->width()*scaledNum) / 2);
        }

        if(labTailor->getY1() <= labTailor->getY2())
        {
            y1 = labTailor->getY1() - ((labTailor->height() - imgEditLayout->height()*scaledNum) / 2);
            y2 = labTailor->getY2() - ((labTailor->height() - imgEditLayout->height()*scaledNum) / 2);
        }
        else
        {
            y1 = labTailor->getY2() - ((labTailor->height() - imgEditLayout->height()*scaledNum) / 2);
            y2 = labTailor->getY1() - ((labTailor->height() - imgEditLayout->height()*scaledNum) / 2);
        }

        newimage = imgEditLayout->copy(x1/scaledNum,y1/scaledNum,(x2-x1)/scaledNum,(y2-y1)/scaledNum);
        *imgEditLayout = newimage;
        pixmap_scaled(*imgEditLayout,labEditLayout);
        *imgNormal = imgEditLayout->copy();
        pixmap_scaled(*imgNormal,labNormalLeft);
        vStackedLayout->setCurrentIndex(index);
        vStackedLayout->removeWidget(widgetTailor);

    }
    if(e->key() == Qt::Key_Z && e->modifiers() ==  Qt::ControlModifier)
    {
        if(!stack.isEmpty())
        {
            *imgEditLayout = stack.pop();
            pixmap_scaled(*imgEditLayout,labEditLayout);
            *imgNormal = imgEditLayout->copy();
            pixmap_scaled(*imgNormal,labNormalLeft);
            vStackedLayout->setCurrentIndex(index);
        }
    }
}

QImage *scan_display::imageSave(QString fileName)
{
    if(flagOrc == 0)
    {
        *imgEditLayout = imgNormal->copy();
        if(fileName.endsWith(".pdf"))
            return imgEditLayout;
        if(fileName.endsWith(".png") || fileName.endsWith(".jpg") || fileName.endsWith(".bmp"))
            imgEditLayout->save(fileName);
    }
    else {
        if(!fileName.endsWith(".txt"))
            fileName += ".txt";
        QFile file(fileName);
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        QByteArray str = outText.toUtf8();
        file.write(str);
        file.close();
    }
    return NULL;
}

void scan_display::set_no_device()
{
    vStackedLayout->setCurrentWidget(widgetConnectError);
}

void scan_display::set_init_device()
{
    vStackedLayout->setCurrentWidget(labInit);
}

void scan_display::set_pixmap(QImage img, QLabel *lab)
{
    int width = lab->width();
    int height = lab->height();
    QPixmap pixmap = QPixmap::fromImage(img);
    QPixmap fitpixmap = pixmap.scaled(width, height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);  // 按比例缩放
    lab->setPixmap(fitpixmap);
    lab->setAlignment(Qt::AlignCenter);
}

float scan_display::pixmap_scaled(QImage img, QLabel *lab)
{
//    float labWidth = lab->width();
//    float labHeight = lab->height();
    float labWidth = this->width() - 240;
    float labHeight = this->height() - 77;
    float imgWidth = img.width();
    float imgHeight = img.height();
    float num = 1;
    if((labWidth/imgWidth) <= (labHeight/imgHeight))
        num = labWidth/imgWidth;
    else
        num = labHeight/imgHeight;
    int width,height;
    width = imgWidth * num;
    height = imgHeight * num;
    QPixmap pixmap = QPixmap::fromImage(img);
    QPixmap fitpixmap = pixmap.scaled(width, height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);  // 按比例缩放
    lab->setPixmap(fitpixmap);
    lab->setAlignment(Qt::AlignCenter);
    return num;
}

void scan_display::updateWindowSize()
{

    pixmap_scaled(*imgNormal,labNormalLeft);
    pixmap_scaled(*imgEditLayout,labEditLayout);

}

void scan_display::rotating()
{

    QMatrix matrix;
    *imgStack = imgEditLayout->copy();
    stack.push(*imgStack);
    matrix.rotate(270);
    *imgEditLayout = imgEditLayout->transformed(matrix);
    pixmap_scaled(*imgEditLayout,labEditLayout);
    *imgNormal = imgEditLayout->copy();
    pixmap_scaled(*imgNormal,labNormalLeft);

}

void scan_display::symmetry()
{
    *imgStack = imgEditLayout->copy();
    stack.push(*imgStack);
    *imgEditLayout=imgEditLayout->mirrored(true,false);
    pixmap_scaled(*imgEditLayout,labEditLayout);
    *imgNormal = imgEditLayout->copy();
    pixmap_scaled(*imgNormal,labNormalLeft);
}

void scan_display::addmark()
{

    if(n == 0)
    {
        n = 1;
        imgBackup = new QImage();
        *imgBackup = imgEditLayout->copy();
    }
    QString text;
    mark_dialog *dialog = new mark_dialog(this);
    int ret=dialog->exec();// 以模态方式显示对话框，用户关闭对话框时返回 DialogCode值
    if (ret==QDialog::Accepted) //OK键被按下,对话框关闭，若设置了setAttribute(Qt::WA_DeleteOnClose)，对话框被释放，无法获得返回值
    { //OK键被按下，获取对话框上的输入，设置行数和列数
        text = dialog->get_lineedit();
        *imgStack = imgEditLayout->copy();
        stack.push(*imgStack);
        *imgEditLayout = imgBackup->copy();
        QPainter painter(imgEditLayout);
        int fontSize = 70, spacing = 20;
        QFont font("华文黑体", fontSize, QFont::Thin);
        font.setLetterSpacing(QFont::AbsoluteSpacing, spacing);
        painter.setFont(font);
        painter.setPen(QColor(1, 1, 1));
        //painter.translate(img2->width() / 2, -img2->width() / 2);//调整位置
        painter.rotate(15);

        int squareEdgeSize = imgEditLayout->width() * sin(45) + imgEditLayout->height() * sin(45);//对角线长度
        int hCount = squareEdgeSize / ((fontSize + spacing) * (text.size() + 1)) + 1;
        int x = squareEdgeSize / hCount + (fontSize + spacing) * 3;
        int y = x / 2;

        for (int i = 0; i < hCount; i++)
        {
            for (int j = 0; j < hCount * 2; j++)
            {
               painter.drawText(x * i, y * j,text);
            }
        }
        pixmap_scaled(*imgEditLayout,labEditLayout);
        *imgNormal = imgEditLayout->copy();
        pixmap_scaled(*imgNormal,labNormalLeft);
    }
    delete dialog; //删除对话框

}

void scan_display::orcText()
{
    labOrcRight->setText(outText);
    qDebug()<<outText;
}

void scan_display::orc()
{
    if(flagOrc == 0)
    {
        flagOrc = 1;
        widgetindex = vStackedLayout->currentIndex();
        labOrcLeft = new QLabel();
        labOrcRight = new QLabel();
        imgBackup = new QImage();
        vBoxOrc = new QVBoxLayout();
        hBoxOrc = new QHBoxLayout();
        widgetOrc = new QWidget();
        scrollArea = new QScrollArea();

        labOrcLeft->setFixedWidth(120);
        *imgEditLayout = imgNormal->copy();
        imgEditLayout->save("/tmp/scanner/scan1.png");
        *imgBackup = imgEditLayout->copy();
        thread.start();
        *imgBackup = imgBackup->scaled(120,166);
        QPalette palette;
        palette.setColor(QPalette::Background, QColor(192,253,123,100));
        labOrcLeft->setPalette(palette);
        labOrcLeft->setPixmap(QPixmap::fromImage(*imgBackup));
        labOrcLeft->setAlignment(Qt::AlignTop);

        outText = tr("Try to ocr ...");
        QFont ft1;
        ft1.setPointSize(14);
        labOrcRight->setFont(ft1);
        QPalette pa1;
        pa1.setColor(QPalette::WindowText,QColor(255,255,255));
        labOrcRight->setPalette(pa1);
        labOrcRight->setText(outText);
        labOrcRight->setStyleSheet("background-color:#2f2c2b;border:1px solid #717171;");
        labOrcRight->setMargin(20);
        labOrcRight->setAlignment(Qt::AlignTop);
        labOrcRight->setWordWrap(true);
        labOrcLeft->setParent(widgetOrc);
        labOrcRight->setParent(widgetOrc);
        vBoxOrc->setSpacing(0);
        vBoxOrc->addWidget(labOrcRight);
        vBoxOrc->setContentsMargins(0,0,0,0);
        scrollArea->setParent(widgetOrc);
        QWidget *widget = new QWidget();

        widget->setMinimumHeight(labOrcRight->height());
        widget->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint); // 去掉标题栏,去掉任务栏显示，窗口置顶
        widget->setLayout(vBoxOrc);
        widget->setContentsMargins(0,0,0,0);
        scrollArea->setFixedWidth(392);
        scrollArea->setMinimumHeight(503);
        scrollArea->setWidget(widget);
        scrollArea->setWidgetResizable(true);
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setStyleSheet("QScrollBar:vertical"
                                  "{"
                                  "width:6px;"
                                  "background:#2f2c2b;"
                                  "margin:0px,0px,0px,0px;"
                                  "padding-top:0px;"
                                  "padding-bottom:0px;"
                                  "padding-right:3px"
                                  "}"
                                  "QScrollBar::handle:vertical"
                                  "{"
                                  "width:6px;"
                                  "background:rgba(255,255,255,50%);"
                                  " border-radius:3px;"
                                  "min-height:20;"
                                  "}"
                                  "QScrollBar::handle:vertical:hover"
                                  "{"
                                  "width:6px;"
                                  "background:rgba(255,255,255,80%);"
                                  " border-radius:3px;"
                                  "min-height:20;"
                                  "}"
                                  "QScrollBar::add-line:vertical"
                                  "{"
                                  "height:0px;width:0px;"
                                  "}"
                                  "QScrollBar::sub-line:vertical"
                                  "{"
                                  "height:0px;width:0px;"
                                  "subcontrol-position:top;"
                                  "}"
                                  );
        hBoxOrc->setSpacing(0);
        hBoxOrc->addStretch();
        hBoxOrc->addWidget(labOrcLeft);
        hBoxOrc->addSpacing(24);
        hBoxOrc->addWidget(scrollArea);
        hBoxOrc->addStretch();
        hBoxOrc->setContentsMargins(32,32,32,32);
        widgetOrc->setLayout(hBoxOrc);
        vStackedLayout->addWidget(widgetOrc);
        vStackedLayout->setCurrentWidget(widgetOrc);
    }
    else
    {
        flagOrc = 0;
        vStackedLayout->setCurrentIndex(widgetindex);
    }
}

void scan_display::scan()
{
    vStackedLayout->setCurrentIndex(0);
    vStackedLayout->setCurrentIndex(1);
    vStackedLayout->setCurrentIndex(2);
    vStackedLayout->setCurrentIndex(3);

    stack.clear();
    list.clear();
    flagOrc = 0;
    flagRectify = 0;
    flagBeautify = 0;

    imgNormal->load("/tmp/scanner/scan.pnm");
    pixmap_scaled(*imgNormal,labNormalLeft);
    vStackedLayout->setCurrentWidget(widgetNormal);
    *imgEditLayout = imgNormal->copy();
    pixmap_scaled(*imgEditLayout,labEditLayout);
}

void scan_display::rectify()
{
    qDebug()<<"rectify\n";
    if(flagRectify == 0)
    {
        flagRectify = 1;
        if(vStackedLayout->currentWidget() == widgetNormal)
        {
            imgNormal->save("/tmp/scanner/scan1.png");
            *imgRectify = imgNormal->copy();
            list.append("Rectify");
            ImageRectify("/tmp/scanner/scan1.png");
            imgNormal->load("/tmp/scanner/scan1.png");
            pixmap_scaled(*imgNormal,labNormalLeft);
            *imgEditLayout = imgNormal->copy();
            pixmap_scaled(*imgEditLayout,labEditLayout);
        }
        else
        {
            imgEditLayout->save("/tmp/scanner/scan1.png");
            *imgRectify = imgEditLayout->copy();
            list.append("Rectify");
            ImageRectify("/tmp/scanner/scan1.png");
            imgEditLayout->load("/tmp/scanner/scan1.png");
            pixmap_scaled(*imgEditLayout,labEditLayout);
            *imgNormal = imgEditLayout->copy();
            pixmap_scaled(*imgNormal,labNormalLeft);
        }
    }
    else
    {
        flagRectify = 0;
        if(vStackedLayout->currentWidget() == widgetNormal)
        {
            *imgNormal = imgRectify->copy();
            if(list.count() == 2)
            {
                if(list[0] == "Rectify")
                {
                    list.clear();
                    imgNormal->save("/tmp/scanner/scan1.png");
                    *imgBeautify = imgNormal->copy();
                    list.append("Beautify");
                    oneClickEmbelish("/tmp/scanner/scan1.png");
                    imgNormal->load("/tmp/scanner/scan1.png");
                }
                else
                    list.removeLast();
            }
            else
                list.clear();
            pixmap_scaled(*imgNormal,labNormalLeft);
            *imgEditLayout = imgNormal->copy();
            pixmap_scaled(*imgEditLayout,labEditLayout);
        }
        else
        {
            *imgEditLayout = imgRectify->copy();
            if(list.count() == 2)
            {
                if(list[0] == "Rectify")
                {
                    list.clear();
                    imgEditLayout->save("/tmp/scanner/scan1.png");
                    *imgBeautify = imgEditLayout->copy();
                    list.append("Beautify");
                    oneClickEmbelish("/tmp/scanner/scan1.png");
                    imgEditLayout->load("/tmp/scanner/scan1.png");
                }
                else
                    list.removeLast();
            }
            else
                list.clear();
            pixmap_scaled(*imgEditLayout,labEditLayout);
            *imgNormal = imgEditLayout->copy();
            pixmap_scaled(*imgNormal,labNormalLeft);
        }
    }
}

void scan_display::beautify()
{
    qDebug() << "beauty()";
    if(flagBeautify == 0)
    {
        flagBeautify = 1;
        if(vStackedLayout->currentWidget() == widgetNormal)
        {
            imgNormal->save("/tmp/scanner/scan1.png");
            *imgBeautify = imgNormal->copy();
            list.append("Beautify");
            oneClickEmbelish("/tmp/scanner/scan1.png");
            imgNormal->load("/tmp/scanner/scan1.png");
            pixmap_scaled(*imgNormal,labNormalLeft);
            *imgEditLayout = imgNormal->copy();
            pixmap_scaled(*imgEditLayout,labEditLayout);
        }
        else
        {
            imgEditLayout->save("/tmp/scanner/scan1.png");
            *imgBeautify = imgEditLayout->copy();
            list.append("Beautify");
            oneClickEmbelish("/tmp/scanner/scan1.png");
            imgEditLayout->load("/tmp/scanner/scan1.png");
            pixmap_scaled(*imgEditLayout,labEditLayout);
            *imgNormal = imgEditLayout->copy();
            pixmap_scaled(*imgNormal,labNormalLeft);
        }
    }
    else
    {
        flagBeautify = 0;
        if(vStackedLayout->currentWidget() == widgetNormal)
        {
            *imgNormal = imgBeautify->copy();
            if(list.count() == 2)
            {
                if(list[0] == "Beautify")
                {
                    list.clear();
                    imgNormal->save("/tmp/scanner/scan1.png");
                    *imgRectify = imgNormal->copy();
                    list.append("Rectify");
                    ImageRectify("/tmp/scanner/scan1.png");
                    imgNormal->load("/tmp/scanner/scan1.png");
                }
                else
                    list.removeLast();
            }
            else
                list.clear();
            pixmap_scaled(*imgNormal,labNormalLeft);
            *imgEditLayout = imgNormal->copy();
            pixmap_scaled(*imgEditLayout,labEditLayout);
        }
        else
        {
            *imgEditLayout = imgBeautify->copy();
            if(list.count() == 2)
            {
                if(list[0] == "Beautify")
                {
                    list.clear();
                    imgEditLayout->save("/tmp/scanner/scan1.png");
                    *imgRectify = imgEditLayout->copy();
                    list.append("Rectify");
                    ImageRectify("/tmp/scanner/scan1.png");
                    imgEditLayout->load("/tmp/scanner/scan1.png");
                }
                else
                    list.removeLast();
            }
            else
                list.clear();
            pixmap_scaled(*imgEditLayout,labEditLayout);
            *imgNormal = imgEditLayout->copy();
            pixmap_scaled(*imgNormal,labNormalLeft);
        }
    }
}

void scan_display::switchPage()
{
    index++;
    if(index > 1)
    {
        index = 0;
        *imgNormal = imgEditLayout->copy();
        pixmap_scaled(*imgNormal,labNormalLeft);
    }
    else
    {
        *imgEditLayout = imgNormal->copy();
        pixmap_scaled(*imgEditLayout,labEditLayout);
    }
    vStackedLayout->setCurrentIndex(index);
}

void scan_display::tailor()
{
    btnTailor = new QPushButton();
    labTailor = new my_label();
    editLayoutTailor = new edit_bar();
    widgetTailor = new QWidget();
    hBoxTailor = new QHBoxLayout();
    imgTailor = new QImage();
    labTailor->setMinimumSize(360,490);
    labTailor->setParent(widgetTailor);
    btnTailor->setParent(widgetTailor);
    editLayoutTailor->setParent(widgetTailor);
    labTailor->setAlignment(Qt::AlignCenter);

    btnTailor->setFixedSize(12,30);
    btnTailor->setStyleSheet("QPushButton{border-image: url(:/icon/icon/toolbutton1.png);border:none;background-color:#0f0801;border-radius:0px;}");

    hBoxTailor->setSpacing(0);
    hBoxTailor->addSpacing(93);
    hBoxTailor->addStretch();
    hBoxTailor->addWidget(labTailor);
    hBoxTailor->addStretch();
    hBoxTailor->addSpacing(93);
    hBoxTailor->addWidget(editLayoutTailor);
    hBoxTailor->addSpacing(2);
    hBoxTailor->addWidget(btnTailor);
    hBoxTailor->setContentsMargins(0,45,0,32);
    widgetTailor->setLayout(hBoxTailor);
    vStackedLayout->addWidget(widgetTailor);
    vStackedLayout->setCurrentWidget(widgetTailor);
    *imgTailor = imgEditLayout->copy();
    *imgStack = imgEditLayout->copy();
    stack.push(*imgStack);
    scaledNum = pixmap_scaled(*imgTailor,labTailor);
}

edit_bar::edit_bar(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(40,220);
    vBoxEditBar = new QVBoxLayout(this);
    btnRotate = new QPushButton();
    btnTailor = new QPushButton();
    btnSymmetry = new QPushButton();
    btnWatermark = new QPushButton();

    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(232, 232, 232));
    setAutoFillBackground(true);
    setPalette(pal);

    btnRotate->setFixedSize(30,30);
    btnTailor->setFixedSize(30,30);
    btnSymmetry->setFixedSize(30,30);
    btnWatermark->setFixedSize(30,30);
    btnSymmetry->setToolTip("对称翻转");

    btnRotate->setStyleSheet("QPushButton{border-image: url(:/icon/icon/rotate.png);border:none;background-color:rgb(232,232,232);border-radius:0px;}"
                              "QPushButton:hover{border-image: url(:/icon/icon/rotate-click.png);border:none;background-color:rgb(232,232,232);border-radius:0px;}"
                                "QPushButton:checked{border-image: url(:/icon/icon/rotate-click.png);border:none;background-color:rgb(232,232,232);border-radius:0px;}");
    btnTailor->setStyleSheet("QPushButton{border-image: url(:/icon/icon/tailor.png);border:none;background-color:rgb(232,232,232);border-radius:0px;}"
                              "QPushButton:hover{border-image: url(:/icon/icon/tailor-click.png);border:none;background-color:rgb(232,232,232);border-radius:0px;}"
                                "QPushButton:checked{border-image: url(:/icon/icon/tailor-click.png);border:none;background-color:rgb(232,232,232);border-radius:0px;}");
    btnSymmetry->setStyleSheet("QPushButton{border-image: url(:/icon/icon/symmetry.png);border:none;background-color:rgb(232,232,232);border-radius:0px;}"
                              "QPushButton:hover{border-image: url(:/icon/icon/symmetry-click.png);border:none;background-color:rgb(232,232,232);border-radius:0px;}"
                                "QPushButton:checked{border-image: url(:/icon/icon/symmetry-click.png);border:none;background-color:rgb(232,232,232);border-radius:0px;}");
    btnWatermark->setStyleSheet("QPushButton{border-image: url(:/icon/icon/watermark.png);border:none;background-color:rgb(232,232,232);border-radius:0px;}"
                              "QPushButton:hover{border-image: url(:/icon/icon/watermark-click.png);border:none;background-color:rgb(232,232,232);border-radius:0px;}"
                                "QPushButton:checked{border-image: url(:/icon/icon/watermark-click.png);border:none;background-color:rgb(232,232,232);border-radius:0px;}");
    vBoxEditBar->setSpacing(0);
    vBoxEditBar->addSpacing(17);
    vBoxEditBar->addWidget(btnTailor);
    vBoxEditBar->addSpacing(22);
    vBoxEditBar->addWidget(btnRotate);
    vBoxEditBar->addSpacing(22);
    vBoxEditBar->addWidget(btnSymmetry);
    vBoxEditBar->addSpacing(22);
    vBoxEditBar->addWidget(btnWatermark);
    vBoxEditBar->addSpacing(17);
    vBoxEditBar->setContentsMargins(5,0,5,0);
    setLayout(vBoxEditBar);

}

void myThread::run()
{
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    qDebug()<<"orc run!\n";
    //使用中文初始化tesseract-ocr，而不指定tessdata路径。正在识别中
    if (api->Init(NULL, "chi_sim")) {
        qDebug()<<"Could not initialize tesseract.\n";
        outText = "Unable to read text";
        exit(1);
    }
    // 使用leptonica库打开输入图像。
    Pix* image = pixRead("/tmp/scanner/scan1.png");
    if(!image)
    {
        qDebug()<<"pixRead error!";
        outText = "Unable to read text";
        emit orcFinished();
        // 销毁使用过的对象并释放内存。
        api->End();
       // pixDestroy(&image);
        quit();
    }
    api->SetImage(image);
    // 得到光学字符识别结果
    outText = api->GetUTF8Text();
    emit orcFinished();
    // 销毁使用过的对象并释放内存。
    api->End();
    pixDestroy(&image);
    quit();
}

