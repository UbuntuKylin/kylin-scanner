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

#include "common.h"
#include "sendmail.h"
#include "xatomhelper.h"

KYCNoMailDialog::KYCNoMailDialog(QWidget *parent) :
    QDialog(parent)
    , style_settings (new QGSettings(ORG_UKUI_STYLE))
    , icon_theme_settings (new QGSettings(ORG_UKUI_STYLE))
    , btnClose (new QPushButton())
    , labTitle (new QLabel())
    , textEdit (new QTextEdit())
    , line (new QFrame())
    , btnOk (new QPushButton())
    , btnCancel (new QPushButton())
    , hBoxLayout (new QHBoxLayout())
    , hBoxLayoutClose (new QHBoxLayout())
    , vBoxLayout (new QVBoxLayout())
{
    initWindow();

    initLayout();

    initStyle();

    initConnect();
}

KYCNoMailDialog::~KYCNoMailDialog()
{

}

void KYCNoMailDialog::initWindow()
{
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    KYCXAtomHelperObject::getInstance()->setWindowMotifHint(winId(), hints);

    setWindowTitle (tr("No email client")); // For system tray text
    setFixedSize(MAIL_WINDOW_WIDTH, MAIL_WINDOW_HEIGHT); // 窗口固定大小
}

void KYCNoMailDialog::initLayout()
{
    btnClose->setIcon (QIcon::fromTheme (ICON_THEME_CLOSE));
    btnClose->setToolTip(tr("Close"));
    btnClose->setFixedSize(30, 30);
    btnClose->setIconSize (QSize(16, 16));
    btnClose->setProperty("isWindowButton", 0x2);
    btnClose->setProperty("useIconHighlightEffect", 0x8);
    btnClose->setFlat(true);

    hBoxLayoutClose->setSpacing(0);
    hBoxLayoutClose->addStretch();
    hBoxLayoutClose->addWidget(btnClose);
    //hBoxLayoutClose->setSpacing(2);
    hBoxLayoutClose->setAlignment(Qt::AlignCenter);
    hBoxLayoutClose->setContentsMargins(0, 4, 4, 4);

    labTitle->setText(tr("No email client"));
    labTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    labTitle->setFixedSize(260, 32);
    QFont ft;
    ft.setPixelSize(24);
    labTitle->setFont(ft);

    QFont ft1;
    ft1.setPixelSize(14);
    //ft1.setPointSize(14);
    textEdit->setFont(ft1);
    textEdit->setText(
        tr("Not find email client in the system, please download and install email client firstly."));

    int textEditSizeX = MAIL_WINDOW_WIDTH - 32 - 32;
    textEdit->setFixedSize(textEditSizeX, 50);
    textEdit->setReadOnly(true);

    line->setObjectName(QString::fromUtf8("line"));
    line->setMaximumHeight(1);
    line->setMaximumWidth(260);
    line->setMinimumWidth(260);
    line->setFrameShape(QFrame::HLine);

    btnOk->setText(tr("Go to install"));
    btnOk->setFixedSize(120, 36);
    btnCancel->setText(tr("Cancel"));
    btnCancel->setFixedSize(120, 36);

    hBoxLayout->setSpacing(0);
    hBoxLayout->addWidget(btnOk);
    hBoxLayout->addSpacing(16);
    hBoxLayout->addWidget(btnCancel);
    hBoxLayout->setContentsMargins(0, 0, 32, 0);

    vBoxLayout->setSpacing(0);
    vBoxLayout->addLayout(hBoxLayoutClose);
    vBoxLayout->addSpacing(18);
    vBoxLayout->addWidget(labTitle);
    vBoxLayout->addSpacing(24);
    vBoxLayout->addWidget(textEdit);
    vBoxLayout->addSpacing(40);
    vBoxLayout->addLayout(hBoxLayout);
    vBoxLayout->setContentsMargins(32, 0, 0, 48);
    setLayout(vBoxLayout);
}

