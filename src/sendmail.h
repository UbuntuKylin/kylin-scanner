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

#ifndef SENDMAIL_H
#define SENDMAIL_H

#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QFrame>
#include <QDialog>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QProcess>
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
#include <QBitmap>
#include <QFileInfo>
#include <QFileIconProvider>
#include <qmath.h>
#include "theme.h"

#ifdef signals
#undef signals
#endif
extern "C" {
#include <glib.h>
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>
}

#define MAILTYPE "x-scheme-handler/mailto"
#define DESKTOPPATH "/usr/share/applications/"
#define MAIL_WINDOW_WIDTH 320
#define MAIL_WINDOW_HEIGHT 260

typedef struct _Applist {
    char *appid;
} AppList; //用于存放应用列表
AppList *getAppIdList(const char *contentType);

typedef struct _AppInfo {
    GAppInfo *item;
} Appinfo; //用于存放应用列表信息
Appinfo *_getAppList(const char *contentType);

class KYCNoMailDialog : public QDialog
{
    Q_OBJECT
public:
    explicit KYCNoMailDialog(QWidget *parent = nullptr);
    ~KYCNoMailDialog();

    void initWindow();
    void initLayout();
    void initStyle();
    void initConnect();

private:
    QStringList stylelist;
    QStringList iconthemelist;
    QGSettings *style_settings;
    QGSettings *icon_theme_settings;
    QPushButton *btnClose;
    QLabel *labTitle;
    QTextEdit *textEdit;
    QFrame *line;
    QPushButton *btnOk;
    QPushButton *btnCancel;
    QHBoxLayout *hBoxLayout;
    QHBoxLayout *hBoxLayoutClose; // 关闭按钮水平布局
    QVBoxLayout *vBoxLayout;

Q_SIGNALS:
   void noMailWindowClose();

private slots:
    void nomail_style_changed(QString); // 系统黑白主题样式变换
    void closeNoMailWindow();
};

class KYCSendMailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KYCSendMailDialog(QWidget *parent = nullptr);
    ~KYCSendMailDialog();

    void initWindow();
    void initLayout();
    void initStyle();
    void initConnect();

    void setBtnList();
    void openMail(QString name);
    //void paintEvent(QPaintEvent *event); // 窗口阴影

private:
    QStringList stylelist;
    QStringList iconthemelist;
    QGSettings *style_settings;
    QGSettings *icon_theme_settings;
    QLabel *labTitle;
    QPushButton *btnClose;
    QHBoxLayout *hBoxLayout;
    QHBoxLayout *hBoxLayout1;
    QHBoxLayout *hBoxLayoutClose; // 关闭按钮水平布局
    QVBoxLayout *vBoxLayout;
    QVBoxLayout *vBoxLayout1;
    QScrollArea *scrollArea;
    QWidget *widget;
    QList<QPushButton *> btnList;
    QList<QString> desktopName;

Q_SIGNALS:
    void sendMailWindowClose();

public slots:
    void onBtnClicked();
    void closeSendMailWindow();
    void sendmail_style_changed(QString); // 系统黑白主题样式变换
};


#endif // SENDMAIL_H
