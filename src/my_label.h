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
#ifndef MY_LABEL_H
#define MY_LABEL_H
#include <QLabel>
#include <QMouseEvent>
#include <QString>
#include <QShowEvent>

class my_label  :   public QLabel
{
//    Q_OBJECT
public:
    my_label(QLabel *parent = nullptr);
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    int x1, y1, x2, y2;

    void savePicture(QString path, int filename);

public:
    int getX1() {return x1;}
    int getY1() {return y1;}
    int getX2() {return x2;}
    int getY2() {return y2;}

};

#endif // MY_LABEL_H
