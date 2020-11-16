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

#include "kylinCmb.h"

KylinCmb::KylinCmb(QWidget *parent) : QComboBox(parent)
    , style_settings (new QGSettings(ORG_UKUI_STYLE))
    , icon_theme_settings (new QGSettings(ORG_UKUI_STYLE))
{
    stylelist << STYLE_NAME_KEY_DARK << STYLE_NAME_KEY_BLACK << STYLE_NAME_KEY_DEFAULT;
    iconthemelist << ICON_THEME_KEY_BASIC << ICON_THEME_KEY_CLASSICAL << ICON_THEME_KEY_DEFAULT;

    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        QComboBox::setStyleSheet(
                    "QComboBox{background-color:#0D0400;color:white;border-radius:4px;}"
                    "QComboBox QLineEdit{border-radius:4px;}"
                    "QComboBox:hover{border-radius:4px;background-color:#0D0400;color:white;}"
                    "QComboBox::drop-down{border-radius:4px;}" //下拉按钮
                    "QToolTip { height: 32px; background: #000000; border: 1px solid #383838; border-radius: 4px;font-size: 14px; font-weight: 400; color: #F0F0F0; line-height: 52px;} " // tooltip提示
                    "QComboBox::down-arrow{border-radius:4px;border-image:url(:/icon/icon/down.svg);}"//下拉箭头
                    "QComboBox QAbstractItemView{margin-top:4px solid #0D0400;color:white;border-radius:4px;background-color:#888888;}"
                    "QComboBox QAbstractItemView::item{width:180px;height: 32px;border-radius:4px;}"//下拉列表框样式
                    "QComboBox QAbstractItemView::item:hover:selected{background-color:#3D6BE5;}"
                    );
    } else {
        QComboBox::setStyleSheet(
                    "QComboBox{border-radius:4px;background-color:#E7E7E7;color:#000000;}"
                    "QComboBox QLineEdit{border-radius:4px;}"
                    "QComboBox:hover{border-radius:4px;background-color:#E7E7E7;color:#000000;}"
                    "QComboBox::drop-down{border-radius:4px;}" //下拉按钮
                    "QToolTip { height: 32px; background:#E7E7E7; border: 1px solid #E7E7E7; border-radius: 4px;font-size: 14px; font-weight: 400; color:#000000; line-height: 52px;} " // tooltip提示
                    "QComboBox::down-arrow{border-radius:4px;border-image:url(:/icon/icon/down_black.svg);}"//下拉箭头
                    "QComboBox QAbstractItemView{background-color:#FFFFFF;color:#000000;border-radius:4px;}" // 下拉的背景颜色由color指定
                    "QComboBox QAbstractItemView::item{background-color:#FFFFFF;color:#000000;width:180px;height:32px;border-radius:4px;}"//下拉列表框样式
                    "QComboBox QAbstractItemView::item:hover:selected{background-color:#E7E7E7;color:#000000;}"
                    );
    }

    QComboBox::setEditable(false);
}
KylinCmb::~KylinCmb()
{

}

void KylinCmb::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked();  //触发clicked信号
    }

    QComboBox::mousePressEvent(event);  //将该事件传给父类处理，这句话很重要，如果没有，父类无法处理本来的点击事件
}

