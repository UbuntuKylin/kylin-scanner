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

#include "interrupt.h"
#include "xatomhelper.h"

KYCInterruptDialog::KYCInterruptDialog(QWidget *parent) :
    QDialog(parent),
    btnClose(new QPushButton()),
    btnKnow(new QPushButton()),
    labInfo(new QLabel()),
    labWarnImage(new QLabel()),
    hlayoutClose(new QHBoxLayout()),
    hlayoutInfo(new QHBoxLayout()),
    hlayoutKnow(new QHBoxLayout()),
    vlayoutInterrupt(new QVBoxLayout())
{
    setDialogAttrbutes();
    setBtnCloseAttributes();
    setlabInfoAttributes();
    setBtnKnowAttributes();

    // For close button
    connect(btnClose, SIGNAL(clicked()), this, SLOT(reject()));
    connect(btnKnow, SIGNAL(clicked()), this, SLOT(accept()));
}

KYCInterruptDialog::~KYCInterruptDialog()
{
    delete btnKnow;
    delete btnClose;
    delete labInfo;
    delete labWarnImage;
    delete hlayoutInfo;
    delete hlayoutClose;
    delete hlayoutKnow;
    delete vlayoutInterrupt;
}

/**
 * @brief InterruptDlg::setDialogAttrbutes
 * 设置对话框窗口属性
 */
void KYCInterruptDialog::setDialogAttrbutes()
{
    setFixedSize(322, 200);
    //setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    //setWindowFlags(Qt::Dialog);
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    KYCXAtomHelperObject::getInstance()->setWindowMotifHint(winId(), hints);

    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(47, 44, 43));
    setAutoFillBackground(true);
    setPalette(pal);
}

/**
 * @brief InterruptDlg::setBtnCloseAttributes
 * 设置关闭按钮属性
 */
void KYCInterruptDialog::setBtnCloseAttributes()
{
    btnClose->setFixedSize(30, 30);
    btnClose->setStyleSheet("QPushButton{border-image: url(:/icon/icon/close_white.svg);border:none;background-color:rgb(47,44,43);border-radius:4px;}"
                            "QPushButton:hover{border-image: url(:/icon/icon/close_white.svg);border:none;background-color:rgb(240,65,52);border-radius:4px;}"
                            "QPushButton:checked{border-image: url(:/icon/icon/close_white.svg);border:none;background-color:rgb(215,52,53);border-radius:4px;}");

    hlayoutClose->setSpacing(0);
    hlayoutClose->addStretch();
    hlayoutClose->addWidget(btnClose);
    hlayoutClose->setAlignment(Qt::AlignCenter);
    hlayoutClose->setContentsMargins(0, 0, 5, 0);
}

/**
 * @brief InterruptDlg::setlabImageAttributes
 * 设置标签显示图片属性
 */
void KYCInterruptDialog::setlabInfoAttributes()
{
    labWarnImage->setFixedSize(31, 31);
    QPixmap pix = QPixmap(":/icon/svg/warn.svg");
    //pix.scaled(labWarnImage->width(), labWarnImage->height(), Qt::KeepAspectRatio);
    labWarnImage->setPixmap(pix);
    labInfo->setText("扫描设备已断开，扫描终止！");
    labInfo->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    labInfo->setStyleSheet("color: #D9FFFFFF");

    QFont font;
    font.setPixelSize(18);
    labInfo->setFont(font);

    hlayoutInfo->setSpacing(0);
    hlayoutInfo->addWidget(labWarnImage);
    hlayoutInfo->addSpacing(2);
    hlayoutInfo->addWidget(labInfo);
    hlayoutInfo->setAlignment(Qt::AlignCenter);
    hlayoutInfo->setContentsMargins(32, 0, 0, 0);
}

void KYCInterruptDialog::setBtnKnowAttributes()
{
    btnKnow->setFixedSize(120, 36);
    btnKnow->setText("知道了");
    btnKnow->setStyleSheet("QPushButton{background:#2F2C2B;border:1px solid #939393;color:rgb(232,232,232);border-radius:18px;}"
                           "QPushButton:hover{border:none;background-color:rgb(39,208,127);border:rgb(147,147,147);color:rgb(232,232,232);border-radius:18px;}"
                           "QPushButton:checked{border:none;background-color:rgb(39,208,127);border:rgb(147,147,147);color:rgb(232,232,232);border-radius:18px;}");

    hlayoutKnow->setSpacing(0);
    hlayoutKnow->addStretch();
    hlayoutKnow->addWidget(btnKnow);
    hlayoutKnow->setAlignment(Qt::AlignCenter);
    hlayoutKnow->setContentsMargins(0, 0, 32, 0);

    vlayoutInterrupt->setSpacing(0);
    vlayoutInterrupt->addLayout(hlayoutClose);
    vlayoutInterrupt->addSpacing(26);
    vlayoutInterrupt->addLayout(hlayoutInfo);
    vlayoutInterrupt->addSpacing(40);
    vlayoutInterrupt->addLayout(hlayoutKnow);
    vlayoutInterrupt->setContentsMargins(0, 0, 0, 53);

    setLayout(vlayoutInterrupt);
}
