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

#ifndef WATERMARK_H
#define WATERMARK_H

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QDialog>
#include <QFrame>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QBitmap>
#include "theme.h"

class KYCWaterMarkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KYCWaterMarkDialog(QWidget *parent = nullptr);
    ~KYCWaterMarkDialog();
    QString getLineEdit();

    void setWindowMask();

private:
    QStringList stylelist;
    QStringList iconthemelist;

    QGSettings *style_settings;
    QGSettings *icon_theme_settings;
    QLabel *label;
    QLineEdit *lineedit;
    QFrame *line;
    QPushButton *btnOk;
    QPushButton *btnCancel;
    QHBoxLayout *hBoxLayout;
    QVBoxLayout *vBoxLayout;

signals:
    void closeDialog();

private slots:
    void watermarkdlg_style_changed(QString); // 系统黑白主题样式变换
    void doAccept();

};

#endif // WATERMARK_H
