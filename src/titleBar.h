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

#ifndef TITLE_BAR_H
#define TITLE_BAR_H

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QPainter>
#include "theme.h"

class TitleBar : public QDialog
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);
    ~TitleBar();

protected:
    // 双击标题栏进行界面的最大化/还原
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

    // 进行鼠界面的拖动
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

    // 设置界面标题与图标
    virtual bool eventFilter(QObject *obj, QEvent *event);

    virtual void keyPressEvent(QKeyEvent *e);

private:
    // 最大化/还原
    void updateMaximize();

    bool mMoving;
    bool flagMaxWindow;
    QPoint mLastMousePosition;
    QPoint offset;
    QStringList stylelist;
    QStringList iconthemelist;

    QGSettings *style_settings;
    QGSettings *icon_theme_settings;
    QLabel *m_logo;
    QLabel *m_logoMsg;
    QPushButton *m_pMinimizeButton;
    QPushButton *m_pMaximizeButton;
    QPushButton *m_pCloseButton;
    QHBoxLayout *pLayout;

private slots:
    // 进行最小化、最大化/还原、关闭操作
    void onClicked();
    void titlebar_icon_theme_changed(QString); // 系统图标主题风格变化
    void titlebar_style_changed(QString); // 系统黑白主题样式变换

Q_SIGNALS:
    void isNormal();
    void isMax();
};
#endif // TITLE_BAR_H
