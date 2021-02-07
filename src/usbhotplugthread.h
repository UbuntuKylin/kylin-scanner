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

#ifndef USBHOTPLUGTHREAD_H
#define USBHOTPLUGTHREAD_H

#include <QObject>
#include <QThread>
#include <QString>

class USBHotplugThread : public QThread
{
    Q_OBJECT
public:
    USBHotplugThread();
    void run() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void usbRemove(QString);
    void usbAdd(QString);
};

#endif // USBHOTPLUGTHREAD_H
