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
#include "singleApplication.h"
#include "xatomhelper.h"
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

void verifyScannerDir()
{
    QString logPath = "/tmp/scanner/";
    QDir configPath;
    if (! configPath.exists (logPath))
    {
        qDebug() << "Create " << logPath;
        configPath.mkdir (logPath);
    }
}

int getScreenWidth() 
{
    Display *disp = XOpenDisplay(NULL);
    Screen *scrn = DefaultScreenOfDisplay(disp);
    if (NULL == scrn) {
        return 0;
    }
    int width = scrn->width;

    if (NULL != disp) {
        XCloseDisplay(disp);
    }
    return width;
}

void customOutputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QString text;
    switch(type)
    {
    case QtDebugMsg:
       text = QString("Debug:"); // 调试信息
       break;

    case QtWarningMsg:
       text = QString("Warning:"); // 警告信息
       break;

    case QtCriticalMsg:
       text = QString("Critical:"); // 严重错误
       break;

    case QtFatalMsg:
       text = QString("Fatal:"); // 致命错误
       break;

    case QtInfoMsg:
        text = QString("Info:");
       break;
    default:
        text = QString("None:");
    }

    QString context_info = QString("(%1:%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("[%1]").arg(current_date_time);
    QString message = QString("%1 %2 %3 %4").arg(current_date).arg(text).arg(context_info).arg(msg);

    verifyScannerDir();

    QFile file("/tmp/scanner/kylin-scanner.log");
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}

int main(int argc, char *argv[])
{
    if (getScreenWidth() > 2560) {
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
                QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
                QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
        #endif
    }
    //注册MessageHandler
    verifyScannerDir();
    qInstallMessageHandler(customOutputMessage);

    SingleApplication app(argc, argv);
    app.setApplicationName("kylin-scanner");
    app.setApplicationVersion("1.0.8");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

    // For qt: QFileDialog
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    // For translations with different language environments
    QTranslator translator;
    QString localePath = "/usr/share/kylin-scanner/translations/kylin-scanner.";
    QString locale = localePath + QLocale::system().name();

    translator.load(locale);
    app.installTranslator(&translator);

    if (!app.isRunning()){
        qDebug() << "isRunning = false.";
        Widget w;
        app.w = &w;

        // 添加窗管协议
        MotifWmHints hints;
        hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
        hints.functions = MWM_FUNC_ALL;
        hints.decorations = MWM_DECOR_BORDER;
        XAtomHelper::getInstance()->setWindowMotifHint(w.winId(), hints);

        w.show();
        return app.exec();
    }
    return 0;
}
