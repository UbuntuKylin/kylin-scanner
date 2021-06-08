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

#include "runningdialog.h"

KYCRunningDialog::KYCRunningDialog(QWidget *parent)
    : QDialog(parent)
    , svghandler (new SVGHandler())
    , time (new QTimer())
    , style_settings (new QGSettings(ORG_UKUI_STYLE))
    , waitImage (new QLabel())
    , waitText (new QLabel())
    , btnCancel (new QPushButton())
    , hLayoutInfo (new QHBoxLayout())
    , hLayoutCancel (new QHBoxLayout())
    , vLayout (new QVBoxLayout())
{
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    KYCXAtomHelperObject::getInstance()->setWindowMotifHint(winId(), hints);

    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    stylelist << STYLE_NAME_KEY_DARK << STYLE_NAME_KEY_BLACK;
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(0, 0, 0, 1));
        setAutoFillBackground(true);
        setPalette(pal);
    } else {
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(255, 255, 255, 1));
        setAutoFillBackground(true);
        setPalette(pal);
    }

    waitImage->setFixedSize(30, 30);
    waitText->setText("");
    hLayoutInfo->setSpacing(0);
    hLayoutInfo->addWidget(waitImage);
    hLayoutInfo->addSpacing(2);
    hLayoutInfo->addWidget(waitText);
    hLayoutInfo->addStretch();
    hLayoutInfo->setAlignment(Qt::AlignCenter);
    hLayoutInfo->setContentsMargins(32, 0, 0, 0); // 只对左边布局进行设置

    btnCancel->setFixedSize(100, 36);
    btnCancel->setText(tr("Cancel"));
    hLayoutCancel->setSpacing(0);
    hLayoutCancel->addStretch();
    hLayoutCancel->addWidget(btnCancel);
    hLayoutCancel->addStretch();
    hLayoutCancel->setAlignment(Qt::AlignCenter);
    hLayoutCancel->setContentsMargins(248, 0, 32, 0); // 对左右布局进行设置

    vLayout->setSpacing(0);
    vLayout->addSpacing(33); // 对上布局进行设置
    vLayout->addLayout(hLayoutInfo);
    vLayout->addSpacing(28); // 对上布局进行设置
    vLayout->addLayout(hLayoutCancel);
    vLayout->setContentsMargins(0, 0, 0, 25); // 对下布局进行设置

    setLayout(vLayout);

    getFileListNum();
    time->start(50);
    count = 0;
    waitImage->show();

    connect(time, SIGNAL(timeout()), this, SLOT(showPictures()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(accept()));
    connect(style_settings, SIGNAL(changed(QString)), this, SLOT(runningDialogStyleChanged(QString)));

}

KYCRunningDialog::KYCRunningDialog(QWidget *parent, QString text)
    : QDialog(parent)
    , svghandler (new SVGHandler())
    , time (new QTimer())
    , style_settings (new QGSettings(ORG_UKUI_STYLE))
    , waitImage (new QLabel())
    , waitText (new QLabel())
    , btnCancel (new QPushButton())
    , hLayoutInfo (new QHBoxLayout())
    , hLayoutCancel (new QHBoxLayout())
    , vLayout (new QVBoxLayout())
{
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    KYCXAtomHelperObject::getInstance()->setWindowMotifHint(winId(), hints);

    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    stylelist << STYLE_NAME_KEY_DARK << STYLE_NAME_KEY_BLACK;
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(0, 0, 0));
        setAutoFillBackground(true);
        setPalette(pal);
    } else {
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(255, 255, 255));
        setAutoFillBackground(true);
        setPalette(pal);
    }

    getFileListNum();
    waitImage->show();
    time->start(200);
    count = 0;

    waitImage->setFixedSize(30, 30);
    waitText->setText(text);
    hLayoutInfo->setSpacing(0);
    hLayoutInfo->addWidget(waitImage);
    hLayoutInfo->addSpacing(2);
    hLayoutInfo->addWidget(waitText);
    hLayoutInfo->setAlignment(Qt::AlignLeft);
    hLayoutInfo->setContentsMargins(32, 0, 0, 0);

    btnCancel->setFixedSize(100, 36);
    btnCancel->setText(tr("Cancel"));
    hLayoutCancel->setSpacing(0);
    hLayoutCancel->addStretch();
    hLayoutCancel->addWidget(btnCancel);
    hLayoutCancel->setAlignment(Qt::AlignCenter);
    hLayoutCancel->setContentsMargins(248, 0, 32, 0);

    vLayout->setSpacing(0);
    vLayout->addSpacing(33);
    vLayout->addLayout(hLayoutInfo);
    vLayout->addSpacing(28);
    vLayout->addLayout(hLayoutCancel);
    vLayout->setContentsMargins(0, 0, 0, 25);

    setLayout(vLayout);

    connect(time, SIGNAL(timeout()), this, SLOT(showPictures()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(accept()));
    connect(style_settings, SIGNAL(changed(QString)), this, SLOT(runningDialogStyleChanged(QString)));
}


void KYCRunningDialog::getFileListNum()
{
    path = ":/icon/icon/waiting/";
    num = GetFileList(path).size();
    count = 0;
}

QFileInfoList KYCRunningDialog::GetFileList(QString path)
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

void KYCRunningDialog::runningDialogStyleChanged(QString)
{
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(0, 0, 0));
        setAutoFillBackground(true);
        setPalette(pal);

    } else {
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(255, 255, 255));
        setAutoFillBackground(true);
        setPalette(pal);
    }
}

void KYCRunningDialog::showPictures()
{
    QImage image;
    fileinfo = GetFileList(path).at(count);
    waitImage->setScaledContents(true);
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        waitImage->setPixmap(svghandler->loadSvgColor(fileinfo.filePath(), "white", 30));
    } else {
        waitImage->setPixmap(svghandler->loadSvgColor(fileinfo.filePath(), "black", 30));
    }
    ++count;
    if (count == num)
        count = 0;
}
