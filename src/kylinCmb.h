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

#ifndef KYLINCOMBOBOX_H
#define KYLINCOMBOBOX_H

#include <QComboBox>
#include <QMouseEvent>
#include <QToolTip>
#include "theme.h"

typedef struct styleUnit {
    QString styleCommonWhite;
    QString styleCommonBlack;
    QString styleInitWhite;
    QString styleInitBlack;
    QString styleGrayWhite;
    QString styleGrayBlack;
    QString styleNormalWhite;
    QString styleNormalBlack;
} styleUnit;

class KylinCmb : public QComboBox
{
    Q_OBJECT

public:
    explicit KylinCmb(QWidget *parent = nullptr);
    ~KylinCmb();

    void initStyleUnit();
    void colorInit();
    void colorGray();
    void colorNormal();

protected:
    virtual void mousePressEvent(QMouseEvent *e); // 添加鼠标点击事件
    virtual void hidePopup(); // 重载隐藏下拉框
    virtual void showPopup(); // 重载显示下拉框

private:
    QStringList stylelist;
    QStringList iconthemelist;
    QGSettings *style_settings;
    QGSettings *icon_theme_settings;
    styleUnit styleunit;

signals:
    void clicked();

};

#endif // KYLINCOMBOBOX_H
