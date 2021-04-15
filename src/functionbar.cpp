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

#include <QDir>
#include "functionbar.h"

KYCFunctionBarWidget::KYCFunctionBarWidget(QWidget *parent)
    : QWidget(parent)
    , time (new QTimer())
    , style_settings (new QGSettings(ORG_UKUI_STYLE))
    , icon_theme_settings (new QGSettings(ORG_UKUI_STYLE))
    , btnNorScan (new QPushButton())
    , btnBeautify (new QPushButton())
    , btnRectify (new QPushButton())
    , btnOcr (new QPushButton())
    , btnScan (new QPushButton())
      //, movieScan (new QMovie())
    , labMovieScan (new QLabel())
    , labNorScan (new QLabel())
    , labBeautify (new QLabel())
    , labRectify (new QLabel())
    , labOcr (new QLabel())
    , line1 (new QFrame())
    , line2 (new QFrame())
    , vBoxLay1 (new QVBoxLayout())
    , vBoxLay2 (new QVBoxLayout())
    , vBoxLay3 (new QVBoxLayout())
    , vBoxLay4 (new QVBoxLayout())
    , hBoxLay1 (new QHBoxLayout())
    , hBoxLay2 (new QHBoxLayout())
    , hBoxLay3 (new QHBoxLayout())
    , hBoxLay4 (new QHBoxLayout())
{
    initWindow();

    initLayout();

    initStyle();

    initConnect();
}

KYCFunctionBarWidget::~KYCFunctionBarWidget()
{

}

