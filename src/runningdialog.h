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

#ifndef RUNNINGDIALOG_H
#define RUNNINGDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QGSettings>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QFileInfoList>
#include <QString>
#include <QTimer>
#include <QDebug>
#include <QDir>
#include "xatomhelper.h"
#include "common.h"
#include "theme.h"
#include "svghandler.h"

#define WINDOW_WIDTH    380
#define WINDOW_HEIGHT  142

class KYCRunningDialog : public QDialog
{
    Q_OBJECT
public:
    explicit KYCRunningDialog(QWidget *parent = nullptr);
    explicit KYCRunningDialog(QWidget *parent = nullptr, QString text="");

    void getFileListNum();
    QFileInfoList GetFileList(QString path);
    void setWaitText(QString text);

private:
    int num = 0;
    int count = 0;

    SVGHandler *svghandler;

    QFileInfo fileinfo;
    QString path;
    QTimer *time;

    QString waitMsgText;

    QStringList stylelist;
    QStringList iconthemelist;
    QGSettings *style_settings;

    QLabel *waitImage;
    QLabel *waitText;
    QPushButton *btnCancel;
    QHBoxLayout *hLayoutInfo;
    QHBoxLayout *hLayoutCancel;
    QVBoxLayout *vLayout;

public slots:
    void runningDialogStyleChanged(QString);
    void showPictures();

};

#endif // RUNNINGDIALOG_H
