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
    labDisplayInit = new QLabel();
    labDisplayConnectError = new QLabel();
    labDisplay3 = new QLabel();
    labDisplay4 = new QLabel();
    labDisplay5 = new QLabel();
    labDisplay6 = new QLabel();
    labDisplay7 = new QLabel();

    btnTool = new QPushButton();
    btnTool1 = new QPushButton();

    myWidget = new QWidget();
    myWidget1 = new QWidget();
    myWidget2 = new QWidget();

    img = new QImage();
    img1 = new QImage();
    img2 = new QImage();
    img4 = new QImage();
    img5 = new QImage();
    imgBeautify = new QImage();
    imgRectify = new QImage();
    vBoxScanSet = new QVBoxLayout();
    vBoxScanSet1 = new QVBoxLayout(this);
    vBoxScanSet2 = new QVBoxLayout();
    hBoxScanSet = new QHBoxLayout();
    hBoxScanSet1 = new QHBoxLayout();

    vStackedLayout = new QStackedLayout();

    labDisplayConnectError->setParent(myWidget);
    labDisplay3->setParent(myWidget);
    labDisplayInit->setMinimumSize(600,567);
    labDisplayConnectError->setMinimumSize(600,320);
    labDisplay3->setMinimumSize(600,231);
    labDisplay4->setMinimumSize(600,567);
    labDisplay5->setMinimumSize(360,490);
    labDisplay6->setFixedWidth(40);
    labDisplay7->setMinimumSize(360,490);


    labDisplayInit->setText(" ");


    img->load(":/icon/icon/connect_device.png");
    labDisplayConnectError->setPixmap(QPixmap::fromImage(*img));
    labDisplayConnectError->setAlignment(Qt::AlignHCenter|Qt::AlignBottom);

//    img1->load("/home/test/e.png");
    img1->load("scan.pnm");
    labDisplay4->setPixmap(QPixmap::fromImage(*img1));
    labDisplay4->setAlignment(Qt::AlignCenter);
    QFont ft;
    ft.setPointSize(24);
    labDisplay3->setFont(ft);
    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(232,232,232));
    labDisplay3->setPalette(pa);
    labDisplay3->setText(tr("Please connect to a scan device firstly !"));
    labDisplay3->setAlignment(Qt::AlignHCenter|Qt::AlignTop);

    vBoxScanSet->setSpacing(0);
    vBoxScanSet->addStretch();
    vBoxScanSet->addWidget(labDisplayConnectError);
    vBoxScanSet->addSpacing(16);
    vBoxScanSet->addWidget(labDisplay3);
    vBoxScanSet->addStretch();
    vBoxScanSet->setContentsMargins(0,0,0,0);
    myWidget->setLayout(vBoxScanSet);

    labDisplay5->setParent(myWidget1);
    labDisplay6->setParent(myWidget1);
    btnTool->setParent(myWidget1);
    btnTool->setFixedSize(12,30);
    btnTool->setStyleSheet("QPushButton{border-image: url(:/icon/icon/toolbutton.png);border:none;background-color:#0f0801;border-radius:0px;}");

    labDisplay5->setAlignment(Qt::AlignCenter);
    labDisplay6->setStyleSheet("QLabel{background-color:#0f0801;}");

    hBoxScanSet->setSpacing(0);
    hBoxScanSet->addSpacing(93);
    hBoxScanSet->addStretch();
    hBoxScanSet->addWidget(labDisplay5);
    hBoxScanSet->addStretch();
    hBoxScanSet->addSpacing(93);
    hBoxScanSet->addWidget(labDisplay6);
    hBoxScanSet->addSpacing(2);
    hBoxScanSet->addWidget(btnTool);
    hBoxScanSet->setContentsMargins(0,45,0,32);
    myWidget1->setLayout(hBoxScanSet);


    editlayout = new edit_bar();
    labDisplay7->setParent(myWidget2);
    btnTool1->setParent(myWidget2);
    editlayout->setParent(myWidget2);


    labDisplay7->setAlignment(Qt::AlignCenter);


    btnTool1->setFixedSize(12,30);
    btnTool1->setStyleSheet("QPushButton{border-image: url(:/icon/icon/toolbutton1.png);border:none;background-color:#0f0801;border-radius:0px;}");

    hBoxScanSet1->setSpacing(0);
    hBoxScanSet1->addSpacing(93);
    hBoxScanSet1->addStretch();
    hBoxScanSet1->addWidget(labDisplay7);
    hBoxScanSet1->addStretch();
    hBoxScanSet1->addSpacing(93);
    hBoxScanSet1->addWidget(editlayout);
    hBoxScanSet1->addSpacing(2);
    hBoxScanSet1->addWidget(btnTool1);
    hBoxScanSet1->setContentsMargins(0,45,0,32);
    myWidget2->setLayout(hBoxScanSet1);


    vStackedLayout->addWidget(myWidget1);
    vStackedLayout->addWidget(myWidget2);
    vStackedLayout->addWidget(myWidget);
    vStackedLayout->addWidget(labDisplayInit);
    vStackedLayout->addWidget(labDisplay4);

    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(15, 8, 1));
    setAutoFillBackground(true);
    setPalette(pal);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    vBoxScanSet1->setSpacing(0);
    vBoxScanSet1->addLayout(vStackedLayout);
    vBoxScanSet1->setContentsMargins(0,0,0,0);
    setLayout(vBoxScanSet1);
    vStackedLayout->setCurrentWidget(labDisplayInit);
    vStackedLayout->setContentsMargins(0,0,0,0);

    // For switch page
    connect(btnTool,SIGNAL(clicked()),this,SLOT(switchPage()));
    connect(btnTool1,SIGNAL(clicked()),this,SLOT(switchPage()));

    //For rotate
    connect(editlayout->btnRotate,SIGNAL(clicked()),this,SLOT(rotating()));

    // For tailor
    connect(editlayout->btnTailor,SIGNAL(clicked()),this,SLOT(switchPage1()));

    // For symmetry
    connect(editlayout->btnSymmetry,SIGNAL(clicked()),this,SLOT(symmetry()));

    // For watermark
    connect(editlayout->btnWatermark,SIGNAL(clicked()),this,SLOT(addmark()));

    // For ORC
    connect(&thread,SIGNAL(orcFinished()),this,SLOT(orcText()));
}