void KYCFunctionBarWidget::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Z) && (e->modifiers() ==  Qt::ControlModifier)) {
        if (!stack.isEmpty()) {
            QString flagName = stack.pop();
            if (flagName == "flagOcr") {
                flagOcr = 0;
                if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
                    btnOcr->setStyleSheet("QPushButton{image: url(:/icon/icon/ocr.svg);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                          "QPushButton:hover{image: url(:/icon/icon/ocr.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                          "QPushButton:checked{image: url(:/icon/icon/ocr.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
                } else {
                    btnOcr->setStyleSheet("QPushButton{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#E7E7E7;border-radius:6px;}"
                                          "QPushButton:hover{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                          "QPushButton:checked{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
                }
                emit sendOcrEnd();
            }
            if (flagName == "flagBeautify") {
                flagBeautify = 0;
                if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
                    btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify.svg);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                               "QPushButton:hover{image: url(:/icon/icon/beautify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                               "QPushButton:checked{image: url(:/icon/icon/beautify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
                } else {
                    btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#E7E7E7;border-radius:6px;}"
                                               "QPushButton:hover{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                               "QPushButton:checked{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");

                }
                emit sendBeautifyEnd();
            }
            if (flagName == "flagRectify") {
                flagRectify = 0;
                if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
                    btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify.svg);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                              "QPushButton:hover{image: url(:/icon/icon/rectify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                              "QPushButton:checked{image: url(:/icon/icon/rectify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
                } else {
                    btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#E7E7E7;border-radius:6px;}"
                                              "QPushButton:hover{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                              "QPushButton:checked{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
                }
                sendRectifyEnd();
            }
        }
    }
}

void KYCFunctionBarWidget::initWindow()
{
    setFixedHeight(80);
    setMinimumWidth(860);
}

void KYCFunctionBarWidget::initLayout()
{
    getFileListNum();

    btnScan->setCursor (Qt::PointingHandCursor); // 可点击鼠标样式

    btnNorScan->setFixedSize(60, 32);
    btnBeautify->setFixedSize(60, 32);
    btnRectify->setFixedSize(60, 32);
    btnOcr->setFixedSize(60, 32);
    btnScan->setFixedSize(56, 56);
    btnScan->setText(tr("Scan"));

    labMovieScan->setFixedSize(56, 56);
    labMovieScan->hide();
//    movieScan->setFileName(":/icon/icon/scanner.gif");
//    movieScan->setCacheMode(QMovie::CacheAll);
//    QSize size = labMovieScan->size();
//    //movieScan->setScaledSize(size);
    //labMovieScan->setMovie(movieScan);

    setFontSize(labNorScan, 10);
    setFontSize(labBeautify, 10);
    setFontSize(labRectify, 10);
    setFontSize(labOcr, 10);
    labNorScan->setText(tr("Normal scanning"));
    labNorScan->setAlignment(Qt::AlignCenter);
    labNorScan->setFixedSize(60, 16);

    labBeautify->setText(tr("Beauty"));
    labBeautify->setAlignment(Qt::AlignCenter);
    labBeautify->setFixedSize(56, 16);

    labRectify->setText(tr("Rectify"));
    labRectify->setAlignment(Qt::AlignCenter);
    labRectify->setFixedSize(56, 16);

    labOcr->setText(tr("Text OCR"));
    labOcr->setAlignment(Qt::AlignCenter);
    labOcr->setFixedSize(56, 16);

    line1->setObjectName(QString::fromUtf8("line1"));
    line1->setMaximumWidth(1);
    line1->setMaximumHeight(48);
    line1->setMinimumHeight(48);
    line1->setFrameShape(QFrame::VLine);

    line2->setObjectName(QString::fromUtf8("line2"));
    line2->setMaximumWidth(1);
    line2->setMaximumHeight(48);
    line2->setMinimumHeight(48);
    line2->setFrameStyle(QFrame::VLine);

#ifdef DEBUG_EDIT
    setKylinScanSetEnable();
#else
    setKylinScanSetNotEnable();
#endif

    // 先隐藏
    btnNorScan->hide();
    labNorScan->hide();
    /*
    vBoxLay1->setSpacing(0);
    vBoxLay1->addSpacing(2);
    vBoxLay1->addWidget(btnNorScan);
    vBoxLay1->addSpacing(10);
    vBoxLay1->addWidget(labNorScan);
    // vBoxLay1->addSpacing(10);
    */

    vBoxLay2->setSpacing(0);
    vBoxLay2->addSpacing(2);
    vBoxLay2->addWidget(btnBeautify);
    vBoxLay2->addSpacing(10);
    vBoxLay2->addWidget(labBeautify);
    //  vBoxLay2->addSpacing(10);


    vBoxLay3->setSpacing(0);
    vBoxLay3->addSpacing(2);
    vBoxLay3->addWidget(btnRectify);
    vBoxLay3->addSpacing(10);
    vBoxLay3->addWidget(labRectify);


    vBoxLay4->setSpacing(0);
    vBoxLay4->addSpacing(2);
    vBoxLay4->addWidget(btnOcr);
    vBoxLay4->addSpacing(10);
    vBoxLay4->addWidget(labOcr);

    hBoxLay1->setSpacing(0);
    hBoxLay1->addStretch();
    hBoxLay1->addLayout(vBoxLay1);
    hBoxLay1->setContentsMargins(189, 0, 26, 0);

    hBoxLay2->setSpacing(0);
    hBoxLay2->addLayout(vBoxLay2);
    hBoxLay2->addSpacing(26);
    hBoxLay2->addLayout(vBoxLay3);
    hBoxLay2->addSpacing(26);
    hBoxLay2->addLayout(vBoxLay4);
    hBoxLay2->setContentsMargins(26, 0, 30, 0);

    hBoxLay3->setSpacing(0);
    hBoxLay3->addWidget(labMovieScan);
    hBoxLay3->addSpacing(0);
    hBoxLay3->addWidget(btnScan);
    hBoxLay3->setContentsMargins(30, 0, 189, 0);
    hBoxLay3->addStretch();

    hBoxLay4->setSpacing(0);
    hBoxLay4->addLayout(hBoxLay1);
    //hBoxLay4->addWidget(line1);
    hBoxLay4->addLayout(hBoxLay2);
    hBoxLay4->addWidget(line2);
    hBoxLay4->addLayout(hBoxLay3);

    setLayout(hBoxLay4);
}

void KYCFunctionBarWidget::initStyle()
{
    stylelist << STYLE_NAME_KEY_DARK << STYLE_NAME_KEY_BLACK;
    iconthemelist << ICON_THEME_KEY_BASIC << ICON_THEME_KEY_CLASSICAL << ICON_THEME_KEY_DEFAULT;

    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        // 黑色主题需要用白的图标
        btnNorScan->setStyleSheet("QPushButton{image: url(:/icon/icon/norscan.png);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/norscan.png);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                  "QPushButton:checked{image: url(:/icon/icon/norscan.png);border:none;background-color:#3D6BE5;border-radius:6px;}");
        btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify.svg);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                   "QPushButton:hover{image: url(:/icon/icon/beautify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                   "QPushButton:checked{image: url(:/icon/icon/beautify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify.svg);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/rectify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                  "QPushButton:checked{image: url(:/icon/icon/rectify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        btnOcr->setStyleSheet("QPushButton{image: url(:/icon/icon/ocr.svg);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                              "QPushButton:hover{image: url(:/icon/icon/ocr.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                              "QPushButton:checked{image: url(:/icon/icon/ocr.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");

        btnScan->setStyleSheet("QPushButton{background-color: rgb(232,160,73);border-radius:28px;color:rgb(232,232,232);}");

        line2->setStyleSheet("QFrame{color:#201E1D}");
    } else {
        btnNorScan->setStyleSheet("QPushButton{image: url(:/icon/icon/norscan.png);border:none;background-color:#E7E7E7;border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/norscan.png);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                  "QPushButton:checked{image: url(:/icon/icon/norscan.png);border:none;background-color:#3D6BE5;border-radius:6px;}");
        btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#E7E7E7;border-radius:6px;}"
                                   "QPushButton:hover{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                   "QPushButton:checked{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#E7E7E7;border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                  "QPushButton:checked{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        btnOcr->setStyleSheet("QPushButton{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#E7E7E7;border-radius:6px;}"
                              "QPushButton:hover{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                              "QPushButton:checked{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");

        btnScan->setStyleSheet("QPushButton{background-color: rgb(232,160,73);border-radius:28px;color:rgb(232,232,232);}");

        line2->setStyleSheet("QFrame{color:#DCDCDC}");
    }


}

void KYCFunctionBarWidget::initConnect()
{
    // For ocr
    connect(btnOcr, SIGNAL(clicked()), this, SLOT(onBtnOcrClicked()));

    // For scan
    connect(btnScan, SIGNAL(clicked()), this, SLOT(onBtnScanClicked()));
    connect(&thread, SIGNAL(scanFinishedFuncBar(int)), this, SLOT(scanResult(int)));

    // For rectify
    connect(btnRectify, SIGNAL(clicked()), this, SLOT(onBtnRectifyClicked()));

    // For beauty
    connect(btnBeautify, SIGNAL(clicked()), this, SLOT(onBtnBeautyClicked()));

    connect(style_settings, SIGNAL(changed(QString)), this, SLOT(funcbar_style_changed(QString)));

    connect(time, SIGNAL(timeout()), this, SLOT(showPictures()));
}

void KYCFunctionBarWidget::getFileListNum()
{
    path = ":/icon/icon/scanning/";
    num = GetFileList(path).size();
    qDebug() << "num = " << num;
    count = 0;
}

void KYCFunctionBarWidget::setKylinScanSetNotEnable()
{
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    bool device_status = true;

    device_status = instance.getKylinSaneStatus();
    qDebug() << "device_status = " << device_status;

    if (!device_status) {
        btnNorScan->setEnabled(false);
        btnBeautify->setEnabled(false);
        btnRectify->setEnabled(false);
        btnOcr->setEnabled(false);
        btnScan->setEnabled(false);
    } else {
        btnNorScan->setEnabled(true);
        btnBeautify->setEnabled(true);
        btnRectify->setEnabled(true);
        btnOcr->setEnabled(true);
        btnScan->setEnabled(true);
    }
}

void KYCFunctionBarWidget::setKylinScanSetEnable()
{
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    bool device_status = true;

    device_status = instance.getKylinSaneStatus();

    if (device_status) {
        btnNorScan->setEnabled(true);
        btnBeautify->setEnabled(true);
        btnRectify->setEnabled(true);
        btnOcr->setEnabled(true);
        btnScan->setEnabled(true);
    }
}

void KYCFunctionBarWidget::setBtnScanEnable()
{
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    bool device_status = true;

    device_status = instance.getKylinSaneStatus();

    if (device_status) {
        btnScan->setEnabled(true);
    }
}

void KYCFunctionBarWidget::setFontSize(QLabel *label, int n)
{
    QFont ft;
    ft.setPointSize(n);
    label->setFont(ft);
}

void KYCFunctionBarWidget::setStackClear()
{
    stack.clear();
    flagOcr = 0;
    flagRectify = 0;
    flagBeautify = 0;
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        btnOcr->setStyleSheet("QPushButton{image: url(:/icon/icon/ocr.svg);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                              "QPushButton:hover{image: url(:/icon/icon/ocr.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                              "QPushButton:checked{image: url(:/icon/icon/ocr.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");

        btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify.svg);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/rectify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                  "QPushButton:checked{image: url(:/icon/icon/rectify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");

        btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify.svg);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                   "QPushButton:hover{image: url(:/icon/icon/beautify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                   "QPushButton:checked{image: url(:/icon/icon/beautify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
    } else {
        btnOcr->setStyleSheet("QPushButton{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#E7E7E7;border-radius:6px;}"
                              "QPushButton:hover{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                              "QPushButton:checked{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");

        btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#E7E7E7;border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                  "QPushButton:checked{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");

        btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#E7E7E7;border-radius:6px;}"
                                   "QPushButton:hover{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                   "QPushButton:checked{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
    }
}

void KYCFunctionBarWidget::warnMsg(QString msg)
{
    QMessageBox msgBox(QMessageBox::Warning, QObject::tr("warning"), msg);
    msgBox.setWindowIcon(QIcon::fromTheme("kylin-scanner"));
    msgBox.exec();
}

QFileInfoList KYCFunctionBarWidget::GetFileList(QString path)
{
    QDir dir(path);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (int i = 0; i != folder_list.size(); i++) {
        QString name = folder_list.at(i).absoluteFilePath();
        QFileInfoList child_file_list = GetFileList(name);
        file_list.append(child_file_list);
    }

    return file_list;
}

//QString ocr_text;
void KYCFunctionBarWidget::onBtnOcrClicked()
{
    if (flagOcr == 0) {
        flagOcr = 1;
        stack.push("flagOcr");
        if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
            btnOcr->setStyleSheet("QPushButton{image: url(:/icon/icon/ocr.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        } else {
            btnOcr->setStyleSheet("QPushButton{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        }


        emit sendOcrBegin();
    } else {
        flagOcr = 0;
        stack.pop();
        if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
            btnOcr->setStyleSheet("QPushButton{image: url(:/icon/icon/ocr.svg);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/ocr.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                  "QPushButton:checked{image: url(:/icon/icon/ocr.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");

        } else {
            btnOcr->setStyleSheet("QPushButton{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#E7E7E7;border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                  "QPushButton:checked{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        }

        emit sendOcrEnd();
    }
}

void KYCFunctionBarWidget::onBtnScanClicked()
{
    qDebug() << "clicked!";
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    if (instance.getKylinSaneStatus() == true) {
        thread.start();
        qDebug() << "btnScan: " << btnScan->size() << btnScan->pos() << btnScan->geometry();
        emit clickBtnScanStart();

        btnScan->hide();
        btnScan->resize(0, 0);
        labMovieScan->resize(56, 56);
        labMovieScan->show();
        time->start(100);
        count = 0;
        //movieScan->start();

        qDebug() << "scan()";
    }

}

void KYCFunctionBarWidget::onBtnRectifyClicked()
{
    qDebug() << "send_Rectify_Begin";
    if (flagRectify == 0) {
        flagRectify = 1;
        stack.push("flagRectify");
        if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
            btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        } else {
            btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        }
        emit sendRectifyBegin();
    } else {
        flagRectify = 0;
        stack.pop();

        if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
            btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify.svg);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                      "QPushButton:hover{image: url(:/icon/icon/rectify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                      "QPushButton:checked{image: url(:/icon/icon/rectify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        } else {
            btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#E7E7E7;border-radius:6px;}"
                                      "QPushButton:hover{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                      "QPushButton:checked{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        }
        emit sendRectifyEnd();
    }
}


void KYCFunctionBarWidget::onBtnBeautyClicked()
{
    qDebug() << "flagBeauty = " << flagBeautify;
    if (flagBeautify == 0) {
        flagBeautify = 1;
        stack.push("flagBeautify");
        if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
            btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        } else {
            btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        }
        emit sendBeautifyBegin();
    } else {
        flagBeautify = 0;
        stack.pop();
        if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
            btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify.svg);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                       "QPushButton:hover{image: url(:/icon/icon/beautify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                       "QPushButton:checked{image: url(:/icon/icon/beautify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        } else {
            btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#E7E7E7;border-radius:6px;}"
                                       "QPushButton:hover{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                       "QPushButton:checked{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        }
        emit sendBeautifyEnd();
    }
}

void KYCFunctionBarWidget::scanResult(int ret)
{
    qDebug() << "scanResult(0:success) = " << ret;
    btnScan->show();
    btnScan->setText(tr("scan"));
    btnScan->setStyleSheet("QPushButton{background-color: rgb(232,160,73);border-radius:28px;color:rgb(232,232,232);}");

    //movieScan->stop();
    time->stop();
    labMovieScan->resize(0, 0);
    labMovieScan->hide();

    if (ret == 0)
        emit clickBtnScanEnd(true);
    else
        emit clickBtnScanEnd(false);
}

void KYCFunctionBarWidget::funcbar_style_changed(QString)
{
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        btnNorScan->setStyleSheet("QPushButton{image: url(:/icon/icon/norscan.png);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/norscan.png);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                  "QPushButton:checked{image: url(:/icon/icon/norscan.png);border:none;background-color:#3D6BE5;border-radius:6px;}");
        btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify.svg);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                   "QPushButton:hover{image: url(:/icon/icon/beautify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                   "QPushButton:checked{image: url(:/icon/icon/beautify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify.svg);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/rectify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                  "QPushButton:checked{image: url(:/icon/icon/rectify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        btnOcr->setStyleSheet("QPushButton{image: url(:/icon/icon/ocr.svg);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                              "QPushButton:hover{image: url(:/icon/icon/ocr.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                              "QPushButton:checked{image: url(:/icon/icon/ocr.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");

        btnScan->setStyleSheet("QPushButton{background-color: rgb(232,160,73);border-radius:28px;color:rgb(232,232,232);}");

        line2->setStyleSheet("QFrame{color:#201E1D}");
    } else {
        btnNorScan->setStyleSheet("QPushButton{image: url(:/icon/icon/norscan.png);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/norscan.png);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                  "QPushButton:checked{image: url(:/icon/icon/norscan.png);border:none;background-color:#3D6BE5;border-radius:6px;}");
        btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#E7E7E7;border-radius:6px;}"
                                   "QPushButton:hover{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                   "QPushButton:checked{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#E7E7E7;border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                  "QPushButton:checked{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        btnOcr->setStyleSheet("QPushButton{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#E7E7E7;border-radius:6px;}"
                              "QPushButton:hover{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                              "QPushButton:checked{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");

        btnScan->setStyleSheet("QPushButton{background-color: rgb(232,160,73);border-radius:28px;color:rgb(232,232,232);}");

        line2->setStyleSheet("QFrame{color:#DCDCDC}");
    }


    if (flagOcr == 1) { // 在主题变换时，正在文字识别的按钮应该还是选中状态
        if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
            btnOcr->setStyleSheet("QPushButton{image: url(:/icon/icon/ocr.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        } else {
            btnOcr->setStyleSheet("QPushButton{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        }
    } else {
        if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
            btnOcr->setStyleSheet("QPushButton{image: url(:/icon/icon/ocr.svg);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/ocr.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                  "QPushButton:checked{image: url(:/icon/icon/ocr.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");

        } else {
            btnOcr->setStyleSheet("QPushButton{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#E7E7E7;border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                  "QPushButton:checked{image: url(:/icon/icon/ocr_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        }
    }

    if (flagRectify == 1) {
        if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
            btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        } else {
            btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        }
    } else {
        if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
            btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify.svg);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                      "QPushButton:hover{image: url(:/icon/icon/rectify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                      "QPushButton:checked{image: url(:/icon/icon/rectify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        } else {
            btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#E7E7E7;border-radius:6px;}"
                                      "QPushButton:hover{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                      "QPushButton:checked{image: url(:/icon/icon/rectify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        }
    }

    if (flagBeautify == 1) {
        if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
            btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        } else {
            btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        }
    } else {
        if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
            btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify.svg);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                       "QPushButton:hover{image: url(:/icon/icon/beautify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                       "QPushButton:checked{image: url(:/icon/icon/beautify.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        } else {
            btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#E7E7E7;border-radius:6px;}"
                                       "QPushButton:hover{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}"
                                       "QPushButton:checked{image: url(:/icon/icon/beautify_black.svg);border:none;background-color:#3D6BE5;border-radius:6px;}");
        }
    }
}

void KYCFunctionBarWidget::showPictures()
{
    QImage image;
    fileinfo = GetFileList(path).at(count);
    //qDebug() << "filepath = " << fileinfo.filePath();
    if (!image.load(fileinfo.filePath()))
        qDebug() << "fail!";

    image.scaled(labMovieScan->size(), Qt::KeepAspectRatio);
    labMovieScan->setScaledContents(true);
    labMovieScan->setPixmap(QPixmap::fromImage(image));
    ++count;
    if (count == num)
        count = 0;
}

void KYCFuctionBarThread::run()
{
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    int ret;
#ifdef DEBUG_EDIT

    ret = 0;
    emit scanFinishedFuncBar(ret);
    quit();
#else
    if (instance.getKylinSaneStatus() == true) {
        ret = instance.startScanning(instance.userInfo);
        qDebug() << "start_scanning end, status = " << ret;
        emit scanFinishedFuncBar(ret);
    }
    quit();
#endif
}

