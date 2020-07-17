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
#include "func_bar.h"

FuncBar::FuncBar(QWidget *parent)
    : QWidget(parent)
{
    setFixedHeight(80);
    setMinimumWidth(860);
    btnNorScan = new QPushButton();
    btnBeautify = new QPushButton();
    btnRectify = new QPushButton();
    btnOrc = new QPushButton();
    btnScan = new QPushButton();

    btnNorScan->setFixedSize(60,32);
    btnBeautify->setFixedSize(60,32);
    btnRectify->setFixedSize(60,32);
    btnOrc->setFixedSize(60,32);
    btnScan->setFixedSize(56,56);
    btnScan->setText(tr("Scan"));
    labNorScan = new QLabel();
    labBeautify = new QLabel();
    labRectify = new QLabel();
    labOrc = new QLabel();

    setFontSize(labNorScan,10);
    setFontSize(labBeautify,10);
    setFontSize(labRectify,10);
    setFontSize(labOrc,10);
    labNorScan->setText(tr("Normal scanning"));
    labNorScan->setAlignment(Qt::AlignCenter);
    labNorScan->setStyleSheet("color:rgb(232,232,232)");
    labNorScan->setFixedSize(60,16);

    labBeautify->setText(tr("Beauty"));
    labBeautify->setAlignment(Qt::AlignCenter);
    labBeautify->setStyleSheet("color:rgb(232,232,232)");
    labBeautify->setFixedSize(56,16);

    labRectify->setText(tr("Rectify"));
    labRectify->setAlignment(Qt::AlignCenter);
    labRectify->setStyleSheet("color:rgb(232,232,232)");
    labRectify->setFixedSize(56,16);

    labOrc->setText(tr("Text OCR"));
    labOrc->setAlignment(Qt::AlignCenter);
    labOrc->setStyleSheet("color:rgb(232,232,232)");
    labOrc->setFixedSize(56,16);


    btnNorScan->setStyleSheet("QPushButton{image: url(:/icon/icon/norscan.png);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                              "QPushButton:hover{image: url(:/icon/icon/norscan.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}"
                                "QPushButton:checked{image: url(:/icon/icon/norscan.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}");
    btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify.png);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                              "QPushButton:hover{image: url(:/icon/icon/beautify.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}"
                                "QPushButton:checked{image: url(:/icon/icon/beautify.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}");
    btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify.png);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                              "QPushButton:hover{image: url(:/icon/icon/rectify.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}"
                                "QPushButton:checked{image: url(:/icon/icon/rectify.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}");
    btnOrc->setStyleSheet("QPushButton{image: url(:/icon/icon/orc.png);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                              "QPushButton:hover{image: url(:/icon/icon/orc.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}"
                                "QPushButton:checked{image: url(:/icon/icon/orc.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}");

    btnScan->setStyleSheet("QPushButton{background-color: rgb(232,160,73);border-radius:28px;color:rgb(232,232,232);}");


    line1 = new QFrame();
    line1->setObjectName(QString::fromUtf8("line1"));
    line1->setMaximumWidth(1);
    line1->setMaximumHeight(48);
    line1->setMinimumHeight(48);
    line1->setFrameShape(QFrame::VLine);
    line1->setStyleSheet("QFrame{color:rgb(32,30,29)}");

    line2 = new QFrame();
    line2->setObjectName(QString::fromUtf8("line2"));
    line2->setMaximumWidth(1);
    line2->setMaximumHeight(48);
    line2->setMinimumHeight(48);
    line2->setFrameStyle(QFrame::VLine);
    line2->setStyleSheet("QFrame{color:rgb(32,30,29)}");

    setKylinScanSetNotEnable();

    vBoxLay1 = new QVBoxLayout();
    vBoxLay2 = new QVBoxLayout();
    vBoxLay3 = new QVBoxLayout();
    vBoxLay4 = new QVBoxLayout();

    hBoxLay1 = new QHBoxLayout();
    hBoxLay2 = new QHBoxLayout();
    hBoxLay3 = new QHBoxLayout();
    hBoxLay4 = new QHBoxLayout();

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
    vBoxLay4->addWidget(btnOrc);
    vBoxLay4->addSpacing(10);
    vBoxLay4->addWidget(labOrc);


    hBoxLay1->setSpacing(0);
    hBoxLay1->addStretch();
    hBoxLay1->addLayout(vBoxLay1);
    hBoxLay1->setContentsMargins(189,0,26,0);

    hBoxLay2->setSpacing(0);
    hBoxLay2->addLayout(vBoxLay2);
    hBoxLay2->addSpacing(26);
    hBoxLay2->addLayout(vBoxLay3);
    hBoxLay2->addSpacing(26);
    hBoxLay2->addLayout(vBoxLay4);    
    hBoxLay2->setContentsMargins(26,0,30,0);

    hBoxLay3->setSpacing(0);
    hBoxLay3->addWidget(btnScan);
    hBoxLay3->setContentsMargins(30,0,189,0);
    hBoxLay3->addStretch();

    hBoxLay4->setSpacing(0);
    hBoxLay4->addLayout(hBoxLay1);
    hBoxLay4->addWidget(line1);
    hBoxLay4->addLayout(hBoxLay2);
    hBoxLay4->addWidget(line2);
    hBoxLay4->addLayout(hBoxLay3);
    setLayout(hBoxLay4);

    // For orc
    connect(btnOrc,SIGNAL(clicked()),this,SLOT(on_btnOrc_clicked()));

    // For scan
    connect(btnScan, SIGNAL(clicked()), this, SLOT(on_btnScan_clicked()));

    // For rectify
    connect(btnRectify, SIGNAL(clicked()), this, SLOT(on_btnRectify_clicked()));

    // For beauty
    connect(btnBeautify, SIGNAL(clicked()), this, SLOT(on_btnBeauty_clicked()));
    connect(&thread,SIGNAL(scanFinished(int)),this,SLOT(scan_result(int)));
}