void scan_display::keyPressEvent(QKeyEvent *e)
{
    if (((e->key() == Qt::Key_Return) || (e->key() == 0x20)) && (vStackedLayout->currentWidget() == myWidget3))
    {
        QImage newimage;
        int x1,y1,x2,y2;
        if(labDisplay8->getX1() <= labDisplay8->getX2())
        {
             x1 = labDisplay8->getX1() - ((labDisplay8->width() - img2->width()*scaledNum) / 2);
             x2 = labDisplay8->getX2() - ((labDisplay8->width() - img2->width()*scaledNum) / 2);
        }
        else
        {
            x1 = labDisplay8->getX2() - ((labDisplay8->width() - img2->width()*scaledNum) / 2);
            x2 = labDisplay8->getX1() - ((labDisplay8->width() - img2->width()*scaledNum) / 2);
        }

        if(labDisplay8->getY1() <= labDisplay8->getY2())
        {
            y1 = labDisplay8->getY1() - ((labDisplay8->height() - img2->height()*scaledNum) / 2);
            y2 = labDisplay8->getY2() - ((labDisplay8->height() - img2->height()*scaledNum) / 2);
        }
        else
        {
            y1 = labDisplay8->getY2() - ((labDisplay8->height() - img2->height()*scaledNum) / 2);
            y2 = labDisplay8->getY1() - ((labDisplay8->height() - img2->height()*scaledNum) / 2);
        }

        newimage = img2->copy(x1/scaledNum,y1/scaledNum,(x2-x1)/scaledNum,(y2-y1)/scaledNum);
        *img2 = newimage;
        pixmap_scaled(*img2,labDisplay7);
        *img5 = img2->copy();
        pixmap_scaled(*img5,labDisplay5);
        vStackedLayout->setCurrentIndex(index);
        vStackedLayout->removeWidget(myWidget3);

    }
    if(e->key() == Qt::Key_Z && e->modifiers() ==  Qt::ControlModifier)
    {
        if(!stack.isEmpty())
        {
            *img2 = stack.pop();
            pixmap_scaled(*img2,labDisplay7);
            *img5 = img2->copy();
            pixmap_scaled(*img5,labDisplay5);
            vStackedLayout->setCurrentIndex(index);
        }
    }
}

