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

#ifndef THEME_H
#define THEME_H

#include <QWidget>
#include <QGSettings/qgsettings.h>
#include <QWidget>
#include <QStringList>

#define ORG_UKUI_STYLE            "org.ukui.style"

#define STYLE_NAME                "styleName"
#define STYLE_NAME_KEY_DARK       "ukui-dark"
#define STYLE_NAME_KEY_DEFAULT    "ukui-default"
#define STYLE_NAME_KEY_BLACK      "ukui-black"
#define STYLE_NAME_KEY_LIGHT      "ukui-light"
#define STYLE_NAME_KEY_WHITE      "ukui-white"

#define ICON_THEME_NAME           "iconThemeName"
#define ICON_THEME_KEY_BASIC      "ukui-icon-theme-basic"
#define ICON_THEME_KEY_CLASSICAL  "ukui-icon-theme-classical"
#define ICON_THEME_KEY_DEFAULT    "ukui-icon-theme-default"

#define ICON_THEME_CLOSE "window-close-symbolic"
#define ICON_THEME_MINIMIZE "window-minimize-symbolic"
#define ICON_THEME_MAXIMAZE "window-maximize-symbolic"
#define ICON_THEME_MENU "open-menu-symbolic"
#define ICON_THEME_RESTORE "window-restore-symbolic"

class KYCThemeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KYCThemeWidget(QWidget *parent = nullptr);

signals:

};

#endif // THEME_H
