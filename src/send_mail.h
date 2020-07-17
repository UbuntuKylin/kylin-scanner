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
#ifndef SEND_MAIL_H
#define SEND_MAIL_H
#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QFrame>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QProcess>

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
typedef struct _Applist
{
    char *appid;
}AppList;//用于存放应用列表

typedef struct _AppInfo
{
    GAppInfo *item;
}Appinfo;//用于存放应用列表信息
class no_mail : public QDialog
{
    Q_OBJECT
public:
    explicit no_mail(QWidget *parent = nullptr);
    ~no_mail();
private:
    QLabel *labTitle;
    QTextEdit *textEdit;
    QFrame *line;
    QPushButton *btnOk;
    QPushButton *btnCancel;
    QHBoxLayout *hBoxLayout;
    QVBoxLayout *vBoxLayout;


};

class send_mail : public QDialog
{
    Q_OBJECT
public:
    explicit send_mail(QWidget *parent = nullptr);
    void set_btnList();
    void open_email(QString name);

private:
    QLabel *labTitle;
    QPushButton *btnCancel;
    QHBoxLayout *hBoxLayout;
    QHBoxLayout *hBoxLayout1;
    QVBoxLayout *vBoxLayout;
    QVBoxLayout *vBoxLayout1;
    QScrollArea *scrollArea;
    QWidget *widget;
    QList<QPushButton *> btnList;
    QList<QString> desktopName;

private slots:
    void on_btn_clicked(void);

};
Appinfo * _getAppList(const char *contentType);
AppList * getAppIdList(const char *contentType);
#endif // MARK_DIALOG_H
