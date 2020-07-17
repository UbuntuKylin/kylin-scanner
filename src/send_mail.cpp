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
#include "send_mail.h"
#include <QPainter>
#include <QDebug>
#include <QBitmap>
#include <QFileInfo>
#include <QFileIconProvider>

no_mail::no_mail(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setFixedSize(320,250);
    labTitle = new QLabel();
    textEdit = new QTextEdit();
    line = new QFrame();
    btnOk = new QPushButton();
    btnCancel = new QPushButton();
    hBoxLayout = new QHBoxLayout();
    vBoxLayout = new QVBoxLayout(this);

    labTitle->setText(tr("No email client"));
    labTitle->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    labTitle->setStyleSheet("color:rgb(232,232,232)");
    labTitle->setFixedSize(260,32);
    QFont ft;
    ft.setPointSize(16);
    labTitle->setFont(ft);

    QFont ft1;
    ft1.setPointSize(10);
    textEdit->setFont(ft1);
    textEdit->setText(tr("Not find email client in the system, please download and install email client firstly."));
    textEdit->setStyleSheet("QTextEdit{background-color:rgb(47,44,43);color:rgb(232,232,232);border:0px}");
    textEdit->setFixedSize(260,50);
    textEdit->setReadOnly(true);

    line = new QFrame();
    line->setObjectName(QString::fromUtf8("line"));
    line->setMaximumHeight(1);
    line->setMaximumWidth(260);
    line->setMinimumWidth(260);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("QLabel{color:rgb(32,30,29)}");

    btnOk->setText(tr("Go to install"));
    btnOk->setFixedSize(120,36);
    btnCancel->setText(tr("Cancel"));
    btnCancel->setFixedSize(100,36);

    btnOk->setStyleSheet("QPushButton{background-color:rgb(32,30,29);border:1px solid #939393;color:rgb(232,232,232);border-radius:18px;}"
                              "QPushButton:hover{border:none;background-color:rgb(39,208,127);border:rgb(147,147,147);color:rgb(232,232,232);border-radius:18px;}"
                                "QPushButton:checked{border:none;background-color:rgb(39,208,127);border:rgb(147,147,147);color:rgb(232,232,232);border-radius:18px;}");
    btnCancel->setStyleSheet("QPushButton{background-color:rgb(32,30,29);border:1px solid #939393;color:rgb(232,232,232);border-radius:18px;}"
                              "QPushButton:hover{border:none;background-color:rgb(39,208,127);color:rgb(232,232,232);border-radius:18px;}"
                                "QPushButton:checked{border:none;background-color:rgb(39,208,127);color:rgb(232,232,232)border-radius:18px;}");

    hBoxLayout->setSpacing(0);
    hBoxLayout->addWidget(btnOk);
    hBoxLayout->addSpacing(16);
    hBoxLayout->addWidget(btnCancel);
    hBoxLayout->setContentsMargins(16,0,16,0);

    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(47, 44, 43));
    setAutoFillBackground(true);
    setPalette(pal);

    vBoxLayout->setSpacing(0);
    vBoxLayout->addWidget(labTitle);
    vBoxLayout->addSpacing(22);
    vBoxLayout->addWidget(textEdit);
    vBoxLayout->addSpacing(22);
    vBoxLayout->addLayout(hBoxLayout);
    vBoxLayout->setContentsMargins(30,30,30,30);

    QBitmap bitMap(width(),height()); // A bit map has the same size with current widget

    bitMap.fill();

    QPainter painter(&bitMap);

    painter.setBrush(Qt::black);

    painter.setPen(Qt::NoPen); // Any color that is not QRgb(0,0,0) is right

    painter.setRenderHint(QPainter::Antialiasing);

    painter.drawRoundedRect(bitMap.rect(),6,6); //设置圆角弧度

    setMask(bitMap);

    setLayout(vBoxLayout);
    connect(btnOk,SIGNAL(clicked()),this,SLOT(accept()));
    connect(btnCancel,SIGNAL(clicked()),this,SLOT(reject()));
}

no_mail::~no_mail()
{

}


send_mail::send_mail(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setFixedSize(320,350);
    labTitle = new QLabel();
    btnCancel = new QPushButton();
    hBoxLayout = new QHBoxLayout();
    hBoxLayout1 = new QHBoxLayout();
    vBoxLayout = new QVBoxLayout();
    vBoxLayout1 = new QVBoxLayout();
    scrollArea = new QScrollArea();
    widget = new QWidget();

    widget->setFixedSize(300,270);
    widget->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint); // 去掉标题栏,去掉任务栏显示，窗口置顶

    labTitle->setText(tr("Select email client"));
    labTitle->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    labTitle->setStyleSheet("color:rgb(232,232,232)");
    labTitle->setFixedSize(190,32);
    QFont ft;
    ft.setPointSize(16);
    labTitle->setFont(ft);

    btnCancel->setFixedSize(12,12);
    btnCancel->setStyleSheet("QPushButton{border-image: url(:/icon/icon/close.png);border:none;background-color:rgb(47,44,43);border-radius:0px;}"
                              "QPushButton:hover{border-image: url(:/icon/icon/close-click.png);border:none;background-color:rgb(47,44,43);border-radius:0px;}"
                                "QPushButton:checked{border-image: url(:/icon/icon/close-click.png);border:none;background-color:rgb(47,44,43);border-radius:0px;}");
    hBoxLayout->addStretch();
    hBoxLayout->addWidget(btnCancel);
    hBoxLayout->setContentsMargins(0,0,5,0);
    hBoxLayout1->addStretch();
    hBoxLayout1->addWidget(labTitle);
    hBoxLayout1->addStretch();
    hBoxLayout1->setContentsMargins(0,0,0,0);
    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(47, 44, 43));
    setAutoFillBackground(true);
    setPalette(pal);

    vBoxLayout->setSpacing(0);
    vBoxLayout->addLayout(hBoxLayout);
    vBoxLayout->addSpacing(6);
    vBoxLayout->addLayout(hBoxLayout1);
    vBoxLayout->setContentsMargins(0,5,0,0);
    setLayout(vBoxLayout);

    QBitmap bitMap(width(),height()); // A bit map has the same size with current widget

    bitMap.fill();

    QPainter painter(&bitMap);

    painter.setBrush(Qt::black);

    painter.setPen(Qt::NoPen); // Any color that is not QRgb(0,0,0) is right

    painter.setRenderHint(QPainter::Antialiasing);

    painter.drawRoundedRect(bitMap.rect(),6,6); //设置圆角弧度

    setMask(bitMap);
    connect(btnCancel,SIGNAL(clicked()),this,SLOT(reject()));
}
Appinfo * _getAppList(const char *contentType){
    GList *applist;
    applist = g_app_info_get_all_for_type(contentType);
    GAppInfo * item;

    if(applist != NULL){
        int len = g_list_length(applist);
        Appinfo * appinfo=(Appinfo *)malloc(sizeof(Appinfo)*(len+1));

        //获取应用列表
        for (int index=0; index < len; index++){
            item = (GAppInfo*) g_list_nth_data(applist, index);
            appinfo[index].item=item;
        }
        appinfo[len].item=NULL;
        return appinfo;

    } else {
        return NULL;
    }
}