void KYCNoMailDialog::initStyle()
{
    stylelist << STYLE_NAME_KEY_DARK << STYLE_NAME_KEY_BLACK;
    iconthemelist << ICON_THEME_KEY_BASIC << ICON_THEME_KEY_CLASSICAL << ICON_THEME_KEY_DEFAULT;

    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(47, 44, 43));
        setAutoFillBackground(true);
        setPalette(pal);

        labTitle->setStyleSheet("color:#D9FFFFFF"); // 85% => D9, 255,255,255 => FFFFFF
        textEdit->setStyleSheet("QTextEdit{background-color:rgb(47,44,43);color:#D9FFFFFF;border:0px}");
        line->setStyleSheet("QFrame{color:rgb(32,30,29)}");
    } else {
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(255, 255, 255));
        setAutoFillBackground(true);
        setPalette(pal);

        labTitle->setStyleSheet("color:#D9000000"); // 85% => D9, 255,255,255 => FFFFFF
        textEdit->setStyleSheet("QTextEdit{background-color:rgb(255,255,255);color:#D9000000;border:0px}");
        line->setStyleSheet("QFrame{color:rgb(32,30,29}");
    }
}

void KYCNoMailDialog::initConnect()
{
    connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(closeNoMailWindow()));
    connect(btnClose, SIGNAL(clicked()), this, SLOT(closeNoMailWindow()));
    connect(style_settings, SIGNAL(changed(QString)), this, SLOT(nomail_style_changed(QString)));
}

void KYCNoMailDialog::nomail_style_changed(QString)
{
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(47, 44, 43));
        setAutoFillBackground(true);
        setPalette(pal);

        labTitle->setStyleSheet("color:#D9FFFFFF"); // 85% => D9, 255,255,255 => FFFFFF
        textEdit->setStyleSheet("QTextEdit{background-color:rgb(47,44,43);color:#D9FFFFFF;border:0px}");
        line->setStyleSheet("QFrame{color:rgb(32,30,29)}");
    } else {
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(255, 255, 255));
        setAutoFillBackground(true);
        setPalette(pal);

        labTitle->setStyleSheet("color:#D9000000"); // 85% => D9, 255,255,255 => FFFFFF
        textEdit->setStyleSheet("QTextEdit{background-color:rgb(255,255,255);color:#D9000000;border:0px}");
        line->setStyleSheet("QFrame{color:rgb(32,30,29}");
    }
}

void KYCNoMailDialog::closeNoMailWindow()
{
    reject();
    emit noMailWindowClose();
}

KYCSendMailDialog::KYCSendMailDialog(QWidget *parent) :
    QDialog(parent)
    , style_settings (new QGSettings(ORG_UKUI_STYLE))
    , icon_theme_settings (new QGSettings(ORG_UKUI_STYLE))
    , labTitle (new QLabel())
    , btnClose (new QPushButton())
    , hBoxLayout (new QHBoxLayout())
    , hBoxLayout1 (new QHBoxLayout())
    , vBoxLayout (new QVBoxLayout())
    , vBoxLayout1 (new QVBoxLayout())
    , scrollArea (new QScrollArea())
    , widget (new QWidget())
{
    initWindow();

    initLayout();

    initStyle();

    initConnect();
}

KYCSendMailDialog::~KYCSendMailDialog()
{

}

void KYCSendMailDialog::initWindow()
{
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    KYCXAtomHelperObject::getInstance()->setWindowMotifHint(winId(), hints);

    setWindowTitle (tr("Select email client"));
    setFixedSize(MAIL_WINDOW_WIDTH, MAIL_WINDOW_HEIGHT);

    stylelist << STYLE_NAME_KEY_DARK << STYLE_NAME_KEY_BLACK;
    iconthemelist << ICON_THEME_KEY_BASIC << ICON_THEME_KEY_CLASSICAL << ICON_THEME_KEY_DEFAULT;

    widget->setFixedSize(MAIL_WINDOW_WIDTH, MAIL_WINDOW_HEIGHT);
    widget->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
}

