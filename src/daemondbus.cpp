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

#include "daemondbus.h"

int KYCDaemonDbusObject::daemonIsNotRunning()
{
    char service_name[SERVICE_NAME_SIZE];
    memset(service_name, 0, SERVICE_NAME_SIZE);
    snprintf(service_name, SERVICE_NAME_SIZE, "%s_%d", KYLIN_USER_GUIDE_SERVICE, getuid());
    QDBusConnection conn = QDBusConnection::sessionBus();
    if (!conn.isConnected())
        return -1;

    QDBusReply<QString> reply = conn.interface()->call("GetNameOwner", service_name);
    return reply.value() == "";
}

void KYCDaemonDbusObject::showGuide(QString appName)
{
    qDebug() << Q_FUNC_INFO << appName;
    bool bRet  = false;

    char service_name[SERVICE_NAME_SIZE];
    memset(service_name, 0, SERVICE_NAME_SIZE);
    snprintf(service_name, SERVICE_NAME_SIZE, "%s_%d", KYLIN_USER_GUIDE_SERVICE, getuid());

    qDebug() << "service_name  " << service_name;
    // 构造一个在D-Bus上传递的Message
    QDBusMessage msg = QDBusMessage::createMethodCall(QString(service_name), KYLIN_USER_GUIDE_PATH,
                                                      KYLIN_USER_GUIDE_INTERFACE, "showGuide");
    // 给QDBusMessage增加一个参数;
    msg << appName;

    // 发送Message
    QDBusMessage response = QDBusConnection::sessionBus().call(msg);
    // 判断Method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage) {
        // QDBusMessage的arguments不仅可以用来存储发送的参数，也用来存储返回值;
        //bRet = response.arguments().at(0).toBool();
        qDebug() << "showGuide is success!\n";
    } else {
        qDebug() << "showGuide is fail!\n";
    }

    qDebug() << "bRet:" << bRet;
}