AppList * getAppIdList(const char *contentType){
    Appinfo *appinfo = _getAppList(contentType);
    if(appinfo != NULL){
        int i = 0;
        while(appinfo[i].item != NULL)
            i++;
        AppList *list = (AppList *)malloc(sizeof(AppList)*(i+1));
        int count = i;
        int index = 0;
        for(gint j = 0;appinfo[j].item != NULL;j++){
            const char *id = g_app_info_get_id(appinfo[j].item);
            if(id != NULL){
                int len = strlen(id);
                list[index].appid = (char *)malloc(sizeof(char)*(len+1));
                strcpy(list[index].appid,id);
                index++;
            } else {
                free(list+count);
                count--;
            }
        }
        list[count].appid=NULL;
        free(appinfo);
        return list;
    } else {
        return NULL;
    }
}
void send_mail::set_btnList()
{
    AppList * maillist = getAppIdList(MAILTYPE);
    if (maillist){
        vBoxLayout1->setSpacing(0);
        vBoxLayout1->setContentsMargins(0,0,0,0);
        for (int i = 0; maillist[i].appid != NULL; i++)
        {
            QString single(maillist[i].appid);
            desktopName.append(single);
            QByteArray ba = QString(DESKTOPPATH + single).toUtf8();
            GDesktopAppInfo * mailinfo = g_desktop_app_info_new_from_filename(ba.constData());
            QString appname = g_app_info_get_name(G_APP_INFO(mailinfo));
            const char * iconname = g_icon_to_string(g_app_info_get_icon(G_APP_INFO(mailinfo)));
            QIcon appicon;
            if (QIcon::hasThemeIcon(QString(iconname)))
                appicon = QIcon::fromTheme(QString(iconname));
            QPushButton *btn = new QPushButton();
            QFont ft;
            ft.setPointSize(11);
            btn->setFont(ft);
            btn->setText(appname);
            btn->setFixedSize(190,40);
            btn->setStyleSheet("QPushButton{background-color:rgb(68,66,72);background-position:left;text-align:left;border:none;color:rgb(232,232,232);border-radius:6px;}"
                                "QPushButton:hover{border:none;background-color:rgb(39,208,127);color:rgb(232,232,232);border-radius:6px;}"
                                 "QPushButton:checked{border:none;background-color:rgb(39,208,127);color:rgb(232,232,232)border-radius:6px;}");
            btn->setIcon(appicon);
            btn->setCheckable(true);
            btnList.append(btn);
            QHBoxLayout *hBoxLayout1 = new QHBoxLayout();
            hBoxLayout1->addStretch();
            hBoxLayout1->addWidget(btnList[i]);
            hBoxLayout1->addStretch();
            hBoxLayout1->setContentsMargins(0,0,0,0);
            vBoxLayout1->addSpacing(10);
            vBoxLayout1->addLayout(hBoxLayout1);
            connect(btnList[i],SIGNAL(toggled(bool)),this,SLOT(on_btn_clicked()));

            free(maillist[i].appid);
        }
        vBoxLayout1->addStretch();
        widget->setLayout(vBoxLayout1);
        scrollArea->setWidget(widget);
        scrollArea->setFrameShape(QFrame::NoFrame);
        free(maillist);
    }
    vBoxLayout->addWidget(scrollArea);
    setLayout(vBoxLayout);
}

void send_mail::open_email(QString name)
{
    QFile aFile(DESKTOPPATH+name);
        if(!aFile.exists())
            qDebug() << DESKTOPPATH << name << " no exists!";
        if(!aFile.open(QIODevice::ReadOnly | QIODevice::Text))
            qDebug() << DESKTOPPATH << name << " open false!";
        QTextStream aStream(&aFile);
        QString str;
        aStream.setAutoDetectUnicode(true);
        while(!aStream.atEnd())
        {
            str = aStream.readLine();
            if(str.startsWith("Exec=",Qt::CaseSensitive))
                break;
        }
        str = str.section("Exec=",1,1);
        str = str.section(" ",0,0);
        QProcess *process = new QProcess();
        process->start(str);

}

void send_mail::on_btn_clicked()
{
    for (int i = 0;i < btnList.size();i++)
    {
        if(btnList[i]->isChecked())
        {
            open_email(desktopName[i]);
            reject();
            break;
        }
    }
}
