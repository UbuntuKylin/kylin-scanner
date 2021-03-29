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

#ifndef USBHOTPLUG_H
#define USBHOTPLUG_H

#include <QObject>
#include <QThread>
#include <QString>

class KYCUsbHotplugThread : public QThread
{
    Q_OBJECT
public:
    KYCUsbHotplugThread();
    void run() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void usbRemove(QString);
    void usbAdd(QString);
};

#endif // USBHOTPLUG_H
