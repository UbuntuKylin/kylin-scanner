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

#include "scanDisplay.h"

QString outText;

ScanDisplay::ScanDisplay(QWidget *parent)
    : QWidget(parent)
    , style_settings (new QGSettings(ORG_UKUI_STYLE))
    , icon_theme_settings (new QGSettings(ORG_UKUI_STYLE))
    , timerScan (new QTimer())
    , labInit (new QLabel())
    , labConnectError (new QLabel())
    , labConnectErrorText (new QLabel())
    , labNormalLeft (new QLabel())
    , labNormalRight (new QLabel())
    , labEditLayout (new QLabel())
    , labOrcLeft (new QLabel())
    , labOrcRight (new QLabel())
    , btnNormal (new QPushButton())
    , btnEditLayout (new QPushButton())
    , btnTailorLayout (new QPushButton())
    , imgConnectError (new QImage())
    , imgEditLayout (new QImage())
    , imgTailor (new QImage())
    , imgStack (new QImage())
    , imgNormal (new QImage())
    , imgBackup (new QImage())
    , imgBeautify (new QImage())
    , imgRectify (new QImage())
    , vBoxConnectError (new QVBoxLayout())
    , vBoxScanSet (new QVBoxLayout())
    , vBoxOrc (new QVBoxLayout())
    , hBoxNormal (new QHBoxLayout())
    , hBoxEditLayout (new QHBoxLayout())
    , widgetConnectError (new QWidget())
    , widgetNormal (new QWidget())
    , widgetEditLayout (new QWidget())
    //, widgetTailor (new QWidget())
    , widgetOrc (new QWidget())
    , vStackedLayout (new QStackedLayout())
    , editLayout (new EditBar())
    , editLayoutTailor (new EditBar())
    , scrollArea (new QScrollArea())
{
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(600,567);

    stylelist << STYLE_NAME_KEY_DARK << STYLE_NAME_KEY_BLACK;
    iconthemelist << ICON_THEME_KEY_BASIC << ICON_THEME_KEY_CLASSICAL << ICON_THEME_KEY_DEFAULT;

    labConnectError->setParent(widgetConnectError);
    labConnectErrorText->setParent(widgetConnectError);
    labInit->setMinimumSize(600,567);
    labConnectError->setMinimumSize(600,320);
    labConnectErrorText->setMinimumSize(600,231);
    labNormalLeft->setMinimumSize(360,490);
    labNormalRight->setFixedWidth(40);
    labEditLayout->setMinimumSize(360,490);

    labInit->setText("");
    //labInit->setCursor (Qt::BusyCursor);

    /*
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
    imgConnectError->load(":/icon/icon/index-icon.svg");
    labConnectError->setPixmap(QPixmap::fromImage(*imgConnectError));
    labConnectError->setAlignment(Qt::AlignHCenter|Qt::AlignBottom);
    */


    vBoxConnectError->setSpacing(0);
    vBoxConnectError->addStretch();
    vBoxConnectError->addWidget(labConnectError);
    vBoxConnectError->addSpacing(16);
    vBoxConnectError->addWidget(labConnectErrorText);
    vBoxConnectError->addStretch();
    vBoxConnectError->setContentsMargins(0,0,0,0);
    widgetConnectError->setLayout(vBoxConnectError);

    labNormalLeft->setParent(widgetNormal);
    labNormalLeft->setAlignment(Qt::AlignCenter);

    labNormalRight->setParent(widgetNormal);
    btnNormal->setParent(widgetNormal);
    btnNormal->setFixedSize(12,30);

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


    labEditLayout->setParent(widgetEditLayout);
    labEditLayout->setAlignment(Qt::AlignCenter);

    btnEditLayout->setParent(widgetEditLayout);
    btnEditLayout->setFixedSize(12,30);
    editLayout->setParent(widgetEditLayout);


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


    vBoxScanSet->setSpacing(0);
    vBoxScanSet->addLayout(vStackedLayout);
    vBoxScanSet->setContentsMargins(0,0,0,0);
    setLayout(vBoxScanSet);
    vStackedLayout->setCurrentWidget(labInit);
    vStackedLayout->setContentsMargins(0,0,0,0);


    initStyle ();


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
    connect(editLayout->btnWatermark,SIGNAL(clicked()),this,SLOT(addWatermark()));

    // For ORC
    connect(&thread,SIGNAL(orcFinished()),this,SLOT(orcText()));

    // For timerScan
    //connect(timerScan, SIGNAL(timeout()), this, SLOT(timerScanUpdate()));

    connect(icon_theme_settings,SIGNAL(changed(QString)), this, SLOT(scandisplay_theme_changed(QString)));
}

