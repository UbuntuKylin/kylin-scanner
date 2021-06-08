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

#include "watermark.h"

KYCWaterMarkDialog::KYCWaterMarkDialog(QWidget *parent) :
    QDialog(parent)
    , style_settings (new QGSettings(ORG_UKUI_STYLE))
    , icon_theme_settings (new QGSettings(ORG_UKUI_STYLE))
    , label (new QLabel())
    , lineedit (new QLineEdit())
    , line (new QFrame())
    , btnOk (new QPushButton())
    , btnCancel (new QPushButton())
    , hBoxLayout (new QHBoxLayout())
    , vBoxLayout (new QVBoxLayout(this))
{
    initWindow();

    initLayout();

    initStyle();

    initConnect();
}

KYCWaterMarkDialog::~KYCWaterMarkDialog()
{
}

void KYCWaterMarkDialog::initWindow()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setFixedSize(320, 250);
    setWindowMask ();
}

void KYCWaterMarkDialog::initLayout()
{
    label->setText(tr("Input watermark content"));
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label->setFixedSize(192, 32);
    QFont ft;
    ft.setPointSize(16);
    label->setFont(ft);

    lineedit->setText("");
    lineedit->setFixedSize(288, 32);

    line->setObjectName(QString::fromUtf8("line"));
    line->setMaximumHeight(1);
    line->setMaximumWidth(288);
    line->setMinimumWidth(288);
    line->setFrameShape(QFrame::HLine);

    btnOk->setText(tr("Confirm"));
    btnOk->setFixedSize(120, 36);
    btnCancel->setText(tr("Cancel"));
    btnCancel->setFixedSize(100, 36);

    hBoxLayout->setSpacing(0);
    hBoxLayout->addWidget(btnOk);
    hBoxLayout->addSpacing(16);
    hBoxLayout->addWidget(btnCancel);
    hBoxLayout->setContentsMargins(16, 0, 16, 0);

    vBoxLayout->setSpacing(0);
    vBoxLayout->addWidget(label);
    vBoxLayout->addSpacing(32);
    vBoxLayout->addWidget(lineedit);
    vBoxLayout->addSpacing(22);
    vBoxLayout->addWidget(line);
    vBoxLayout->addSpacing(22);
    vBoxLayout->addLayout(hBoxLayout);
    vBoxLayout->setContentsMargins(16, 30, 16, 30);

    setLayout(vBoxLayout);
}

void KYCWaterMarkDialog::initStyle()
{
    stylelist << STYLE_NAME_KEY_DARK << STYLE_NAME_KEY_BLACK;
    iconthemelist << ICON_THEME_KEY_BASIC << ICON_THEME_KEY_CLASSICAL << ICON_THEME_KEY_DEFAULT;

    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        QPalette pal(palette()); // 水印对话框背景
        pal.setColor(QPalette::Background, QColor(47, 44, 43));
        setAutoFillBackground(true);
        this->setPalette(pal);

        label->setStyleSheet("color:rgb(232,232,232);");
        lineedit->setStyleSheet("QLineEdit{background-color:rgb(15,08,01);color:rgb(232,232,232);border-radius:6px;}");
        line->setStyleSheet("QFrame{color:rgb(32,30,29)}");
    } else {
        QPalette pal(palette()); // 水印对话框背景
        pal.setColor(QPalette::Background, QColor(255, 255, 255));
        setAutoFillBackground(true);
        this->setPalette(pal);

        label->setStyleSheet("color:rgb(15,8,1);");
        lineedit->setStyleSheet("QLineEdit{background-color:#E7E7E7;color:rgb(15,8,1);border-radius:6px;}");
        line->setStyleSheet("QFrame{color:#000000}");
        line->hide (); // 显示线条不好看
    }
}

void KYCWaterMarkDialog::initConnect()
{
    connect(btnOk, SIGNAL(clicked()), this, SLOT(doAccept()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(style_settings, SIGNAL(changed(QString)), this, SLOT(watermarkdlg_style_changed(QString)));
}

QString KYCWaterMarkDialog::getLineEdit()
{
    return lineedit->text();
}

void KYCWaterMarkDialog::setWindowMask()
{
    QBitmap bitMap(width(), height()); // A bit map has the same size with current widget
    bitMap.fill();
    QPainter painter(&bitMap);
    painter.setBrush(Qt::red);
    painter.setPen(Qt::NoPen); // Any color that is not QRgb(0,0,0) is right
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawRoundedRect(bitMap.rect(), 6, 6); //设置圆角弧度
    setMask(bitMap);
}

void KYCWaterMarkDialog::watermarkdlg_style_changed(QString)
{
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        QPalette pal(palette()); // 水印对话框背景
        pal.setColor(QPalette::Background, QColor(47, 44, 43));
        setAutoFillBackground(true);
        setPalette(pal);

        label->setStyleSheet("color:rgb(232,232,232)");
        lineedit->setStyleSheet("QLineEdit{background-color:rgb(15,08,01);color:rgb(232,232,232);border-radius:6px;}");
        line->setStyleSheet("QFrame{color:rgb(32,30,29)}");
    } else {
        QPalette pal(palette()); // 水印对话框背景
        pal.setColor(QPalette::Background, QColor(255, 255, 255));
        setAutoFillBackground(true);
        setPalette(pal);

        label->setStyleSheet("color:rgb(15,8,1);");
        lineedit->setStyleSheet("QLineEdit{background-color:#E7E7E7;color:rgb(15,8,1);border-radius:6px;}");
        line->setStyleSheet("QFrame{color:#000000}");
        line->hide ();
    }
}

void KYCWaterMarkDialog::doAccept()
{
    if (lineedit->text() == "") {
        reject();
    } else {
        accept();
    }
}
