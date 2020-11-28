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


const int mounth_days[12]= {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
int mounth_days_add[12];

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
        text = QString("None:");
    }

    QString context_info = QString("(%1:%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("[%1]").arg(current_date_time);
    QString message = QString("%1 %2 %3 %4").arg(current_date).arg(text).arg(context_info).arg(msg);

    QFile file(qgetenv("HOME") +"/.cache/kylin-scanner/kylin-scanner.log");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}

void deleteOutputMessage()
{
    QDir dir(qgetenv("HOME") +"/.cache/kylin-scanner/");
    QStringList filters;//设置过滤器
    filters<<"*.log";//过滤留下txt文件
    dir.setNameFilters(filters);
    qDebug() << "filter: " << filters;

    QList<QString> dataList; //用来存放从log文件名获取到的日期

    QDateTime data = QDateTime::currentDateTime();//获取当前时间
    QString dataStr = data.toString("yyyy-MM-dd");//时间格式转换

    /*初始化月份累加天数*/
    for (int i=0 ; i<12; i++) {
        if (i == 0) {
            mounth_days_add[i] = 0;
        }
        else {
            mounth_days_add[i] = mounth_days[i-1] + mounth_days_add[i-1];
        }
    }

    //循环遍历，在Qlist中存取文件名
    foreach (QFileInfo mItem, dir.entryInfoList()) {
        dataList << mItem.fileName();
    }
    //将当天日期换算为天数
    int currentDate_value =
        mounth_days_add[dataStr.mid(5,2).toInt()-1]+dataStr.mid(8,2).toInt();

    for (int i=0; i<dataList.count(); i++) {
        //从获取到文件名中，读取日期，将日期换算成天数，大于7天的删除
        if ((currentDate_value - (mounth_days_add[dataList.at(i).mid(5,2).toInt()-1] + dataList.at(i).mid(8,2).toInt())) > 7) {
            qDebug()<<"overdue debug file :"<<dataList.at(i);
            if (QFile::exists(dataList.at(i))) {
                qDebug()<<dataList.at(i)<<"deubg file txt exist , removing...";
                QFile::remove(dataList.at(i));
                qDebug()<<dataList.at(i)<<"file txt removed";
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (getScreenWidth() > 2560) {
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
                QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
                QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
        #endif
    }
    qInstallMessageHandler(customOutputMessage);
    deleteOutputMessage ();

    SingleApplication app(argc, argv);
    //QApplication::setWindowIcon(QIcon::fromTheme("kylin-scanner", QIcon(":/icon/icon/scanner.png")));
    //QApplication::setWindowIcon(QIcon::fromTheme("scanner"));

    //注册MessageHandler
    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    QString logPath = homePath.at(0) + "/.cache/kylin-scanner";
    QDir configPath;
    if (! configPath.exists (logPath))
    {
        qDebug() << "Create " << logPath;
        configPath.mkdir (logPath);
    }


    app.setApplicationVersion("1.0.0");
    app.setApplicationName("kylin-scanner");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    //QCommandLineOption l(QStringList() << "l" << "language", "set code language c++ or lua", "c++");
    //parser.addOption(l);

    parser.process(app);

    //通过parser来解析一系列命令行参数
    //QString strValue = parser.value("l");


    // For qt: QFileDialog
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    // For translations with different language environments
    QTranslator translator;
    QString locale = "/usr/share/kylin-scanner/translations/kylin-scanner." + QLocale::system().name();
    qDebug() << locale ;

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