/**
 * @brief ScanDisplay::keyPressEvent 监听键盘事件
 * @param e
 */
void ScanDisplay::keyPressEvent(QKeyEvent *e)
{
    qDebug() << "key() = " << e->key();
    if ((e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter  || e->key() == Qt::Key_Space)
            && (vStackedLayout->currentWidget() == widgetTailor))
    {
        qDebug() << "key() = " << e->key();
        QImage newimage;
        int x1,y1,x2,y2;
        if (labTailor->getStartX() <= labTailor->getEndX())
        {
             x1 = labTailor->getStartX() - ((labTailor->width() - imgEditLayout->width()*scaledNum) / 2);
             x2 = labTailor->getEndX() - ((labTailor->width() - imgEditLayout->width()*scaledNum) / 2);
        }
        else
        {
            x1 = labTailor->getEndX() - ((labTailor->width() - imgEditLayout->width()*scaledNum) / 2);
            x2 = labTailor->getStartX() - ((labTailor->width() - imgEditLayout->width()*scaledNum) / 2);
        }

        if (labTailor->getStartY() <= labTailor->getEndY())
        {
            y1 = labTailor->getStartY() - ((labTailor->height() - imgEditLayout->height()*scaledNum) / 2);
            y2 = labTailor->getEndY() - ((labTailor->height() - imgEditLayout->height()*scaledNum) / 2);
        }
        else
        {
            y1 = labTailor->getEndY() - ((labTailor->height() - imgEditLayout->height()*scaledNum) / 2);
            y2 = labTailor->getStartY() - ((labTailor->height() - imgEditLayout->height()*scaledNum) / 2);
        }

        newimage = imgEditLayout->copy(x1/scaledNum,y1/scaledNum,(x2-x1)/scaledNum,(y2-y1)/scaledNum);
        *imgEditLayout = newimage;
        setPixmapScaled(*imgEditLayout,labEditLayout);
        *imgNormal = imgEditLayout->copy();
        setPixmapScaled(*imgNormal,labNormalLeft);
        vStackedLayout->setCurrentIndex(index);
        vStackedLayout->removeWidget(widgetTailor);

        // 裁切完成后，btnTailor属性需要回到最开始的状态
        editLayout->btnTailor->setStyleSheet("QPushButton{border-image: url(:/icon/icon/editBar/tailor.svg);border:none;background-color:rgb(232,232,232);border-radius:0px;}"
                                  "QPushButton:hover{border-image: url(:/icon/icon/editBar/tailor-click.svg);border:none;background-color:rgb(232,232,232);border-radius:0px;}"
                                    "QPushButton:checked{border-image: url(:/icon/icon/editBar/tailor-click.svg);border:none;background-color:rgb(232,232,232);border-radius:0px;}");
        flagTailor = 0;
    }
    if (e->key() == Qt::Key_Z && e->modifiers() ==  Qt::ControlModifier)
    {
        if (!stack.isEmpty())
        {
            *imgEditLayout = stack.pop();
            setPixmapScaled(*imgEditLayout,labEditLayout);
            *imgNormal = imgEditLayout->copy();
            setPixmapScaled(*imgNormal,labNormalLeft);
            vStackedLayout->setCurrentIndex(index);
        }
    }
}

QImage *ScanDisplay::imageSave(QString filename)
{
    qDebug() << "save image: " << filename;
    if (flagOrc == 0) {
        *imgEditLayout = imgNormal->copy();
        if (filename.endsWith(".pdf"))
            return imgEditLayout;
        if (filename.endsWith(".png") || filename.endsWith(".jpg") || filename.endsWith(".bmp"))
            imgEditLayout->save(filename);
    } else {
        if (!filename.endsWith(".txt"))
            filename += ".txt";
        QFile file(filename);
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        QByteArray str = outText.toUtf8();
        file.write(str);
        file.close();
    }
    return NULL;
}

/**
 * @brief ScanDisplay::setNoDevice
 * 设置无设备时的页面widget
 * 对设计新的等待页面有作用
 */
void ScanDisplay::setNoDevice()
{
    vStackedLayout->setCurrentWidget(widgetConnectError);
}