FuncBar::~FuncBar()
{

}

void FuncBar::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Z && e->modifiers() ==  Qt::ControlModifier)
    {
        if(!stack.isEmpty())
        {
            QString flagName = stack.pop();
            if(flagName == "flagOrc")
            {
                flagOrc = 0;
                btnOrc->setStyleSheet("QPushButton{image: url(:/icon/icon/orc.png);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                          "QPushButton:hover{image: url(:/icon/icon/orc.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}"
                                            "QPushButton:checked{image: url(:/icon/icon/orc.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}");

                emit send_Orc_End();
            }
            if(flagName == "flagBeautify")
            {
                flagBeautify = 0;
                btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify.png);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                          "QPushButton:hover{image: url(:/icon/icon/beautify.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}"
                                            "QPushButton:checked{image: url(:/icon/icon/beautify.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}");
                emit send_Beautify_End();
            }
            if(flagName == "flagRectify")
            {
                flagRectify = 0;
                btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify.png);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                          "QPushButton:hover{image: url(:/icon/icon/rectify.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}"
                                            "QPushButton:checked{image: url(:/icon/icon/rectify.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}");
                send_Rectify_End();
            }
        }
    }
}

void FuncBar::setKylinScanSetNotEnable()
{
    KylinSane& instance = KylinSane::getInstance();
    bool device_status = true;

    device_status = instance.getKylinSaneStatus();

    if(!device_status)
    {
        btnNorScan->setEnabled(false);
        btnBeautify->setEnabled(false);
        btnRectify->setEnabled(false);
        btnOrc->setEnabled(false);
        btnScan->setEnabled(false);
    }
    else
    {
        btnNorScan->setEnabled(true);
        btnBeautify->setEnabled(true);
        btnRectify->setEnabled(true);
        btnOrc->setEnabled(true);
        btnScan->setEnabled(true);
    }
}

void FuncBar::setKylinScanSetEnable()
{
    KylinSane& instance = KylinSane::getInstance();
    bool device_status = true;

    device_status = instance.getKylinSaneStatus();

    if(device_status)
    {
        btnNorScan->setEnabled(true);
        btnBeautify->setEnabled(true);
        btnRectify->setEnabled(true);
        btnOrc->setEnabled(true);
        btnScan->setEnabled(true);
    }
}