void KylinCmb::hidePopup()
{
    QComboBox::setEditable(false);
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        QComboBox::setStyleSheet(
                    "QComboBox{border-radius:4px;background-color:#0D0400;color:white;}"
                    "QComboBox QLineEdit{border-radius:4px;}"
                    "QComboBox:hover{border-radius:4px;background-color:#0D0400;color:white;}"
                    "QComboBox::drop-down{border-radius:4px;}" //下拉按钮
                    "QToolTip { height: 32px; background: #000000; border: 1px solid #383838; border-radius: 4px;font-size: 14px; font-weight: 400; color: #F0F0F0; line-height: 52px;} " // tooltip提示
                    "QComboBox::down-arrow{border-radius:4px;border-image:url(:/icon/icon/down.svg);}"//下拉箭头
                    "QComboBox QAbstractItemView{margin-top:4px solid #0D0400;color:white;border-radius:4px;background-color:#888888;}"
                    "QComboBox QAbstractItemView::item{width:180px;height: 32px;border-radius:4px;}"//下拉列表框样式
                    "QComboBox QAbstractItemView::item:hover:selected{background-color:#3D6BE5;}"
                    );
    } else {
        QComboBox::setStyleSheet(
                    "QComboBox{border-radius:4px;background-color:#E7E7E7;color:#000000;}"
                    "QComboBox QLineEdit{border-radius:4px;}"
                    "QComboBox:hover{border-radius:4px;background-color:#E7E7E7;color:#000000;}"
                    "QComboBox::drop-down{border-radius:4px;}" //下拉按钮
                    "QToolTip { height: 32px; background: #E7E7E7; border: 1px solid #E7E7E7; border-radius: 4px;font-size: 14px; font-weight: 400; color: #000000; line-height: 52px;} " // tooltip提示
                    "QComboBox::down-arrow{border-radius:4px;border-image:url(:/icon/icon/down_black.svg);}"//下拉箭头
                    "QComboBox QAbstractItemView{background-color:#FFFFFF;color:#000000;border-radius:4px;}"
                    "QComboBox QAbstractItemView::item{background-color:#FFFFFF;color:#000000;width:180px;height:32px;border-radius:4px;}"//下拉列表框样式
                    "QComboBox QAbstractItemView::item:hover:selected{background-color:#E7E7E7;color:#000000;}"
                    );
    }

    QComboBox::hidePopup();
}

/**
 * @brief KylinCmb::showPopup
 * 正常点击出现的下拉框
 */
void KylinCmb::showPopup()
{
    QComboBox::setEditable(true);
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        QComboBox::setStyleSheet(
                    "QComboBox{border:2px solid #3D6BE5;border-radius:4px;background-color:#0D0400;color:white;}"
                    "QComboBox QLineEdit{border-radius:4px;}"
                    "QComboBox:hover{border-radius:4px;background-color:#0D0400;color:white;}"
                    "QComboBox::drop-down{border-radius:4px;}" //下拉按钮
                    "QToolTip { height: 32px; background: #000000; border: 1px solid #383838; border-radius: 4px;font-size: 14px; font-weight: 400; color: #F0F0F0; line-height: 52px;} " // tooltip提示
                    "QComboBox::down-arrow{border-radius:4px;border-image:url(:/icon/icon/down.svg);}"//下拉箭头
                    "QComboBox QAbstractItemView{margin-top:4px solid #0D0400;color:white;border-radius:4px;background-color:#888888;}"
                    "QComboBox QAbstractItemView::item{width:180px;height: 32px;border-radius:4px;}"//下拉列表框样式
                    "QComboBox QAbstractItemView::item:hover:selected{background-color:#3D6BE5;}"
                    );
    } else {
        QComboBox::setStyleSheet(
                    "QComboBox{border:2px solid #3D6BE5;border-radius:4px;background-color:#E7E7E7;color:#000000;}"
                    "QComboBox QLineEdit{background-color:#E7E7E7;border-radius:4px;}"
                    "QComboBox:hover{border-radius:4px;background-color:#E7E7E7;color:#000000;}"
                    "QComboBox::drop-down{border-radius:4px;}" //下拉按钮
                    "QToolTip { height:32px;background:#E7E7E7;border:1px solid #E7E7E7;border-radius:4px;font-size:14px;font-weight:400;color:#000000;line-height:52px;} " // tooltip提示
                    "QComboBox::down-arrow{border-radius:4px;border-image:url(:/icon/icon/down_black.svg);}"//下拉箭头
                    "QComboBox QAbstractItemView{background-color:#FFFFFF;color:#000000;border-radius:4px;}"
                    "QComboBox QAbstractItemView::item{background-color:#FFFFFF;color:#000000;width:180px;height:32px;border-radius:4px;}"//下拉列表框样式
                    "QComboBox QAbstractItemView::item{background-color:#FFFFFF;color:#000000;width:180px;height:32px;border-radius:4px;}"//下拉列表框样式
                    "QComboBox QAbstractItemView::item:hover:selected{background-color:#E7E7E7;color:#000000;}"
                    );
    }

    QComboBox::showPopup();
}

/**
 * @brief KylinCmb::colorGray
 * 按钮不可用时，字体为gray
 */
