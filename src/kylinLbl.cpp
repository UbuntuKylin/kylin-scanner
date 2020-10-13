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
#include "kylinLbl.h"

KylinLbl::KylinLbl(QLabel *parent)
    : QLabel(parent)
{
    x1 = 0;
    y1 = 0;
    x2 = 0;
    y2 = 0;
}

void KylinLbl::paintEvent(QPaintEvent *event)
{
    //comment before
    QLabel::paintEvent(event); //绘制背景的图片
    QPainter painter(this);
    painter.setPen(QPen(Qt::green, 1));
    painter.drawRect(QRect(x1, y1, x2 - x1, y2 - y1));
}

/**
 * @brief KylinLbl::mousePressEvent
 * 裁切时的起始坐标
 * @param event
 */
void KylinLbl::mousePressEvent(QMouseEvent *event)
{
    x1 = event->pos().x();
    y1 = event->pos().y();
    QCursor cursor;
    cursor.setShape(Qt::ClosedHandCursor);
    QApplication::setOverrideCursor(cursor);
}

/**
 * @brief KylinLbl::mouseReleaseEvent
 * 裁切时的终止坐标
 * @param event
 */
void KylinLbl::mouseReleaseEvent(QMouseEvent *event)
{
    x2 = event->pos().x(); //鼠标相对于所在控件的位置
    y2 = event->pos().y();
    update();
    QApplication::restoreOverrideCursor();
}

void KylinLbl::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        x2 = event->pos().x(); //鼠标相对于所在控件的位置
        y2 = event->pos().y();
        update();
    }
}