/**
 * @brief ScanDisplay::setInitDevice
 * 设置初始化页面widget
 */
void ScanDisplay::setInitDevice()
{
    vStackedLayout->setCurrentWidget(labInit);
}

void ScanDisplay::setPixmap(QImage img, QLabel *lab)
{
    int width = lab->width();
    int height = lab->height();
    QPixmap pixmap = QPixmap::fromImage(img);
    QPixmap fitpixmap = pixmap.scaled(width, height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);  // 按比例缩放
    lab->setPixmap(fitpixmap);
    lab->setAlignment(Qt::AlignCenter);
}

float ScanDisplay::setPixmapScaled(QImage img, QLabel *lab)
{
    float labWidth = this->width() - 240;
    float labHeight = this->height() - 77;
    float imgWidth = img.width();
    float imgHeight = img.height();
    float num = 1;
    if ((labWidth/imgWidth) <= (labHeight/imgHeight))
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

void ScanDisplay::updateWindowSize()
{
    setPixmapScaled(*imgNormal,labNormalLeft);
    setPixmapScaled(*imgEditLayout,labEditLayout);
}


void ScanDisplay::timerEvent(QTimerEvent *e)
{
    int id = e->timerId();

    if (id == m_timeScanId)
    {
        qDebug() << "timeScanId";
//            labNormalLeft->height = 490;

//            labNormalLeft->data = NULL; //图像信息
//            labNormalLeft->update();
    }
}

void ScanDisplay::initStyle()
{
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        QPalette init_pacolor_black;
        init_pacolor_black.setColor (QPalette::Background, QColor(15,8,1));
        labInit->clear ();
        labInit->setAutoFillBackground (true);
        labInit->setPalette (init_pacolor_black);

        QFont ft;
        ft.setPointSize(24);

        labConnectErrorText->setFont(ft);
        labConnectErrorText->setText (tr("Please connect to a scan device firstly !"));
        labConnectErrorText->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
        labConnectErrorText->setStyleSheet ("QLabel{background-color:rgb(15,8,1);"
                                            "color:rgb(232,232,232);}");

        imgConnectError->load(":/icon/icon/no-conection-dark.svg");
        labConnectError->setPixmap(QPixmap::fromImage(*imgConnectError));
        labConnectError->setAlignment(Qt::AlignHCenter|Qt::AlignBottom);

        QPalette connect_error_pacolor_black;
        connect_error_pacolor_black.setColor (QPalette::Background, QColor(15,8,1)); 
        labConnectError->setAutoFillBackground (true);
        labConnectError->setPalette (connect_error_pacolor_black);

        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(15, 8, 1));
        setAutoFillBackground(true);
        setPalette(pal);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        btnEditLayout->setStyleSheet("QPushButton{border-image: url(:/icon/icon/editBar/shrink-editLayout.svg);border:none;background-color:#0f0801;border-radius:0px;}" );
        btnNormal->setStyleSheet("QPushButton{ "
                                 "border-image: url(:/icon/icon/editBar/shrink-normal.svg);"
                                 "border:none;"
                                 "background-color:#0f0801;"
                                 "border-radius:0px;"
                                 "}");
        labNormalRight->setStyleSheet("QLabel{background-color:#0f0801;}");

    } else {
        QPalette connect_error_pacolor;
        QPalette init_pacolor;
        QFont ft;

        ft.setPointSize(24);
        labConnectErrorText->setFont(ft);
        labConnectErrorText->setText (tr("Please connect to a scan device firstly !"));
        labConnectErrorText->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
        labConnectErrorText->setStyleSheet ("QLabel{background-color:rgb(232,232,232);"
                                            "color:rgb(15,8,1);}");

        imgConnectError->load(":/icon/icon/no-conection-light.svg");
        labConnectError->setPixmap(QPixmap::fromImage(*imgConnectError));
        labConnectError->setAlignment(Qt::AlignHCenter|Qt::AlignBottom);

        connect_error_pacolor.setColor (QPalette::Background, QColor(232,232,232));
        labConnectError->setAutoFillBackground (true);
        labConnectError->setPalette (connect_error_pacolor);

        init_pacolor.setColor (QPalette::Background, QColor(232,232,232));
        labInit->clear ();
        labInit->setAutoFillBackground (true);
        labInit->setPalette (init_pacolor);

        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(232,232,232));
        setAutoFillBackground(true);
        this->setPalette(pal);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        btnEditLayout->setStyleSheet("QPushButton{border-image: url(:/icon/icon/editBar/shrink-editLayout.svg);"
                                     "border:none;background-color:#E7E7E7;border-radius:0px;}" );
        btnNormal->setStyleSheet("QPushButton{ "
                                 "border-image: url(:/icon/icon/editBar/shrink-normal.svg);"
                                 "border:none;"
                                 "background-color:#E7E7E7;"
                                 "border-radius:0px;"
                                 "}");
        labNormalRight->setStyleSheet("QLabel{background-color:#E7E7E7;}");
    }
}

