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
#include "widget.h"
#include "singleapplication.h"
#include "xatomhelper.h"
#include "common.h"
#include <QApplication>
#include <QLabel>
#include <QTranslator>
#include <QLockFile>
#include <QDesktopWidget>
#include <X11/Xlib.h> // ought to put this file last
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QIODevice>
#include <QStandardPaths>
#include <QMessageBox>
#include <QObject>
#include <QProcess>
#include <QStandardPaths>
#include <fstream>
#include <stdio.h>

typedef struct UsedUsernameAndPid{
    QString usedUsername; 	// Exist app's currently username
    QString usedPid;		// Exist app's pid
}UsedUsernameAndPid;
UsedUsernameAndPid uuap;

static void verifyScannerDir()
{
    QString logPath(LOG_PATH);
    QDir configPath;
    if (! configPath.exists (logPath)) {
        qDebug() << "Create " << logPath;
        configPath.mkdir (logPath);
    }
}

static void KYCMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QByteArray localMsg = msg.toLocal8Bit();
    QFileInfo filePath(context.file);
    QString context_info = QString("%1:%2").arg(filePath.fileName()).arg(context.line);

    QString strMsg("");
    switch(type)
    {
    case QtDebugMsg:
        strMsg = QString("Debug:");
        break;
    case QtWarningMsg:
        strMsg = QString("Warning:");
        break;
    case QtCriticalMsg:
        strMsg = QString("Critical:");
        break;
    case QtFatalMsg:
        strMsg = QString("Fatal:");
        break;
    case QtInfoMsg:
        strMsg = QString("Info:");
        break;
    }

    // Message format
    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString strMessage = QString("%1 %2 %3: %4")
            .arg(strDateTime).arg(strMsg).arg(context_info).arg(localMsg.constData());

    QFile file(LOGFILE_PATH);
    // If not open file correctly, maybe logfile's permission(rwx) are limited.
    if (!file.open(QIODevice::ReadWrite | QIODevice::Append)) {
        qDebug() << "Cannot open file `kylin-scanner.log` .";
        return;
    }
    //file.open(QIODevice::ReadWrite | QIODevice::Append);

    // Easy to other user to modify `scanner/user.id` file
    file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner
                        | QFileDevice::ReadUser | QFileDevice::WriteUser
                        | QFileDevice::ReadGroup | QFileDevice::WriteGroup
                        | QFileDevice::ReadOther | QFileDevice::WriteOther);

    QTextStream stream(&file);
    stream << strMessage << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}

void customOutputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    /**
     * This code incorrect whill result in current user cannot open `kylin-scanner` in `ukui-menu`,
     * while other users having opened it.
     * Therefore, abandon it!
     */
    static QMutex mutex;
    mutex.lock();

    QFileInfo filePath(context.file);
    QString context_info = QString("(%1:%2)").arg(filePath.fileName()).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("[%1]").arg(current_date_time);
    QString text("");
    switch (type) {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtWarningMsg:
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:");
        break;

    case QtInfoMsg:
        text = QString("Info:");
        break;
    default:
        text = QString("");
    }
    QString message = QString("%1 %2 %3 %4").arg(text).arg(current_date).arg(context_info).arg(msg);

    verifyScannerDir();

    QFile file(LOGFILE_PATH);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Append))
        return;

    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}

static QString getCurrentUserName()
{
    QString cmd(BASH_TYPE);
    QStringList arglists;

    arglists << "-c";
    arglists << "whoami";

    QProcess *process = new QProcess();
    process->start(cmd, arglists);
    process->waitForFinished();
    QString userNow = QString::fromLocal8Bit(process->readAllStandardOutput().trimmed());
    //QString userNow = QString::fromStdString(process->readAllStandardOutput().toStdString());
    qDebug() << "Exist user: " << userNow;

    return userNow;
}

static void doWrite(QString userNow, int pidNow)
{
    QFile file(EXIST_USERNAME_AND_PID_PATH);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        qDebug() << "open this file error!";
        return;
    }

    // Easy to other user to modify `scanner/user.id` file
    file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner
                        | QFileDevice::ReadUser | QFileDevice::WriteUser
                        | QFileDevice::ReadGroup | QFileDevice::WriteGroup
                        | QFileDevice::ReadOther | QFileDevice::WriteOther);

    QString message = userNow + "," + QString::number(pidNow);
    qDebug() << "doWrite: message= " << message;

    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();
}

