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

#include "kylinLog.h"
#include "singleApplication.h"
#include <QtNetwork/QLocalSocket>
#include <QFileInfo>

#define TIME_OUT                (500)    // 500ms

SingleApplication::SingleApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , w(NULL)
    , _isRunning(false)
    , _localServer(NULL) {

    // 取应用程序名作为LocalServer的名字
    _serverName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();

    _initLocalConnection();
}

/**
 * @brief SingleApplication::isRunning
 * 检查是否已經有一个实例在运行, true - 有实例运行， false - 没有实例运行
 * @return
 */
bool SingleApplication::isRunning()
{
    return _isRunning;
}

/**
 * @brief SingleApplication::_newLocalConnection
 * 通过socket通讯实现程序单实例运行，监听到新的连接时触发该函数
 */
void SingleApplication::_newLocalConnection()
{
    QLocalSocket *socket = _localServer->nextPendingConnection();
    if (socket) {
        socket->waitForReadyRead(2*TIME_OUT);
        delete socket;

        // 其他处理，如：读取启动参数

        _activateWindow();
    }
}

/**
 * @brief SingleApplication::_initLocalConnection
 * 通过socket通讯实现程序单实例运行，
 * 初始化本地连接，如果连接不上server，则创建，否则退出
 */
void SingleApplication::_initLocalConnection()
{
    _isRunning = false;

    QLocalSocket socket;
    socket.connectToServer(_serverName);

    if (socket.waitForConnected(TIME_OUT)) {
        fprintf(stderr, "%s already running.\n",
                _serverName.toLocal8Bit().constData());
        _isRunning = true;
        MYLOG << "isRunnig = " << _isRunning;
        // 其他处理，如：将启动参数发送到服务端
        return;
    } else {
        MYLOG << "isRunnig = " << _isRunning;
    }

    //连接不上服务器，就创建一个
    _newLocalServer();
}

/**
 * @brief SingleApplication::_newLocalServer
 * 创建LocalServer
 */
void SingleApplication::_newLocalServer()
{
    _localServer = new QLocalServer(this);
    connect(_localServer, SIGNAL(newConnection()), this, SLOT(_newLocalConnection()));

    if (!_localServer->listen(_serverName)) {
        // 此时监听失败，可能是程序崩溃时,残留进程服务导致的,移除之
        if (_localServer->serverError() == QAbstractSocket::AddressInUseError) {
            QLocalServer::removeServer(_serverName); // <-- 重点
            _localServer->listen(_serverName); // 再次监听
        }
    }
}

/**
 * @brief SingleApplication::_activateWindow
 * 激活主窗口
 */
void SingleApplication::_activateWindow()
{
    if (w) {
        MYLOG << "activateWindow";
        w->show();
        w->raise();
        w->activateWindow(); // 激活窗口
    }
}