void KYCSendMailDialog::initLayout()
{
    btnClose->setIcon (QIcon::fromTheme (ICON_THEME_CLOSE));
    btnClose->setToolTip(tr("Close"));
    btnClose->setFixedSize(30, 30);
    btnClose->setIconSize (QSize(16, 16));
    btnClose->setProperty("isWindowButton", 0x2);
    btnClose->setProperty("useIconHighlightEffect", 0x8);
    btnClose->setFlat(true);

    hBoxLayout->setSpacing(0);
    hBoxLayout->addStretch();
    hBoxLayout->addWidget(btnClose); // 取消也是关闭按钮
    hBoxLayout->setAlignment(Qt::AlignCenter);
    hBoxLayout->setContentsMargins(0, 4, 4, 4);

    labTitle->setText(tr("Select email client"));
    labTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    labTitle->setFixedSize(260, 32);
    QFont ft;
    ft.setPixelSize(24);
    labTitle->setFont(ft);

    hBoxLayout1->addStretch();
    hBoxLayout1->addWidget(labTitle);
    hBoxLayout1->addStretch();
    hBoxLayout1->setContentsMargins(0, 0, 0, 0);

    vBoxLayout->setSpacing(0);
    vBoxLayout->addLayout(hBoxLayout);
    vBoxLayout->addLayout(hBoxLayout1);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(vBoxLayout);
}

void KYCSendMailDialog::initStyle()
{
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(47, 44, 43));
        setAutoFillBackground(true);
        setPalette(pal);

        labTitle->setStyleSheet("color:#D9FFFFFF"); // 85% => D9, 255,255,255 => FFFFFF
    } else {
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(255, 255, 255));
        setAutoFillBackground(true);
        setPalette(pal);

        labTitle->setStyleSheet("color:#D9000000"); // 85% => D9, 255,255,255 => FFFFFF
    }
}

void KYCSendMailDialog::initConnect()
{
    connect(btnClose, SIGNAL(clicked()), this, SLOT(closeSendMailWindow()));
    connect(style_settings, SIGNAL(changed(QString)), this, SLOT(sendmail_style_changed(QString)));
}

Appinfo *_getAppList(const char *contentType)
{
    GList *applist;
    applist = g_app_info_get_all_for_type(contentType);
    GAppInfo *item;

    if (applist != NULL) {
        int len = g_list_length(applist);
        Appinfo *appinfo = (Appinfo *)malloc(sizeof(Appinfo) * (len + 1));

        //获取应用列表
        for (int index = 0; index < len; ++index) {
            item = (GAppInfo *) g_list_nth_data(applist, index);
            appinfo[index].item = item;
        }
        appinfo[len].item = NULL;
        return appinfo;

    } else {
        return NULL;
    }
}

AppList *getAppIdList(const char *contentType)
{
    Appinfo *appinfo = _getAppList(contentType);
    if (appinfo != NULL) {
        int i = 0;

        while (appinfo[i].item != NULL)
            ++i;

        int count = i;
        int index = 0;
        AppList *list = (AppList *)malloc(sizeof(AppList) * (i + 1));

        int j = 0;
        for (j = 0; appinfo[j].item != NULL; ++j) {
            const char *id = g_app_info_get_id(appinfo[j].item);
            if (id != NULL) {
                int len = strlen(id);
                list[index].appid = (char *)malloc(sizeof(char) * (len + 1));
                strcpy(list[index].appid, id);
                ++index;
            } else {
                free(list + count);
                --count;
            }
        }
        list[count].appid = NULL;
        free(appinfo);
        return list;
    } else {
        return NULL;
    }
}
void KYCSendMailDialog::setBtnList()
{
    int i = 0;

    AppList *maillist = getAppIdList(MAILTYPE);
    if (maillist) {
        qDebug() << maillist;
        vBoxLayout1->setSpacing(0);
        vBoxLayout1->setContentsMargins(0, 0, 0, 0);

        for (i = 0; maillist[i].appid != NULL; ++i) {
            QString single(maillist[i].appid);
            desktopName.append(single);
            QByteArray ba = QString(DESKTOPPATH + single).toUtf8();
            GDesktopAppInfo *mailinfo = g_desktop_app_info_new_from_filename(ba.constData());
            QString appname = g_app_info_get_name(G_APP_INFO(mailinfo));
            const char *iconname = g_icon_to_string(g_app_info_get_icon(G_APP_INFO(mailinfo)));
            QIcon appicon;
            if (QIcon::hasThemeIcon(QString(iconname)))
                appicon = QIcon::fromTheme(QString(iconname));
            QPushButton *btn = new QPushButton();
            QFont ft;
            ft.setPixelSize(14);
            btn->setFont(ft);
            btn->setText(appname);
            btn->setFixedSize(256, 56);
            btn->setIcon(appicon);
            btn->setIconSize(QSize(40, 40)); // 应用图标尺寸
            btn->setCheckable(true);
            btn->setStyleSheet("text-align:left");

            btnList.append(btn);

            QHBoxLayout *hBoxLayout1 = new QHBoxLayout();
            hBoxLayout1->addStretch();
            hBoxLayout1->addWidget(btnList[i]);
            hBoxLayout1->addStretch();

            if (i == 0) {
                hBoxLayout1->setContentsMargins(0, 16, 0, 0);
            } else {
                hBoxLayout1->setContentsMargins(0, 0, 0, 0);
            }
            vBoxLayout1->addSpacing(4);
            vBoxLayout1->addLayout(hBoxLayout1);
            connect(btnList[i], SIGNAL(toggled(bool)), this, SLOT(onBtnClicked()));

            free(maillist[i].appid);
        }
        vBoxLayout1->addStretch();
        widget->setLayout(vBoxLayout1);

        scrollArea->setWidget(widget);
        scrollArea->setFrameShape(QFrame::NoFrame);

        if (i < 2) {
            // Only 2 btnMail, so hide scrollbar
            scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏横向滚动条
            scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏竖向滚动条
        }
        free(maillist);
    }
    vBoxLayout->addWidget(scrollArea);
    setLayout(vBoxLayout);
}