void ScanDisplay::initStyleOrc()
{
    if (flagOrc == 1)
    {
        QHBoxLayout *hBoxOrc = new QHBoxLayout();
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

    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        QPalette palette;
        palette.setColor(QPalette::Background, QColor(192,253,123,100)); // palette可以模糊
        labOrcLeft->setPalette(palette);

        labOrcRight->setStyleSheet("background-color:rgb(15,8,1);color:rgb(255,255,255);border:1px solid #717171;");

        scrollArea->setStyleSheet("QScrollBar:vertical {width:6px;background:#2f2c2b;margin:0px,0px,0px,0px;padding-top:0px;padding-bottom:0px;padding-right:3px;}"
                                  "QScrollBar::handle:vertical {width:6px;background:rgba(255,255,255,50%);border-radius:3px;min-height:20;}"
                                  "QScrollBar::handle:vertical:hover {width:6px;background:rgba(255,255,255,80%);border-radius:3px;min-height:20;}"
                                  "QScrollBar::add-line:vertical {height:0px;width:0px;}"
                                  "QScrollBar::sub-line:vertical {height:0px;width:0px;subcontrol-position:top;}"
                                  );
    } else {
        QPalette palette;
        palette.setColor(QPalette::Background, QColor(192,253,123,100)); // palette可以模糊
        labOrcLeft->setPalette(palette);

        labOrcRight->setStyleSheet("background-color:rgb(255,255,255);color:rgb(15,8,1);border:1px solid #FFFFFF;");

        scrollArea->setStyleSheet("QScrollBar:vertical {width:6px;background:#E7E7E7;margin:0px,0px,0px,0px;padding-top:0px;padding-bottom:0px;padding-right:3px;}"
                                  "QScrollBar::handle:vertical {width:6px;background:rgba(15,8,1,50%);border-radius:3px;min-height:20;}"
                                  "QScrollBar::handle:vertical:hover {width:6px;background:rgba(15,8,1,80%);border-radius:3px;min-height:20;}"
                                  "QScrollBar::add-line:vertical {height:0px;width:0px;}"
                                  "QScrollBar::sub-line:vertical {height:0px;width:0px;subcontrol-position:top;}"
                                  );
    }
}

void ScanDisplay::initStyleTailor()
{
    if (flagTailor == 1)
    {
        widgetTailor = new QWidget(); // 需重新初始化，否则底部有原先的裁切图片

        labTailor = new TailorLabel();
        labTailor->setMinimumSize(360,490);
        labTailor->setParent(widgetTailor);
        labTailor->setAlignment(Qt::AlignCenter);

        hBoxTailor = new QHBoxLayout();
        hBoxTailor->setSpacing(0);
        hBoxTailor->addSpacing(93);
        hBoxTailor->addStretch();
        hBoxTailor->addWidget(labTailor);
        hBoxTailor->addStretch();
        hBoxTailor->addSpacing(93);
        hBoxTailor->addWidget(editLayoutTailor);
        hBoxTailor->addSpacing(2);
        hBoxTailor->addWidget(btnTailorLayout);
        hBoxTailor->setContentsMargins(0,45,0,32);
        widgetTailor->setLayout(hBoxTailor);

        vStackedLayout->addWidget(widgetTailor);
        vStackedLayout->setCurrentWidget(widgetTailor);
        *imgTailor = imgEditLayout->copy();

        *imgStack = imgEditLayout->copy();
        stack.push(*imgStack);
        scaledNum = setPixmapScaled(*imgTailor,labTailor);
        btnTailorLayout->setStyleSheet("QPushButton{border-image: url(:/icon/icon/editBar/shrink-editLayout.svg);border:none;background-color:#0f0801;border-radius:0px;}");
    }
}

