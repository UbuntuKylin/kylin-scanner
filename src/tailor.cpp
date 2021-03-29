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

#include "tailor.h"

KYCTailorLabel::KYCTailorLabel(QLabel *parent)
    : QLabel(parent)
{
    startX = 0;
    startY = 0;
    endX = 0;
    endY = 0;
}

KYCTailorLabel::~KYCTailorLabel()
{

}

void KYCTailorLabel::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event); // draw label background

    QPainter painter(this);
    QPen pen(Qt::green, 1);
    painter.setPen(pen);

    QPainterPath borderPath, tailorPath, outPath;

    borderPath.setFillRule(Qt::WindingFill);
    borderPath.addRect(0, 0, this->width(), this->height()); // need judge

    QRect rect(startX, startY, endX - startX, endY - startY);
    tailorPath.addRect(rect);

    painter.drawPath(tailorPath);

    outPath = borderPath.subtracted(tailorPath);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(outPath, QColor(0, 0, 0, 100));
}

void KYCTailorLabel::mousePressEvent(QMouseEvent *event)
{
    QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));

    if (event->button() == Qt::LeftButton) {
        startX = event->pos().x();
        startY = event->pos().y();
        endX = event->pos().x();
        endY = event->pos().y();

        isPressed = true;
    }
}

void KYCTailorLabel::mouseReleaseEvent(QMouseEvent *event)
{
    isPressed = false;

    if (event->buttons() & Qt::LeftButton) { // For avoid rightButton
        endX = event->pos().x();
        endY = event->pos().y();
    }

    endX = judgePosition(endX, 0, this->width());
    endY = judgePosition(endY, 0, this->height());
    update();

    QApplication::restoreOverrideCursor();
}

void KYCTailorLabel::mouseMoveEvent(QMouseEvent *event)
{
    if (isPressed && (event->buttons() & Qt::LeftButton)) {
        endX = event->pos().x();
        endY = event->pos().y();

        update();
    }
}

int KYCTailorLabel::judgePosition(int origin, int min, int max)
{
    if ((origin < min) || (origin > max))
        return min;

    return origin;
}