void KYCSendMailDialog::openMail(QString name)
{
    QFile aFile(DESKTOPPATH + name);
    QString cmd(BASH_TYPE);
    QString str;
    QStringList  arglists;
    QString mailPicture(MAIL_PICTURE_PATH);

    if (!aFile.exists())
        qDebug() << DESKTOPPATH << name << " no exists!";
    if (!aFile.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug() << DESKTOPPATH << name << " open false!";

    QTextStream aStream(&aFile);
    aStream.setAutoDetectUnicode(true);
    while (!aStream.atEnd()) {
        str = aStream.readLine();
        if (str.startsWith("Exec=", Qt::CaseSensitive))
            break;
    }
    str = str.section("Exec=", 1, 1);
    str = str.section(" ", 0, 0);
    qDebug() << "exec str = " << str;
    QProcess *process = new QProcess();

    arglists << "-c";
    arglists << str;
    qDebug() << "str = " << str;
    if (str == "thunderbird") {
        arglists << "-compose" << "attachment='/tmp/scanner/present_image.jpg'";
    } else if (str == "claws-mail") {
        arglists << "--attach" << mailPicture;
    } else if (str == "mutt") {
        arglists << "-a" << mailPicture;
    } else if (str == "evolution") {
        QString attach = QString("mailto:?attach=") + mailPicture;
        arglists << "--component=mail" << attach;
    } else {
        arglists << " ";
    }
    qDebug() << "cmd = " << cmd << arglists;

    if (str == "evolution") {
        process->start("evolution", arglists);
        return;
    }
    process->start(cmd, arglists);
}

/*
void send_mail::paintEvent(QPaintEvent *event)
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    QPainter painter(this);

    QColor color(0, 0, 0, 51);
    for(int i=0; i<10; ++i)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(10-i, 10-i, this->width()-(10-i)*2, this->height()-(10-i)*2);
        color.setAlpha(150 - qSqrt(i)*50);
        painter.setPen(color);
        painter.drawPath(path);
    }
}
*/

void KYCSendMailDialog::onBtnClicked()
{
    for (int i = 0; i < btnList.size(); ++i) {
        if (btnList[i]->isChecked()) {
            openMail(desktopName[i]);
            reject();
            break;
        }
    }
}

void KYCSendMailDialog::closeSendMailWindow()
{
    reject();
    emit sendMailWindowClose();
}

void KYCSendMailDialog::sendmail_style_changed(QString)
{
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(47, 44, 43));
        setAutoFillBackground(true);
        setPalette(pal);

        labTitle->setStyleSheet("color:#D9FFFFFF"); // 85% => D9, 255,255,255 => FFFFFF
    } else {
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(255, 255, 255));
        setAutoFillBackground(true);
        setPalette(pal);

        labTitle->setStyleSheet("color:#D9000000"); // 85% => D9, 255,255,255 => FFFFFF
    }
}