void KylinCmb::colorGray()
{
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        this->setStyleSheet(
                    "QComboBox{border-radius:4px;background-color:#0D0400;color:gray;}"
                    "QComboBox QLineEdit{border-radius:4px;}"
                    "QComboBox:hover{border-radius:4px;background-color:#0D0400;color:gray;}"
                    "QComboBox::drop-down{border-radius:4px;}" //下拉按钮
                    "QToolTip { height: 32px; background: #000000; border: 1px solid #383838; border-radius: 4px;font-size: 14px; font-weight: 400; color:gray; line-height: 52px;} " // tooltip提示
                    "QComboBox::down-arrow{border-radius:4px;border-image:url(:/icon/icon/down.svg);}"//下拉箭头
                    "QComboBox QAbstractItemView{margin-top:4px solid #0D0400;color:gray;border-radius:4px;background-color:#888888;}"
                    "QComboBox QAbstractItemView::item{width:180px;height: 32px;border-radius:4px;}"//下拉列表框样式
                    "QComboBox QAbstractItemView::item:hover:selected{background-color:#3D6BE5;}"
                    );
    } else {
        this->setStyleSheet(
                    "QComboBox{border-radius:4px;background-color:#E7E7E7;color:gray;}"
                    "QComboBox QLineEdit{border-radius:4px;}"
                    "QComboBox:hover{border-radius:4px;background-color:#E7E7E7;color:gray;}"
                    "QComboBox::drop-down{border-radius:4px;}" //下拉按钮
                    "QToolTip { height: 32px; background: #E7E7E7; border: 1px solid #E7E7E7; border-radius: 4px;font-size: 14px; font-weight: 400; color: #000000; line-height: 52px;} " // tooltip提示
                    "QComboBox::down-arrow{border-radius:4px;border-image:url(:/icon/icon/down_black.svg);}"//下拉箭头
                    "QComboBox QAbstractItemView{background-color:#FFFFFF;color:gray;border-radius:4px;}"
                    "QComboBox QAbstractItemView::item{background-color:#FFFFFF;color:gray;width:180px;height:32px;border-radius:4px;}"//下拉列表框样式
                    "QComboBox QAbstractItemView::item:hover:selected{background-color:#E7E7E7;color:gray;}"
                    );
    }
    this->setEditable(false);
    this->setFocusPolicy(Qt::NoFocus);
}

void KylinCmb::colorNormal()
{
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        this->setStyleSheet(
                    "QComboBox{border-radius:4px;background-color:#0D0400;color:white;}"
                    "QComboBox QLineEdit{border-radius:4px;}"
                    "QComboBox:hover{border-radius:4px;background-color:#0D0400;color:white;}"
                    "QComboBox::drop-down{border-radius:4px;}" //下拉按钮
                    "QToolTip { height: 32px; background: #000000; border: 1px solid #383838; border-radius: 4px;font-size: 14px; font-weight: 400; color: #F0F0F0; line-height: 52px;} " // tooltip提示
                    "QComboBox::down-arrow{border-radius:4px;border-image:url(:/icon/icon/down.svg);}"//下拉箭头
                    "QComboBox QAbstractItemView{margin-top:4px solid #0D0400;color:white;border-radius:4px;background-color:#888888;}"
                    "QComboBox QAbstractItemView::item{width:180px;height: 32px;border-radius:4px;}"//下拉列表框样式
                    "QComboBox QAbstractItemView::item:hover:selected{background-color:#3D6BE5;}"
                    );
    } else {
        this->setStyleSheet(
                    "QComboBox{border-radius:4px;background-color:#E7E7E7;color:#000000;}"
                    "QComboBox QLineEdit{border-radius:4px;}"
                    "QComboBox:hover{border-radius:4px;background-color:#E7E7E7;color:#000000;}"
                    "QComboBox::drop-down{border-radius:4px;}" //下拉按钮
                    "QToolTip { height: 32px; background: #E7E7E7; border: 1px solid #E7E7E7; border-radius: 4px;font-size: 14px; font-weight: 400; color: #000000; line-height: 52px;} " // tooltip提示
                    "QComboBox::down-arrow{border-radius:4px;border-image:url(:/icon/icon/down_black.svg);}"//下拉箭头
                    "QComboBox QAbstractItemView{background-color:#FFFFFF;color:#000000;border-radius:4px;}"
                    "QComboBox QAbstractItemView::item{background-color:#FFFFFF;color:#000000;width:180px;height:32px;border-radius:4px;}"//下拉列表框样式
                    "QComboBox QAbstractItemView::item:hover:selected{background-color:#E7E7E7;color:#000000;}"
                    );
    }
    this->setEditable(false);
}
