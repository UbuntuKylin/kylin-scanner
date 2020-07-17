/*
* Copyright (C) 2020, Tianjin KYLIN Information Technology Co., Ltd.
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

class KylinComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit KylinComboBox(QWidget *parent = nullptr);
    ~KylinComboBox();

    void colorGray();
    void colorNormal();

protected:
    virtual void mousePressEvent(QMouseEvent *e); // 添加鼠标点击事件
    virtual void hidePopup(); // 重载隐藏下拉框
    virtual void showPopup(); // 重载显示下拉框


signals:
    void clicked();  // 自定义点击信号，在mousePressEvent事件发生时触发

public slots:
};

#endif // KYLINCOMBOBOX_H