QImage *scan_display::imageSave(QString fileName)
{
    if(flagOrc == 0)
    {
        *img2 = img5->copy();
        if(fileName.endsWith(".pdf"))
            return img2;
        if(fileName.endsWith(".png") || fileName.endsWith(".jpg") || fileName.endsWith(".bmp"))
            img2->save(fileName);
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
    vStackedLayout->setCurrentWidget(myWidget);
}

void scan_display::set_init_device()
{
    vStackedLayout->setCurrentWidget(labDisplayInit);
}

void scan_display::set_pixmap(QImage img, QLabel *lab)
{
    int width = lab->width();
    int height = lab->height();
    QPixmap pixmap = QPixmap::fromImage(img);
    QPixmap fitpixmap = pixmap.scaled(width, height, Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);  // 按比例缩放
    lab->setPixmap(fitpixmap);
    lab->setAlignment(Qt::AlignCenter);
}

float scan_display::pixmap_scaled(QImage img, QLabel *lab)
{
    float labWidth = lab->width();
    float labHeight = lab->height();
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
    QPixmap fitpixmap = pixmap.scaled(width, height, Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);  // 按比例缩放
    lab->setPixmap(fitpixmap);
    lab->setAlignment(Qt::AlignCenter);
    return num;
}

void scan_display::rotating()
{

    QMatrix matrix;
    *img4 = img2->copy();
    stack.push(*img4);
    matrix.rotate(270);
    *img2 = img2->transformed(matrix);
    pixmap_scaled(*img2,labDisplay7);
    *img5 = img2->copy();
    pixmap_scaled(*img5,labDisplay5);

}

void scan_display::symmetry()
{
    *img4 = img2->copy();
    stack.push(*img4);
    *img2=img2->mirrored(true,false);
    pixmap_scaled(*img2,labDisplay7);
    *img5 = img2->copy();
    pixmap_scaled(*img5,labDisplay5);
}

void scan_display::addmark()
{

    if(n == 0)
    {
        n = 1;
        img6 = new QImage();
        *img6 = img2->copy();
    }
    QString text;
    mark_dialog *dialog = new mark_dialog(this);
    int ret=dialog->exec();// 以模态方式显示对话框，用户关闭对话框时返回 DialogCode值
    if (ret==QDialog::Accepted) //OK键被按下,对话框关闭，若设置了setAttribute(Qt::WA_DeleteOnClose)，对话框被释放，无法获得返回值
    { //OK键被按下，获取对话框上的输入，设置行数和列数
        text = dialog->get_lineedit();
        *img4 = img2->copy();
        stack.push(*img4);
        *img2 = img6->copy();
        QPainter painter(img2);
        int fontSize = 70, spacing = 20;
        QFont font("华文黑体", fontSize, QFont::Thin);
        font.setLetterSpacing(QFont::AbsoluteSpacing, spacing);
        painter.setFont(font);
        painter.setPen(QColor(1, 1, 1));
        //painter.translate(img2->width() / 2, -img2->width() / 2);//调整位置
        painter.rotate(15);

        int squareEdgeSize = img2->width() * sin(45) + img2->height() * sin(45);//对角线长度
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
        pixmap_scaled(*img2,labDisplay7);
        *img5 = img2->copy();
        pixmap_scaled(*img5,labDisplay5);
    }
    delete dialog; //删除对话框

}

void scan_display::orcText()
{
    labDisplay10->setText(outText);
    qDebug()<<outText;
}

void scan_display::orc()
{
    if(flagOrc == 0)
    {
        flagOrc = 1;
        widgetindex = vStackedLayout->currentIndex();
        labDisplay9 = new QLabel();
        labDisplay10 = new QLabel();
        img6 = new QImage();
        vBoxScanSet3 = new QVBoxLayout();
        hBoxScanSet3 = new QHBoxLayout();
        myWidget4 = new QWidget();
        scrollArea = new QScrollArea();

        labDisplay9->setFixedWidth(120);
        *img2 = img5->copy();
        img2->save("/tmp/scanner/scan1.png");
        *img6 = img2->copy();
        thread.start();
        *img6 = img6->scaled(120,166);
        QPalette palette;
        palette.setColor(QPalette::Background, QColor(192,253,123,100));
        labDisplay9->setPalette(palette);
        labDisplay9->setPixmap(QPixmap::fromImage(*img6));
        labDisplay9->setAlignment(Qt::AlignTop);

        outText = tr("Try to ocr ...");
        QFont ft1;
        ft1.setPointSize(14);
        labDisplay10->setFont(ft1);
        QPalette pa1;
        pa1.setColor(QPalette::WindowText,QColor(255,255,255));
        labDisplay10->setPalette(pa1);
        labDisplay10->setText(outText);
        labDisplay10->setStyleSheet("background-color:#2f2c2b;border:1px solid #717171;");
        labDisplay10->setMargin(20);
        labDisplay10->setAlignment(Qt::AlignTop);
        labDisplay10->setWordWrap(true);
        labDisplay9->setParent(myWidget4);
        labDisplay10->setParent(myWidget4);
        vBoxScanSet3->setSpacing(0);
        vBoxScanSet3->addWidget(labDisplay10);
        vBoxScanSet3->setContentsMargins(0,0,0,0);
        scrollArea->setParent(myWidget4);
        QWidget *widget = new QWidget();

        widget->setMinimumHeight(labDisplay10->height());
        widget->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint); // 去掉标题栏,去掉任务栏显示，窗口置顶
        widget->setLayout(vBoxScanSet3);
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
        hBoxScanSet3->setSpacing(0);
        hBoxScanSet3->addStretch();
        hBoxScanSet3->addWidget(labDisplay9);
        hBoxScanSet3->addSpacing(24);
        hBoxScanSet3->addWidget(scrollArea);
        hBoxScanSet3->addStretch();
        hBoxScanSet3->setContentsMargins(32,32,32,32);
        myWidget4->setLayout(hBoxScanSet3);
        vStackedLayout->addWidget(myWidget4);
        vStackedLayout->setCurrentWidget(myWidget4);
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

    img5->load("/tmp/scanner/scan.pnm");
    pixmap_scaled(*img5,labDisplay5);
    vStackedLayout->setCurrentWidget(myWidget1);
    *img2 = img5->copy();
    pixmap_scaled(*img2,labDisplay7);
}

void scan_display::rectify()
{
    qDebug()<<"rectify\n";
    if(flagRectify == 0)
    {
        flagRectify = 1;
        if(vStackedLayout->currentWidget() == myWidget1)
        {
            img5->save("/tmp/scanner/scan1.png");
            *imgRectify = img5->copy();
            list.append("Rectify");
            ImageRectify("/tmp/scanner/scan1.png");
            img5->load("/tmp/scanner/scan1.png");
            pixmap_scaled(*img5,labDisplay5);
            *img2 = img5->copy();
            pixmap_scaled(*img2,labDisplay7);
        }
        else
        {
            img2->save("/tmp/scanner/scan1.png");
            *imgRectify = img2->copy();
            list.append("Rectify");
            ImageRectify("/tmp/scanner/scan1.png");
            img2->load("/tmp/scanner/scan1.png");
            pixmap_scaled(*img2,labDisplay7);
            *img5 = img2->copy();
            pixmap_scaled(*img5,labDisplay5);
        }
    }
    else
    {
        flagRectify = 0;
        if(vStackedLayout->currentWidget() == myWidget1)
        {
            *img5 = imgRectify->copy();
            if(list.count() == 2)
            {
                if(list[0] == "Rectify")
                {
                    list.clear();
                    img5->save("/tmp/scanner/scan1.png");
                    *imgBeautify = img5->copy();
                    list.append("Beautify");
                    oneClickEmbelish("/tmp/scanner/scan1.png");
                    img5->load("/tmp/scanner/scan1.png");
                }
                else
                    list.removeLast();
            }
            else
                list.clear();
            pixmap_scaled(*img5,labDisplay5);
            *img2 = img5->copy();
            pixmap_scaled(*img2,labDisplay7);
        }
        else
        {
            *img2 = imgRectify->copy();
            if(list.count() == 2)
            {
                if(list[0] == "Rectify")
                {
                    list.clear();
                    img2->save("/tmp/scanner/scan1.png");
                    *imgBeautify = img2->copy();
                    list.append("Beautify");
                    oneClickEmbelish("/tmp/scanner/scan1.png");
                    img2->load("/tmp/scanner/scan1.png");
                }
                else
                    list.removeLast();
            }
            else
                list.clear();
            pixmap_scaled(*img2,labDisplay7);
            *img5 = img2->copy();
            pixmap_scaled(*img5,labDisplay5);
        }
    }
}

void scan_display::beautify()
{
    qDebug() << "beauty()";
    if(flagBeautify == 0)
    {
        flagBeautify = 1;
        if(vStackedLayout->currentWidget() == myWidget1)
        {
            img5->save("/tmp/scanner/scan1.png");
            *imgBeautify = img5->copy();
            list.append("Beautify");
            oneClickEmbelish("/tmp/scanner/scan1.png");
            img5->load("/tmp/scanner/scan1.png");
            pixmap_scaled(*img5,labDisplay5);
            *img2 = img5->copy();
            pixmap_scaled(*img2,labDisplay7);
        }
        else
        {
            img2->save("/tmp/scanner/scan1.png");
            *imgBeautify = img2->copy();
            list.append("Beautify");
            oneClickEmbelish("/tmp/scanner/scan1.png");
            img2->load("/tmp/scanner/scan1.png");
            pixmap_scaled(*img2,labDisplay7);
            *img5 = img2->copy();
            pixmap_scaled(*img5,labDisplay5);
        }
    }
    else
    {
        flagBeautify = 0;
        if(vStackedLayout->currentWidget() == myWidget1)
        {
            *img5 = imgBeautify->copy();
            if(list.count() == 2)
            {
                if(list[0] == "Beautify")
                {
                    list.clear();
                    img5->save("/tmp/scanner/scan1.png");
                    *imgRectify = img5->copy();
                    list.append("Rectify");
                    ImageRectify("/tmp/scanner/scan1.png");
                    img5->load("/tmp/scanner/scan1.png");
                }
                else
                    list.removeLast();
            }
            else
                list.clear();
            pixmap_scaled(*img5,labDisplay5);
            *img2 = img5->copy();
            pixmap_scaled(*img2,labDisplay7);
        }
        else
        {
            *img2 = imgBeautify->copy();
            if(list.count() == 2)
            {
                if(list[0] == "Beautify")
                {
                    list.clear();
                    img2->save("/tmp/scanner/scan1.png");
                    *imgRectify = img2->copy();
                    list.append("Rectify");
                    ImageRectify("/tmp/scanner/scan1.png");
                    img2->load("/tmp/scanner/scan1.png");
                }
                else
                    list.removeLast();
            }
            else
                list.clear();
            pixmap_scaled(*img2,labDisplay7);
            *img5 = img2->copy();
            pixmap_scaled(*img5,labDisplay5);
        }
    }
}

void scan_display::switchPage()
{
    index++;
    if(index > 1)
    {
        index = 0;
        *img5 = img2->copy();
        pixmap_scaled(*img5,labDisplay5);
    }
    else
    {
        *img2 = img5->copy();
        pixmap_scaled(*img2,labDisplay7);
    }
    vStackedLayout->setCurrentIndex(index);
}

void scan_display::switchPage1()
{
    btnTool2 = new QPushButton();
    labDisplay8 = new my_label();
    editlayout1 = new edit_bar();
    myWidget3 = new QWidget();
    hBoxScanSet2 = new QHBoxLayout();
    img3 = new QImage();
    labDisplay8->setMinimumSize(360,490);
    labDisplay8->setParent(myWidget3);
    btnTool2->setParent(myWidget3);
    editlayout1->setParent(myWidget3);
    labDisplay8->setAlignment(Qt::AlignCenter);

    btnTool2->setFixedSize(12,30);
    btnTool2->setStyleSheet("QPushButton{border-image: url(:/icon/icon/toolbutton1.png);border:none;background-color:#0f0801;border-radius:0px;}");

    hBoxScanSet2->setSpacing(0);
    hBoxScanSet2->addSpacing(93);
    hBoxScanSet2->addStretch();
    hBoxScanSet2->addWidget(labDisplay8);
    hBoxScanSet2->addStretch();
    hBoxScanSet2->addSpacing(93);
    hBoxScanSet2->addWidget(editlayout1);
    hBoxScanSet2->addSpacing(2);
    hBoxScanSet2->addWidget(btnTool2);
    hBoxScanSet2->setContentsMargins(0,45,0,32);
    myWidget3->setLayout(hBoxScanSet2);
    vStackedLayout->addWidget(myWidget3);
    vStackedLayout->setCurrentWidget(myWidget3);
    *img3 = img2->copy();
    *img4 = img2->copy();
    stack.push(*img4);
    scaledNum = pixmap_scaled(*img3,labDisplay8);
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