void FuncBar::setBtnScanEnable()
{
    KylinSane& instance = KylinSane::getInstance();
    bool device_status = true;

    device_status = instance.getKylinSaneStatus();

    if(device_status)
    {
        btnScan->setEnabled(true);
    }
}

void FuncBar::setFontSize(QLabel *label, int n)
{
    QFont ft;
    ft.setPointSize(n);
    label->setFont(ft);
}

void FuncBar::setStackClear()
{
        stack.clear();
        flagOrc = 0;
        flagRectify = 0;
        flagBeautify = 0;
        btnOrc->setStyleSheet("QPushButton{image: url(:/icon/icon/orc.png);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/orc.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}"
                                    "QPushButton:checked{image: url(:/icon/icon/orc.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}");

        btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify.png);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/rectify.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}"
                                    "QPushButton:checked{image: url(:/icon/icon/rectify.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}");

        btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify.png);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/beautify.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}"
                                    "QPushButton:checked{image: url(:/icon/icon/beautify.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}");
}

//QString orc_text;
void FuncBar::on_btnOrc_clicked()
{
    if(flagOrc == 0)
    {
        flagOrc = 1;
        stack.push("flagOrc");
        btnOrc->setStyleSheet("QPushButton{image: url(:/icon/icon/orc.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}");

        emit send_Orc_Begin();
    }
    else
    {
        flagOrc = 0;
        stack.pop();
        btnOrc->setStyleSheet("QPushButton{image: url(:/icon/icon/orc.png);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/orc.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}"
                                    "QPushButton:checked{image: url(:/icon/icon/orc.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}");

        emit send_Orc_End();
    }
}

void FuncBar::on_btnScan_clicked()
{
    KylinSane& instance = KylinSane::getInstance();
    if(instance.getKylinSaneStatus() == true)
    {
        thread.start();
        btnScan->setText(" ");
        btnScan->setStyleSheet("QPushButton{image: url(:/icon/icon/scanner.gif);border-radius:28px;}");
        cout << "scan()" <<endl;
    }

}

void FuncBar::on_btnRectify_clicked()
{
    qDebug()<<"send_Rectify_Begin"<<endl;
    if(flagRectify == 0)
    {
        flagRectify = 1;
        stack.push("flagRectify");
        btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}");
        emit send_Rectify_Begin();
    }
    else
    {
        flagRectify = 0;
        stack.pop();
        btnRectify->setStyleSheet("QPushButton{image: url(:/icon/icon/rectify.png);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/rectify.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}"
                                    "QPushButton:checked{image: url(:/icon/icon/rectify.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}");
        emit send_Rectify_End();
    }
}


void FuncBar::on_btnBeauty_clicked()
{
    qDebug() << "flagBeauty = " << flagBeautify;
    if(flagBeautify == 0)
    {
        flagBeautify = 1;
        stack.push("flagBeautify");
        btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}");
        emit send_Beautify_Begin();
    }
    else
    {
        flagBeautify = 0;
        stack.pop();
        btnBeautify->setStyleSheet("QPushButton{image: url(:/icon/icon/beautify.png);border:none;background-color:rgb(15,08,01);border-radius:6px;}"
                                  "QPushButton:hover{image: url(:/icon/icon/beautify.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}"
                                    "QPushButton:checked{image: url(:/icon/icon/beautify.png);border:none;background-color:rgb(39,208,127);border-radius:6px;}");
        emit send_Beautify_End();
    }
}

void FuncBar::scan_result(int ret)
{
    btnScan->setText("扫描");
    btnScan->setStyleSheet("QPushButton{background-color: rgb(232,160,73);border-radius:28px;color:rgb(232,232,232);}");
    emit send_Scan_End();
}

void threadScan::run()
{
    KylinSane& instance = KylinSane::getInstance();
    if(instance.getKylinSaneStatus() == true)
    {
        int ret = 0;
        ret = instance.start_scanning(instance.userInfo);
        qDebug()<<"start_scanning end!!!";
        emit scanFinished(ret);
    }
    quit();
}

