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

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <QWidget>
#include <QDialog>
#include <QBitmap>
#include <QPalette>
#include <QPainter>
#include <QPixmap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <QPushButton>
#include <QLabel>
#include <QString>

class KYCInterruptDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KYCInterruptDialog(QWidget *parent = nullptr);
    ~KYCInterruptDialog();

    void setDialogAttrbutes();
    void setBtnCloseAttributes();
    void setlabInfoAttributes();
    void setBtnKnowAttributes();

private:
    QString strWarnInfo;
    QPushButton *btnClose;
    QPushButton *btnKnow;
    QLabel *labInfo;
    QLabel *labWarnImage;
    QHBoxLayout *hlayoutClose;
    QHBoxLayout *hlayoutInfo;
    QHBoxLayout *hlayoutKnow;
    QVBoxLayout *vlayoutInterrupt;
};

#endif // INTERRUPT_H