static void doRead()
{
    QString line;

    QFile file(EXIST_USERNAME_AND_PID_PATH);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "file read error!";
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        line = in.readLine();
    }
    qDebug() << "line: " << line;

    QStringList list = line.split(',', QString::SkipEmptyParts);
    uuap.usedUsername = list.at(0);
    uuap.usedPid = list.at(1);
    qDebug() << "uuap.usedUsername = " << uuap.usedUsername
             << "uuap.usedPid = " << uuap.usedPid;

    file.close();
}

void writeCurrentAppUsernameAndPid(QString userNow, int pidNow)
{
    doWrite(userNow, pidNow);
}

/**
 * @brief doKill
 * @return
 * Cancel: exitCode():127	exitStatus():QProcess::NormalExit	error():QProcess::UnknownError
 * Timeout: exitCode():0 	exitStatus():QProcess::NormalExit	error():QProcess::Timedout => so -1, do not timeout
 * Normal Password Input Correctly: exitCode():1 exitStatus():QProcess::NormalExit
 */
static int doKill()
{
    QString cmd(BASH_TYPE);
    QStringList arglists;

    QString args = QString("pkexec kill -9 ") + uuap.usedPid;

    arglists << "-c";
    arglists << args;

    QProcess *process = new QProcess();
    process->start(cmd, arglists);
    process->waitForFinished(-1);

    qDebug() << "exitCode: " << process->exitCode()
             << ", exitStatus: " << process->exitStatus()
             << ", error: " << process->error()
             << ", result err: " << process->readAllStandardError()
             << ", result out: " << process->readAllStandardOutput();

    if (process->exitCode() == 1 || process->exitCode() == 0)
        // Password Input is correctly
        return SUCCESS;

    return FAIL;
}

static bool pidIsNotExist()
{
    QString cmd(BASH_TYPE);
    QStringList arglists;

    arglists << "-c";
    QString args = QString("ps -ef | grep ") + uuap.usedPid + QString(" |grep -v log |grep -v pts |grep -v grep");
    arglists<< args;

    QProcess *process = new QProcess();
    process->start(cmd, arglists);
    process->waitForFinished();
    QString strOutput = QString::fromLocal8Bit(process->readAllStandardOutput().trimmed());
    qDebug() << "strOutput = " << strOutput;

    return strOutput.isEmpty();
}

/**
 * @brief singleWarnMsg
 * 当前xx用户正在使用该软件，打开将会关闭xx用户正在进行的操作
 * @return
 */
int singleWarnMsg()
{
    // normal user: cannot read by `ps -ef`
    //userUsed = getUsedUserName();

    doRead();
    QString userUsed = uuap.usedUsername;
    QString userNow = getCurrentUserName();

#if 1
    // current user straight return
    if (userUsed.compare(userNow) == 0) {
        qDebug() << "user: " << userNow << " " << userUsed;
        return 0;
    }
#endif

    // Q, Because vnc login, so we should message it again ?
    QMessageBox::StandardButton msgBox;
    QString msg = QObject::tr("User ") + userUsed + QObject::tr(" has already opened kylin-scanner, continue open will close ")
            + userUsed + QObject::tr("'s operations.");
    msgBox = QMessageBox::question(0, QObject::tr("Question"), msg);

    if (msgBox == QMessageBox::Yes)
        return 1;
    else
        return 0;
}

int getScreenWidth()
{
    Display *disp = XOpenDisplay(NULL);
    Screen *scrn = DefaultScreenOfDisplay(disp);
    if (NULL == scrn)
        return 0;

    int width = scrn->width;

    if (NULL != disp)
        XCloseDisplay(disp);

    return width;
}

static bool checkStarted()
{
    QString comm;
    QDir dir("/proc/");
    QFileInfo file;
    ifstream in;
    std::string s;

    foreach (QFileInfo f, dir.entryInfoList()) {
        //qDebug() << QString("%1").arg(getpid()) << f.baseName();
        if (f.baseName() == QString("%1").arg(getpid())
                || f.baseName() == "self"
                || f.baseName() == "thread-self")
            continue;

        comm = QString("/proc/%1/comm").arg(f.baseName());
        file.setFile(comm);

        if (file.exists()) {
            in.open(file.absoluteFilePath().toStdString());
            if (in.is_open()) {
                getline(in, s);
                comm = QString::fromStdString(s);
                if (comm == "kylin-scanner") {
                    qDebug() << "exist pid = " << f.baseName();
                    return true;
                }
            }
            in.close();
        }
    }

    return false;
}

static QString getAppVersion()
{
    QProcess process;
    process.start(QString("dpkg-parsechangelog -l %1 --show-field Version").arg(CHANGELOG_PATH));
    process.waitForFinished();
    QByteArray result = process.readAllStandardOutput();
    result = result.left(result.length()-1);
    return result;
}

