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

#include "customcombobox.h"

KYCComboBox::KYCComboBox(QWidget *parent) : QComboBox(parent)
    , style_settings (new QGSettings(ORG_UKUI_STYLE))
    , icon_theme_settings (new QGSettings(ORG_UKUI_STYLE))
{
    stylelist << STYLE_NAME_KEY_DARK << STYLE_NAME_KEY_BLACK;
    iconthemelist << ICON_THEME_KEY_BASIC << ICON_THEME_KEY_CLASSICAL << ICON_THEME_KEY_DEFAULT;

    initStyleUnit();

    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        QComboBox::setStyleSheet(styleunit.styleInitBlack);
    } else {
        QComboBox::setStyleSheet(styleunit.styleInitWhite);
    }

    QComboBox::setEditable(false);
}

KYCComboBox::~KYCComboBox()
{
}

void KYCComboBox::initStyleUnit()
{
    // white theme
    styleunit.styleCommonWhite = "QComboBox QLineEdit{background-color:#E7E7E7;border-radius:4px;}"
                                 "QComboBox::drop-down{border-radius:4px;}" //下拉按钮
                                 "QComboBox::down-arrow{border-radius:4px;border-image:url(:/icon/icon/down_black.svg);}"
                                 "QToolTip {height:32px;background:#E7E7E7;border:1px solid #E7E7E7;border-radius:4px;font-size:14px;font-weight:400;color:#000000;line-height:52px;} ";

    styleunit.styleInitWhite = styleunit.styleCommonWhite +
                               "QComboBox{border-radius:4px;background-color:#E7E7E7;color:#000000;}"
                               "QComboBox:hover{border-radius:4px;background-color:#E7E7E7;color:#000000;}"
                               "QComboBox QAbstractItemView{background-color:#FFFFFF;color:#000000;border-radius:4px;}" // 下拉的背景颜色由color指定
                               "QComboBox QAbstractItemView::item{background-color:#FFFFFF;color:#000000;width:180px;height:32px;border-radius:4px;}"//下拉列表框样式
                               "QComboBox QAbstractItemView::item:hover:selected{background-color:#E7E7E7;color:#000000;}";
    styleunit.styleGrayWhite = styleunit.styleCommonWhite +
                               "QComboBox{border-radius:4px;background-color:#E7E7E7;color:gray;}"
                               "QComboBox:hover{border-radius:4px;background-color:#E7E7E7;color:gray;}"
                               "QComboBox QAbstractItemView{background-color:#FFFFFF;color:gray;border-radius:4px;}"
                               "QComboBox QAbstractItemView::item{background-color:#FFFFFF;color:gray;width:180px;height:32px;border-radius:4px;}"
                               "QComboBox QAbstractItemView::item:hover:selected{background-color:#E7E7E7;color:gray;}";
    styleunit.styleNormalWhite = styleunit.styleCommonWhite +
                                 "QComboBox{border:2px solid #3D6BE5;border-radius:4px;background-color:#E7E7E7;color:#000000;}"
                                 "QComboBox:hover{border-radius:4px;background-color:#E7E7E7;color:#000000;}"
                                 "QComboBox QAbstractItemView{background-color:#FFFFFF;color:#000000;border-radius:4px;}"
                                 "QComboBox QAbstractItemView::item{background-color:#FFFFFF;color:#000000;width:180px;height:32px;border-radius:4px;}"
                                 "QComboBox QAbstractItemView::item:hover:selected{background-color:#E7E7E7;color:#000000;}";

    // black theme
    styleunit.styleCommonBlack = "QComboBox QLineEdit{background-color:#0D0400;border-radius:4px;}"
                                 "QComboBox::drop-down{border-radius:4px;}"
                                 "QComboBox::down-arrow{border-radius:4px;border-image:url(:/icon/icon/down.svg);}"
                                 "QToolTip {height:32px;background:#000000;border:1px solid #383838;border-radius:4px;font-size:14px;font-weight:400;color:gray;line-height:52px;}";

    styleunit.styleInitBlack = styleunit.styleCommonBlack +
                               "QComboBox{background-color:#0D0400;color:white;border-radius:4px;}"
                               "QComboBox:hover{border-radius:4px;background-color:#0D0400;color:white;}"
                               "QComboBox QAbstractItemView{margin-top:4px solid #0D0400;color:white;border-radius:4px;background-color:#888888;}"
                               "QComboBox QAbstractItemView::item{width:180px;height:32px;border-radius:4px;}"
                               "QComboBox QAbstractItemView::item:hover:selected{background-color:#3D6BE5;}";
    styleunit.styleGrayBlack = styleunit.styleCommonBlack +
                               "QComboBox{border-radius:4px;background-color:#0D0400;color:gray;}"
                               "QComboBox:hover{border-radius:4px;background-color:#0D0400;color:gray;}"
                               "QComboBox QAbstractItemView{margin-top:4px solid #0D0400;color:gray;border-radius:4px;background-color:#888888;}"
                               "QComboBox QAbstractItemView::item{width:180px;height: 32px;border-radius:4px;}"
                               "QComboBox QAbstractItemView::item:hover:selected{background-color:#3D6BE5;}";
    styleunit.styleNormalBlack = styleunit.styleCommonBlack +
                                 "QComboBox{border:2px solid #3D6BE5;border-radius:4px;background-color:#0D0400;color:white;}"
                                 "QComboBox:hover{border-radius:4px;background-color:#0D0400;color:white;}"
                                 "QComboBox QAbstractItemView{margin-top:4px solid #0D0400;color:white;border-radius:4px;background-color:#888888;}"
                                 "QComboBox QAbstractItemView::item{width:180px;height: 32px;border-radius:4px;}"
                                 "QComboBox QAbstractItemView::item:hover:selected{background-color:#3D6BE5;}";
}

void KYCComboBox::colorInit()
{
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        QComboBox::setStyleSheet(styleunit.styleInitBlack);
    } else {
        QComboBox::setStyleSheet(styleunit.styleInitWhite);
    }
}

void KYCComboBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }

    QComboBox::mousePressEvent(event);
}

void KYCComboBox::hidePopup()
{
    QComboBox::setEditable(false);
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        QComboBox::setStyleSheet(styleunit.styleInitBlack);
    } else {
        QComboBox::setStyleSheet(styleunit.styleInitWhite);
    }

    QComboBox::hidePopup();
}

/**
 * @brief KylinCmb::showPopup
 * 正常点击出现的下拉框
 */
void KYCComboBox::showPopup()
{
    QComboBox::setEditable(true);
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        QComboBox::setStyleSheet(styleunit.styleNormalBlack);
    } else {
        QComboBox::setStyleSheet(styleunit.styleNormalWhite);
    }

    QComboBox::showPopup();
}

/**
 * @brief KylinCmb::colorGray
 * 按钮不可用时，字体为gray
 */
void KYCComboBox::colorGray()
{
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        this->setStyleSheet(styleunit.styleGrayBlack);
    } else {
        this->setStyleSheet(styleunit.styleGrayWhite);
    }
    this->setEditable(false);
    this->setFocusPolicy(Qt::NoFocus);
}

void KYCComboBox::colorNormal()
{
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        this->setStyleSheet(styleunit.styleNormalBlack);
    } else {
        this->setStyleSheet(styleunit.styleNormalWhite);
    }
    this->setEditable(false);
}