void ScanDisplay::rotating()
{

    QMatrix matrix;
    *imgStack = imgEditLayout->copy();
    stack.push(*imgStack);
    matrix.rotate(270);
    *imgEditLayout = imgEditLayout->transformed(matrix);
    setPixmapScaled(*imgEditLayout,labEditLayout);
    *imgNormal = imgEditLayout->copy();
    setPixmapScaled(*imgNormal,labNormalLeft);

}

void ScanDisplay::symmetry()
{
    *imgStack = imgEditLayout->copy();
    stack.push(*imgStack);
    *imgEditLayout=imgEditLayout->mirrored(true,false);
    setPixmapScaled(*imgEditLayout,labEditLayout);
    *imgNormal = imgEditLayout->copy();
    setPixmapScaled(*imgNormal,labNormalLeft);
}

void ScanDisplay::addWatermark()
{
    qDebug() << "flagWaterMark   = " << flagWaterMark;
    if (flagWaterMark  == 0)
    {
        flagWaterMark = 1;
        *imgBackup = imgEditLayout->copy();
    }
    WaterMarkDialog *dialog = new WaterMarkDialog();
    int ret = dialog->exec();// 以模态方式显示对话框，用户关闭对话框时返回 DialogCode值
    qDebug() << "ret = " << ret;

    if (ret == QDialog::Accepted) //OK键被按下,对话框关闭，若设置了setAttribute(Qt::WA_DeleteOnClose)，对话框被释放，无法获得返回值
    { //OK键被按下，获取对话框上的输入，设置行数和列数
        QString text = dialog->getLineEdit();
        qDebug() << text;

        *imgStack = imgEditLayout->copy();
        stack.push(*imgStack);
        *imgEditLayout = imgBackup->copy();

        QPainter painter(imgEditLayout);
        int fontSize = 60;
        int spacing = 20;
        QFont font("华文黑体", fontSize, QFont::Thin);

        font.setLetterSpacing(QFont::AbsoluteSpacing, spacing);
        painter.setFont(font);
        painter.setPen(QColor(47,44,43)); // 字体颜色
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
               qDebug() << "drawtext: " << text;
            }
        }
        setPixmapScaled(*imgEditLayout,labEditLayout);
        *imgNormal = imgEditLayout->copy();
        setPixmapScaled(*imgNormal,labNormalLeft);
    }
    delete dialog; //删除对话框
}

void ScanDisplay::orcText()
{
    labOrcRight->setText(outText);
    qDebug() <<outText;
}

void ScanDisplay::scandisplay_theme_changed(QString)
{
    initStyle ();
    initStyleOrc ();
    initStyleTailor ();
}

void ScanDisplay::onOrc()
{
    if (flagOrc == 0)
    {
        flagOrc = 1;
        widgetindex = vStackedLayout->currentIndex();

        labOrcLeft->setFixedWidth(120);
        *imgEditLayout = imgNormal->copy();
        imgEditLayout->save("/tmp/scanner/scan1.png");
        *imgBackup = imgEditLayout->copy();
        thread.start();
        *imgBackup = imgBackup->scaled(120,166);

        labOrcLeft->setPixmap(QPixmap::fromImage(*imgBackup));
        labOrcLeft->setAlignment(Qt::AlignTop);

        outText = tr("Try to ocr ...");
        QFont ft1;
        ft1.setPointSize(14);
        labOrcRight->setFont(ft1);
        labOrcRight->setText(outText);

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
        //widget->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint); // 去掉标题栏,去掉任务栏显示，窗口置顶
        widget->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint); // 去掉标题栏,去掉任务栏显示，窗口置顶
        widget->setLayout(vBoxOrc);
        widget->setContentsMargins(0,0,0,0);

        scrollArea->setFixedWidth(392);
        scrollArea->setMinimumHeight(503);
        scrollArea->setWidget(widget);
        scrollArea->setWidgetResizable(true);
        scrollArea->setFrameShape(QFrame::NoFrame);

        initStyleOrc ();

        flagOrcInit++; //每次都加，但在第一次扫描时应该置0
    } else {
        flagOrc = 0;
        vStackedLayout->setCurrentIndex(widgetindex);
    }
}

/**
 * @brief scan_display::scan 扫描功能结束后显示图片
 */
