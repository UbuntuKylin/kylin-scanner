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

#ifndef DAEMONDBUS_H
#define DAEMONDBUS_H

#define KYLIN_USER_GUIDE_PATH "/"

#define KYLIN_USER_GUIDE_SERVICE "com.kylinUserGuide.hotel"

#define KYLIN_USER_GUIDE_INTERFACE "com.guide.hotel"

#define SERVICE_NAME_SIZE 30

#include <QObject>
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusReply>
#include <unistd.h>
#include <sys/types.h>

class KYCDaemonDbusObject : public QObject
{
    Q_OBJECT
    // 定义Interface名称为com.scorpio.test.value
    // Q_CLASSINFO("D-Bus Interface", "com.scorpio.test.value")
public:

    KYCDaemonDbusObject() {}

public slots:

    int daemonIsNotRunning();
    void showGuide(QString appName);

};

#endif // DAEMONDBUS_H
