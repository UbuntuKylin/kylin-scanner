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

#include "kycsavefiledialog.h"
#include <QStandardPaths>
#include <QLineEdit>
#include <QComboBox>
#include <QDebug>
#include <QLineEdit>
#include <QPoint>

KYCSaveFileDialog::KYCSaveFileDialog(QWidget *parent) : QFileDialog(parent)
{
    setWindowIcon(QIcon::fromTheme("kylin-scanner"));
    setNameFilter(QLatin1String("*.jpg;;*.png;;*.pdf;;*.bmp"));
    setAcceptMode(QFileDialog::AcceptSave);
    this->findChildren<QLineEdit *>("fileNameEdit").at(0)->setText(initFilename);
    filetype = ".jpg";
    connect(this->findChildren<QComboBox *>("fileTypeCombo").at(0),
    QOverload<int>::of(&QComboBox::currentIndexChanged), [ = ](int index) {
        switch (index) {
        case 0:
            filetype = ".jpg";
            break;
        case 1:
            filetype = ".png";
            break;
        case 2:
            filetype = ".pdf";
            break;
        case 3:
            filetype = ".bmp";
            break;
        default:
            break;
        }
    });
}

KYCSaveFileDialog::KYCSaveFileDialog(QWidget *parent, int flag, QString filename, QString titlename)
{
    Q_UNUSED(parent);
    setWindowIcon(QIcon::fromTheme("kylin-scanner"));
    QString filter = (flag == 1) ? QLatin1String("*.txt") : QLatin1String("*.jpg;;*.png;;*.pdf;;*.bmp");
    setNameFilter(filter);
    setWindowTitle(titlename);
    setAcceptMode(QFileDialog::AcceptSave);
    this->findChildren<QLineEdit *>("fileNameEdit").at(0)->setText(filename);
    filetype = (flag == 1) ? "*.txt" : ".jpg";
    connect(this->findChildren<QComboBox *>("fileTypeCombo").at(0),
    QOverload<int>::of(&QComboBox::currentIndexChanged), [ = ](int index) {
        switch (index) {
        case 0:
            filetype = (flag == 1) ? "*.txt" : ".jpg";
            break;
        case 1:
            filetype = ".png";
            break;
        case 2:
            filetype = ".pdf";
            break;
        case 3:
            filetype = ".bmp";
            break;
        default:
            break;
        }
    });
}

QString KYCSaveFileDialog::getFileType()
{
    return filetype;
}

void KYCSaveFileDialog::kycSetDirectory(QString directory)
{
    setDirectory(directory);
}

void KYCSaveFileDialog::kycSetFilename(QString filename)
{
    this->initFilename = filename;
}


QString KYCSaveFileDialog::getFileName()
{
    return this->findChildren<QLineEdit *>("fileNameEdit").at(0)->text();
}