void ScanDisplay::onScan(bool ret)
{
    qDebug() << "onScan begin, ret = " << ret;
    if (!ret)
        return;

    vStackedLayout->setCurrentIndex(0);
    vStackedLayout->setCurrentIndex(1);
    vStackedLayout->setCurrentIndex(2);
    vStackedLayout->setCurrentIndex(3);

    // 重新扫描时，应该都初始化，避免撤销到之前的图片情况
    stack.clear();
    list.clear();
    flagOrc = 0;
    flagOrcInit = 0;
    flagRectify = 0;
    flagBeautify = 0;
    flagTailor = 0;
    flagWaterMark = 0; // 需要置0，避免多次扫描造成水印的图片切换到上一次扫描的图片

    //timerScan->start(100);

#ifdef DEBUG_EDIT
    imgNormal->load("/tmp/scanner/scan.pnm");
#else
    imgNormal->load("/tmp/scanner/scan.pnm");
#endif
    setPixmapScaled(*imgNormal,labNormalLeft);
    vStackedLayout->setCurrentWidget(widgetNormal);
    *imgEditLayout = imgNormal->copy();
    setPixmapScaled(*imgEditLayout,labEditLayout);
}

/**
 * @brief scan_display::rectify 智能纠偏
 */
void ScanDisplay::onRectify()
{
    qDebug() <<"flagRectify = " << flagRectify;
    if (flagRectify == 0)
    {
        // 此时代表用户点击了智能纠偏按钮
        flagRectify = 1;
        if (vStackedLayout->currentWidget() == widgetNormal)
        {
            qDebug() << "currentWidget == widgetNormal";
            imgNormal->save("/tmp/scanner/scan1.png");
            // 为了撤销
            *imgRectify = imgNormal->copy();
            list.append("Rectify");
            qDebug() << "before ImageRectify()";
            ImageRectify("/tmp/scanner/scan1.png");
            qDebug() << "end ImageRectify()";
            imgNormal->load("/tmp/scanner/scan1.png");
            setPixmapScaled(*imgNormal,labNormalLeft);
            *imgEditLayout = imgNormal->copy();
            setPixmapScaled(*imgEditLayout,labEditLayout);
        }
        else
        {
            imgEditLayout->save("/tmp/scanner/scan1.png");
            *imgRectify = imgEditLayout->copy();
            list.append("Rectify");
            ImageRectify("/tmp/scanner/scan1.png");
            imgEditLayout->load("/tmp/scanner/scan1.png");
            setPixmapScaled(*imgEditLayout,labEditLayout);
            *imgNormal = imgEditLayout->copy();
            setPixmapScaled(*imgNormal,labNormalLeft);
        }
    }
    else
    {
        // 此时代表用户重复点击智能纠偏按钮，应该进行撤销操作
        flagRectify = 0;
        if (vStackedLayout->currentWidget() == widgetNormal)
        {
            qDebug() << "currentWidget == widgetNormal";
            *imgNormal = imgRectify->copy();
            /**
             * 2代表点击了先智能纠偏和后一键美化：
             * 有2种情况： 1）先撤销一键美化 2）先撤销智能纠偏
             */
            qDebug() << "list.count = " << list.count();
            if (list.count() == 2)
            {
                // 撤销的是智能纠偏：先全部清空，后把一键美化的加上
                if (list[0] == "Rectify")
                {
                    list.clear();
                    imgNormal->save("/tmp/scanner/scan1.png");
                    *imgBeautify = imgNormal->copy();
                    list.append("Beautify");
                    oneClickBeauty("/tmp/scanner/scan1.png");
                    imgNormal->load("/tmp/scanner/scan1.png");
                }
                else
                {
                    // 撤销的是一键美化：撤销到之前一个就行
                    list.removeLast();
                }
            }
            else
                list.clear();
            setPixmapScaled(*imgNormal,labNormalLeft);
            *imgEditLayout = imgNormal->copy();
            setPixmapScaled(*imgEditLayout,labEditLayout);
        }
        else
        {
            *imgEditLayout = imgRectify->copy();
            qDebug() << "list.count = " << list.count();
            if (list.count() == 2)
            {
                if (list[0] == "Rectify")
                {
                    qDebug() << "list[0] = " << list[0];
                    list.clear();
                    imgEditLayout->save("/tmp/scanner/scan1.png");
                    *imgBeautify = imgEditLayout->copy();
                    list.append("Beautify");
                    oneClickBeauty("/tmp/scanner/scan1.png");
                    imgEditLayout->load("/tmp/scanner/scan1.png");
                }
                else
                    list.removeLast();
            }
            else
                list.clear();
            setPixmapScaled(*imgEditLayout,labEditLayout);
            *imgNormal = imgEditLayout->copy();
            setPixmapScaled(*imgNormal,labNormalLeft);
        }
    }
}

