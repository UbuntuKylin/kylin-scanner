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
#ifndef TARLOR_H
#define TARLOR_H

#include <QLabel>
#include <QMouseEvent>
#include <QString>
#include <QShowEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QRect>
#include <QCursor>
#include <QApplication>
#include <QPixmap>
#include <QImage>
#include <QDebug>

#define EPLISION 1e-6

class KYCTailorLabel  :   public QLabel
{
    Q_OBJECT

public:
    KYCTailorLabel(QLabel *parent = nullptr);
    ~KYCTailorLabel();

    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    int judgePosition(int origin, int min, int max);

    inline int getStartX() const {
        return startX;
    }

    inline int getStartY() const {
        return startY;
    }

    inline int getEndX() const {
        return endX;
    }

    inline int getEndY() const {
        return endY;
    }

private:
    bool isPressed;
    int startX;
    int startY;
    int endX;
    int endY;
};

#endif // TARLOR_H
