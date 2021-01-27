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

#ifndef KYCSaveFileDialog_H
#define KYCSaveFileDialog_H

#include <QWidget>
#include <QFileDialog>
#include <QPoint>

class KYCSaveFileDialog : public QFileDialog
{
    Q_OBJECT
public:
    explicit KYCSaveFileDialog(QWidget *parent = nullptr);

    // flag = 1: ocr dialog
    explicit KYCSaveFileDialog(QWidget *parent = nullptr, int flag = 0,
                               QString filename = nullptr, QString titlename = nullptr);

    QString getFileName();
    QString getFileType();

    void kycSetDirectory(QString directory);
    void kycSetFilename(QString filename);

private:
    QString filetype;
    QString initFilename;
};

#endif // KYCSaveFileDialog_H