/**
 * @brief scan_display::beautify 一键美化
 */
void ScanDisplay::onBeautify()
{
    qDebug() << "beauty()";
    if (flagBeautify == 0)
    {
        flagBeautify = 1;
        if (vStackedLayout->currentWidget() == widgetNormal)
        {
            imgNormal->save("/tmp/scanner/scan1.png");
            *imgBeautify = imgNormal->copy();
            list.append("Beautify");
            oneClickBeauty("/tmp/scanner/scan1.png");
            imgNormal->load("/tmp/scanner/scan1.png");
            setPixmapScaled(*imgNormal,labNormalLeft);
            *imgEditLayout = imgNormal->copy();
            setPixmapScaled(*imgEditLayout,labEditLayout);
        }
        else
        {
            imgEditLayout->save("/tmp/scanner/scan1.png");
            *imgBeautify = imgEditLayout->copy();
            list.append("Beautify");
            oneClickBeauty("/tmp/scanner/scan1.png");
            imgEditLayout->load("/tmp/scanner/scan1.png");
            setPixmapScaled(*imgEditLayout,labEditLayout);
            *imgNormal = imgEditLayout->copy();
            setPixmapScaled(*imgNormal,labNormalLeft);
        }
    }
    else
    {
        flagBeautify = 0;
        if (vStackedLayout->currentWidget() == widgetNormal)
        {
            *imgNormal = imgBeautify->copy();
            if (list.count() == 2)
            {
                if (list[0] == "Beautify")
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
            setPixmapScaled(*imgNormal,labNormalLeft);
            *imgEditLayout = imgNormal->copy();
            setPixmapScaled(*imgEditLayout,labEditLayout);
        }
        else
        {
            *imgEditLayout = imgBeautify->copy();
            if (list.count() == 2)
            {
                if (list[0] == "Beautify")
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
            setPixmapScaled(*imgEditLayout,labEditLayout);
            *imgNormal = imgEditLayout->copy();
            setPixmapScaled(*imgNormal,labNormalLeft);
        }
    }
}

/**
 * @brief scan_display::switchPage 用于右侧工具栏切换页面
 */
void ScanDisplay::switchPage()
{
    index++;
    if (index > 1)
    {
        qDebug() << "1 switchPage index = " << index;
        index = 0;
        *imgNormal = imgEditLayout->copy();
        setPixmapScaled(*imgNormal,labNormalLeft);
    }
    else
    {
        qDebug() << "2 switchPage index = " << index;
        *imgEditLayout = imgNormal->copy();
        setPixmapScaled(*imgEditLayout,labEditLayout);
    }
    vStackedLayout->setCurrentIndex(index);
}

/**
 * @brief ScanDisplay::timerScanUpdate 定时器事件
 */
void ScanDisplay::timerScanUpdate()
{
    m_timerNum++;
    //qDebug() << m_timerNum << "timer";
    if (m_timerNum == 50)
    {
        timerScan->stop();
        //qDebug() << "timer stop";
        emit scanTimerFinished();
    }
}

/**
 * @brief scan_display::tailor 工具栏裁切、裁剪
 */
void ScanDisplay::tailor()
{
    qDebug() << "begin";

    btnTailorLayout->setParent(widgetTailor);
    btnTailorLayout->setFixedSize(12,30);

    // 当点击裁切时，需要更换图片为点击时的状态，通过css不行，hover和clicked会冲突
    editLayoutTailor->setParent(widgetTailor); // 编辑工具栏布局
    editLayoutTailor->btnTailor->setIcon(QIcon(":/icon/icon/editBar/tailor-click.svg"));
    editLayoutTailor->btnTailor->setIconSize (QSize(30, 30));
    flagTailor = 1;

    initStyleTailor ();

}

/**
 * @brief edit_bar::edit_bar 工具栏
 * @param parent
 */
EditBar::EditBar(QWidget *parent)
    : QWidget(parent)
    , style_settings (new QGSettings(ORG_UKUI_STYLE))
    , icon_theme_settings (new QGSettings(ORG_UKUI_STYLE))
    , btnTailor (new QPushButton())
    , btnRotate (new QPushButton())
    , btnSymmetry (new QPushButton())
    , btnWatermark (new QPushButton())
    , vBoxEditBar (new QVBoxLayout())
{
    setFixedSize(40,220);

    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(232, 232, 232));
    setAutoFillBackground(true);
    setPalette(pal);

    //setEditBarWindowBorderRadius();

    btnRotate->setFixedSize(30,30);
    btnTailor->setFixedSize(30,30);
    btnSymmetry->setFixedSize(30,30);
    btnWatermark->setFixedSize(30,30);
    btnRotate->setToolTip(tr("rotate")); // 旋转
    btnTailor->setToolTip(tr("tailor")); // 裁切
    btnWatermark->setToolTip(tr("watermark")); // 水印
    btnSymmetry->setToolTip(tr("symmetry")); // 对称翻转

    btnRotate->setStyleSheet("QPushButton{border-image: url(:/icon/icon/editBar/rotate.svg);border:none;background-color:rgb(232,232,232);border-radius:0px;}"
                              "QPushButton:hover{border-image: url(:/icon/icon/editBar/rotate-click.svg);border:none;background-color:rgb(232,232,232);border-radius:0px;}"
                                "QPushButton:checked{border-image: url(:/icon/icon/editBar/rotate-click.svg);border:none;background-color:rgb(232,232,232);border-radius:0px;}");
    btnTailor->setStyleSheet("QPushButton{border-image: url(:/icon/icon/editBar/tailor.svg);border:none;background-color:rgb(232,232,232);border-radius:0px;}"
                              "QPushButton:hover{border-image: url(:/icon/icon/editBar/tailor-click.svg);border:none;background-color:rgb(232,232,232);border-radius:0px;}"
                                "QPushButton:checked{border-image: url(:/icon/icon/editBar/tailor-click.svg);border:none;background-color:rgb(232,232,232);border-radius:0px;}");
    btnSymmetry->setStyleSheet("QPushButton{border-image: url(:/icon/icon/editBar/symmetry.svg);border:none;background-color:rgb(232,232,232);border-radius:0px;}"
                              "QPushButton:hover{border-image: url(:/icon/icon/editBar/symmetry-click.svg);border:none;background-color:rgb(232,232,232);border-radius:0px;}"
                                "QPushButton:checked{border-image: url(:/icon/icon/editBar/symmetry-click.svg);border:none;background-color:rgb(232,232,232);border-radius:0px;}");
    btnWatermark->setStyleSheet("QPushButton{border-image: url(:/icon/icon/editBar/watermark.svg);border:none;background-color:rgb(232,232,232);border-radius:0px;}"
                              "QPushButton:hover{border-image: url(:/icon/icon/editBar/watermark-click.svg);border:none;background-color:rgb(232,232,232);border-radius:0px;}"
                                "QPushButton:checked{border-image: url(:/icon/icon/editBarwatermark-click.svg);border:none;background-color:rgb(232,232,232);border-radius:0px;}");
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

    connect(btnTailor, SIGNAL(clicked()), this, SLOT(onBtnTailorClicked()));

}

/**
 * @brief EditBar::setEditBarWindowBorderRadius 设置窗口圆角
 */
void EditBar::setEditBarWindowBorderRadius()
{
    QBitmap bitMap(width(),height()); // A bit map has the same size with current widget
    bitMap.fill();
    QPainter painter(&bitMap);
    painter.setBrush(Qt::black);
    painter.setPen(Qt::NoPen); // Any color that is not QRgb(0,0,0) is right
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawRoundedRect(bitMap.rect(),4,4); //设置圆角弧度4px
    setMask(bitMap);
}

void EditBar::onBtnTailorClicked()
{
    qDebug() << "clicked";
}

/**
 * @brief myThread::run 文字识别线程
 */
void myThread::run()
{
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    qDebug() <<"orc run!\n";
    //使用中文初始化tesseract-ocr，而不指定tessdata路径。正在识别中
    if (api->Init(NULL, "chi_sim")) {
        qDebug() <<"Could not initialize tesseract.\n";
        outText = "Unable to read text";
        exit(1);
    }
    // 使用leptonica库打开输入图像。
    Pix* image = pixRead("/tmp/scanner/scan1.png");
    if (!image)
    {
        qDebug() <<"pixRead error!";
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
