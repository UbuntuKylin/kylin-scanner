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

#include "savefile.h"
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

KYCSaveFileDialog::KYCSaveFileDialog(QWidget *parent, int ocrFlag, QString filename, QString titlename)
{
    Q_UNUSED(parent);

    setWindowIcon(QIcon::fromTheme("kylin-scanner"));
    setWindowTitle(titlename);
    setAcceptMode(QFileDialog::AcceptSave);

    QFileInfo filenameInfo(filename);
    qDebug() << "filename = " << filename;

    this->findChildren<QLineEdit *>("fileNameEdit").at(0)->setText(filename);

    if (ocrFlag == 1) {
        filetype = ".txt";
        filter = QLatin1String("*.txt");
    } else {
        qDebug() << "First choose save format: " << filenameInfo.suffix();
        if (filename.endsWith(".jpg")) {
            filetype = ".jpg";
            filter = QLatin1String("*.jpg;;*.png;;*.pdf;;*.bmp");
        } else if (filename.endsWith(".png")) {
            filetype = ".png";
            filter = QLatin1String("*.png;;*.jpg;;*.pdf;;*.bmp");
        } else if (filename.endsWith(".pdf")) {
            filetype = ".pdf";
            filter = QLatin1String("*.pdf;;*.jpg;;*.png;;*.bmp");
        } else {
            filetype = ".bmp";
            filter = QLatin1String("*.bmp;;*.jpg;;*.png;;*.pdf");
        }
    }
    setNameFilter(filter);
    qDebug() << "filetype = " << filetype;
    qDebug() << "filter = " << filter
             << "0: " << filter.mid(1, 4)
             << "1: " << filter.mid(8, 4)
             << "2: " << filter.mid(15, 4)
             << "3: " << filter.mid(22, 4);

    connect(this->findChildren<QComboBox *>("fileTypeCombo").at(0),
    QOverload<int>::of(&QComboBox::currentIndexChanged), [ = ](int index) {
        qDebug() << "currentIndexChanged: index = " << index;
        switch (index) {
        case 0:
            if (ocrFlag == 1) {
                filetype = ".txt";
            } else {
                filetype = filter.mid(1, 4);
            }
            break;
        case 1:
            filetype = filter.mid(8, 4);
            break;
        case 2:
            filetype = filter.mid(15, 4);
            break;
        case 3:
            filetype = filter.mid(22, 4);
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
