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

#include "timewait.h"

KYCRealTimeLabel::KYCRealTimeLabel(QWidget *parent) : QLabel(parent)
{

}

KYCRealTimeLabel::~KYCRealTimeLabel()
{

}

void KYCRealTimeLabel::paintEvent(QPaintEvent *event)
{
    //先调用父类的paintEvent
    QLabel::paintEvent(event);

    QPainter painter(this);
    QImage image(data, width, height, QImage::Format_RGB888);
    painter.drawImage(QRect(0, 0, width, height), image);
}
