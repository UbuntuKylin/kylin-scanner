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
#include "kylincombobox.h"

KylinComboBox::KylinComboBox(QWidget *parent) : QComboBox(parent)
{
    QComboBox::setStyleSheet(
                "QComboBox{border-radius:4px;background-color:#0D0400;color:white;}"
                "QComboBox QLineEdit{border-radios:4px;}"
                "QComboBox:hover{border-radius:4px;background-color:#0D0400;color:white;}"
                "QComboBox::drop-down{border-radius:4px;}" //下拉按钮
                "QComboBox::down-arrow{border-radius:4px;border-image:url(:/icon/icon/down.svg);}"//下拉箭头
                "QComboBox QAbstractItemView{margin-top:4px solid #0D0400;color:white;border-radius:4px;background-color:#888888;}"
                "QComboBox QAbstractItemView::item{width:180px;height: 32px;border-radius:4px;}"//下拉列表框样式
                "QComboBox QAbstractItemView::item:hover:selected{background-color:#6F6F6F;}"
                );
    QComboBox::setEditable(false);
}
KylinComboBox::~KylinComboBox()
{

}

void KylinComboBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked();  //触发clicked信号
    }

    QComboBox::mousePressEvent(event);  //将该事件传给父类处理，这句话很重要，如果没有，父类无法处理本来的点击事件
}

void KylinComboBox::hidePopup()
{
    QComboBox::setEditable(false);
    QComboBox::setStyleSheet(
                "QComboBox{border-radius:4px;background-color:#0D0400;color:white;}"
                "QComboBox QLineEdit{border-radios:4px;}"
                "QComboBox:hover{border-radius:4px;background-color:#0D0400;color:white;}"
                "QComboBox::drop-down{border-radius:4px;}" //下拉按钮
                "QComboBox::down-arrow{border-radius:4px;border-image:url(:/icon/icon/down.svg);}"//下拉箭头
                "QComboBox QAbstractItemView{margin-top:4px solid #0D0400;color:white;border-radius:4px;background-color:#888888;}"
                "QComboBox QAbstractItemView::item{width:180px;height: 32px;border-radius:4px;}"//下拉列表框样式
                "QComboBox QAbstractItemView::item:hover:selected{background-color:#6F6F6F;}"
                );

    QComboBox::hidePopup();
}

void KylinComboBox::showPopup()
{
    QComboBox::setEditable(true);
    QComboBox::setStyleSheet(
                "QComboBox{border:2px solid #3D6BE5;border-radius:4px;background-color:#0D0400;color:white;}"
                "QComboBox QLineEdit{border-radios:4px;}"
                "QComboBox:hover{border-radius:4px;background-color:#0D0400;color:white;}"
                "QComboBox::drop-down{border-radius:4px;}" //下拉按钮
                "QComboBox::down-arrow{border-radius:4px;border-image:url(:/icon/icon/down.svg);}"//下拉箭头
                "QComboBox QAbstractItemView{margin-top:4px solid #0D0400;color:white;border-radius:4px;background-color:#888888;}"
                "QComboBox QAbstractItemView::item{width:180px;height: 32px;border-radius:4px;}"//下拉列表框样式
                "QComboBox QAbstractItemView::item:hover:selected{background-color:#6F6F6F;}"
                );

    QComboBox::showPopup();
}

void KylinComboBox::colorGray()
{
    this->setStyleSheet(
                "QComboBox{border-radius:4px;background-color:#0D0400;color:gray;}"
                "QComboBox QLineEdit{border-radios:4px;}"
                "QComboBox:hover{border-radius:4px;background-color:#0D0400;color:white;}"
                "QComboBox::drop-down{border-radius:4px;}" //下拉按钮
                "QComboBox::down-arrow{border-radius:4px;border-image:url(:/icon/icon/down.svg);}"//下拉箭头
                "QComboBox QAbstractItemView{margin-top:4px solid #0D0400;color:white;border-radius:4px;background-color:#888888;}"
                "QComboBox QAbstractItemView::item{width:180px;height: 32px;border-radius:4px;}"//下拉列表框样式
                "QComboBox QAbstractItemView::item:hover:selected{background-color:#6F6F6F;}"
                );
    this->setEditable(false);
    this->setFocusPolicy(Qt::NoFocus);
}

void KylinComboBox::colorNormal()
{
    this->setStyleSheet(
                "QComboBox{border-radius:4px;background-color:#0D0400;color:white;}"
                "QComboBox QLineEdit{border-radios:4px;}"
                "QComboBox:hover{border-radius:4px;background-color:#0D0400;color:white;}"
                "QComboBox::drop-down{border-radius:4px;}" //下拉按钮
                "QComboBox::down-arrow{border-radius:4px;border-image:url(:/icon/icon/down.svg);}"//下拉箭头
                "QComboBox QAbstractItemView{margin-top:4px solid #0D0400;color:white;border-radius:4px;background-color:#888888;}"
                "QComboBox QAbstractItemView::item{width:180px;height: 32px;border-radius:4px;}"//下拉列表框样式
                "QComboBox QAbstractItemView::item:hover:selected{background-color:#6F6F6F;}"
                );
    this->setEditable(false);
}