int main(int argc, char *argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
        QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    verifyScannerDir();
    qInstallMessageHandler(KYCMessageOutput);
    //qInstallMessageHandler(customOutputMessage);
    qDebug() << "Init";

    /** SingleApplication: This class make current user cannot open other `kylin-scanner` again,
     * but switch to other user could open `kylin-scanner` again. Thus, we don't use this class for `kylin-scanner`.
     *
     * SingleApplication app(argc, argv);
     *
     */
    QApplication app(argc, argv);

    app.setApplicationName(APP_NAME);
    //app.setApplicationVersion(QString(APP_VERSION));
    app.setApplicationVersion(getAppVersion());

    app.setWindowIcon (QIcon::fromTheme("kylin-scanner"));

#if 1
    QCommandLineParser parser;
    parser.setApplicationDescription("An interface-friendly scanner with OCR, Smart rectify and OneClickBeauty.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);
#endif

#if 1
    // For qt: QFileDialog
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    // For translations with different language environments
    QTranslator translator;
    QString localePath(CN_TRANSLATION_FILE_PATH);
    QString locale = localePath + QLocale::system().name();

    translator.load(locale);
    app.installTranslator(&translator);
#endif

    bool isRunning = false;
    if (checkStarted()) {
        qDebug() << "App is running, What's your choice ?";
        //QMessageBox::information(NULL, "Cannot start KylinScanner", "KylinScanner has started in current system now.");
        doRead();

        QString userUsed = uuap.usedUsername;
        QMessageBox::StandardButton msgBox;
        QString msg = QObject::tr("Current ") + userUsed + QObject::tr(" User")
                + QObject::tr(" has already opened kylin-scanner, open will close ")
                + userUsed + QObject::tr("'s operations. Are you continue?");
        msgBox = QMessageBox::question(0, QObject::tr("Question"), msg);

        if (msgBox == QMessageBox::Yes) {
            int retKill = doKill();
            if (FAIL == retKill) {
                qDebug() << "Maybe you close this messagebox or choose cancel button!";
                return 0;
            } else {
                // retKill value (0,1) are not fixed, it's reasonable for us to judge whether exist's pid has been killed.
                if (pidIsNotExist()) {
                    qDebug() << "kill " << uuap.usedPid << " success, congratulations!";
                    isRunning = false;
                } else {
                    qDebug() << "kill " << uuap.usedPid << " error!";
                    isRunning = true;
                }
            }
        } else {
            qDebug() << "You choose not open again, so directly close!";
            isRunning = true;
            return 0;
        }
    } else {
         qDebug() << "App is not running, let's start it!";
         isRunning = false;
    }

    if (isRunning) {
#if 0
        qDebug() << "Kylin-scanner has already exist!";
        int ret = singleWarnMsg();
        if (1 == ret) {
            int retKill = doKill();
            if (FAIL == retKill)
                return 0;

            QString userNow = getCurrentUserName();
            int pidNow = app.applicationPid();
            qDebug() << "Exist user= " << userNow << ", pid= " << pidNow;

            writeCurrentAppUsernameAndPid(userNow, pidNow);

            Widget w;
            //app.w = &w; // Need by SingleApplication class

            MotifWmHints hints;
            hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
            hints.functions = MWM_FUNC_ALL;
            hints.decorations = MWM_DECOR_BORDER;
            XAtomHelper::getInstance()->setWindowMotifHint(w.winId(), hints);

            w.show();
            w.move ((QApplication::desktop()->availableGeometry().width() - w.width()) / 2,
                    (QApplication::desktop()->availableGeometry().height() - w.height()) / 2);
            return app.exec();
        }
#endif
    } else {
        QString userNow = getCurrentUserName();
        int pidNow = app.applicationPid();
        qDebug() << "Exist user= " << userNow << ", pid= " << pidNow;

        doWrite(userNow, pidNow);

        KYCWidget w;
        //app.w = &w; // Need by SingleApplication class

        MotifWmHints hints;
        hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
        hints.functions = MWM_FUNC_ALL;
        hints.decorations = MWM_DECOR_BORDER;
        KYCXAtomHelperObject::getInstance()->setWindowMotifHint(w.winId(), hints);

        w.show();
        w.move ((QApplication::desktop()->availableGeometry().width() - w.width()) / 2,
                (QApplication::desktop()->availableGeometry().height() - w.height()) / 2);

        return app.exec();
    }
    return 0;
}
