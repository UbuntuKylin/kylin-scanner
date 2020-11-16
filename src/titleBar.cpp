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

#include "titleBar.h"
#include <QApplication>

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
    , style_settings (new QGSettings(ORG_UKUI_STYLE))
    , icon_theme_settings (new QGSettings(ORG_UKUI_STYLE))
    , m_logo (new QLabel())
    , m_logoMsg (new QLabel())
    , m_pMinimizeButton (new QPushButton())
    , m_pMaximizeButton (new QPushButton())
    , m_pCloseButton (new QPushButton())
    , pLayout (new QHBoxLayout())
{
    setFixedHeight(32);
    setMinimumWidth(860);

    stylelist << STYLE_NAME_KEY_DARK << STYLE_NAME_KEY_BLACK << STYLE_NAME_KEY_DEFAULT;
    iconthemelist << ICON_THEME_KEY_BASIC << ICON_THEME_KEY_CLASSICAL << ICON_THEME_KEY_DEFAULT;

    m_logo->setFixedSize (24, 24);
    m_logoMsg->setText (tr("kylin-scanner"));
    m_logoMsg->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    //m_logoMsg->setStyleSheet("color:rgb(232,232,232)");
    m_logoMsg->setFixedSize(56, 14);

    if ("ukui-icon-theme-basic" == icon_theme_settings->get(ICON_THEME_NAME).toString()) {
        m_logo->setStyleSheet("QLabel{border-image:url(/usr/share/icons/ukui-icon-theme-basic/24x24/devices/scanner.png);border-radius:4px;}");
    } else if ("ukui-icon-theme-classical" == icon_theme_settings->get(ICON_THEME_NAME).toString()) {
        m_logo->setStyleSheet("QLabel{border-image:url(/usr/share/icons/ukui-icon-theme-classical/24x24/devices/scanner.png);border-radius:4px;}");
    } else if ("ukui-icon-theme-default" == icon_theme_settings->get(ICON_THEME_NAME).toString()) {
        m_logo->setStyleSheet("QLabel{border-image:url(/usr/share/icons/ukui-icon-theme-default/24x24/devices/scanner.png);border-radius:4px;}");
    }

    m_pMinimizeButton->setIcon (QIcon::fromTheme (ICON_THEME_MINIMIZE));
    m_pMinimizeButton->setToolTip(tr("Minimize"));
    m_pMinimizeButton->setFixedSize(30, 30);
    m_pMinimizeButton->setIconSize (QSize(16, 16));

    m_pMaximizeButton->setIcon (QIcon::fromTheme (ICON_THEME_MAXIMAZE));
    m_pMaximizeButton->setToolTip(tr("Maximize"));
    m_pMaximizeButton->setFixedSize(30, 30);
    m_pMaximizeButton->setIconSize (QSize(16, 16));

    m_pCloseButton->setIcon (QIcon::fromTheme (ICON_THEME_CLOSE));
    m_pCloseButton->setToolTip(tr("Close"));
    m_pCloseButton->setFixedSize(30, 30);
    m_pCloseButton->setIconSize (QSize(16, 16));
    //m_pCloseButton->setFlat (true);
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        m_pCloseButton->setStyleSheet("QPushButton{border:none;background-color:#3D3D41;border-radius:4px;}"
                                  "QPushButton:hover{border:none;background-color:#F86457;border-radius:4px;}"
                                    "QPushButton:checked{border:none;background-color:#E44C50;border-radius:4px;}");
    } else {
        // 白色主题或默认主题
        m_pCloseButton->setStyleSheet("QPushButton{border:none;background-color:#DCDCDC;border-radius:4px;}"
                                  "QPushButton:hover{border:none;background-color:#F86457;border-radius:4px;}"
                                    "QPushButton:checked{border:none;background-color:#E44C50;border-radius:4px;}");
    }

    pLayout->addSpacing(0);
    pLayout->addWidget (m_logo);
    pLayout->addWidget (m_logoMsg);
    pLayout->addStretch();
    pLayout->addWidget(m_pMinimizeButton);
    pLayout->addWidget(m_pMaximizeButton);
    pLayout->addWidget(m_pCloseButton);
    pLayout->setSpacing(7);
    pLayout->setAlignment(Qt::AlignCenter);
    pLayout->setContentsMargins(7, 0, 7, 0);
    setLayout(pLayout);

    connect(m_pMinimizeButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
    connect(m_pMaximizeButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
    connect(m_pCloseButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
    connect(style_settings,SIGNAL(changed(QString)),this,SLOT(titlebar_style_changed(QString)));
    connect(icon_theme_settings,SIGNAL(changed(QString)), this, SLOT(titlebar_icon_theme_changed(QString)));
}

TitleBar::~TitleBar()
{

}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    emit m_pMaximizeButton->clicked();
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons().testFlag(Qt::LeftButton) && mMoving)
    {
        QWidget *pWindow = this->window();
        pWindow->move(pWindow->pos() + (event->globalPos() - mLastMousePosition));
        mLastMousePosition = event->globalPos();
    }
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        mMoving = true;
        mLastMousePosition = event->globalPos();
    }
}

void TitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        mMoving = false;
    }
}

bool TitleBar::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type())
    {
        case QEvent::WindowStateChange:
        case QEvent::Resize:
            updateMaximize();
            return true;
        default:
            break;
    }
    return QWidget::eventFilter(obj, event);
}

void TitleBar::onClicked()
{
    QPushButton *pButton = qobject_cast<QPushButton *>(sender());
    QWidget *pWindow = this->window();
    if (pWindow->isTopLevel())
    {
        if (pButton == m_pMinimizeButton)
        {
            pWindow->showMinimized();
        }
        else if (pButton == m_pMaximizeButton)
        {
            pWindow->isMaximized() ? pWindow->showNormal() : pWindow->showMaximized();
        }
        else if (pButton == m_pCloseButton)
        {
            pWindow->close();
        }
    }
}

void TitleBar::titlebar_icon_theme_changed(QString)
{
    qDebug() << "titlebar icon-theme: " << icon_theme_settings->get(ICON_THEME_NAME).toString();

    if ("ukui-icon-theme-basic" == icon_theme_settings->get(ICON_THEME_NAME).toString()) {
        m_logo->setStyleSheet("QLabel{border-image:url(/usr/share/icons/ukui-icon-theme-basic/24x24/devices/scanner.png);border-radius:4px;}");
    } else if ("ukui-icon-theme-classical" == icon_theme_settings->get(ICON_THEME_NAME).toString()) {
        m_logo->setStyleSheet("QLabel{border-image:url(/usr/share/icons/ukui-icon-theme-classical/24x24/devices/scanner.png);border-radius:4px;}");
    } else if ("ukui-icon-theme-default" == icon_theme_settings->get(ICON_THEME_NAME).toString()) {
        m_logo->setStyleSheet("QLabel{border-image:url(/usr/share/icons/ukui-icon-theme-default/24x24/devices/scanner.png);border-radius:4px;}");
    }
}

void TitleBar::titlebar_style_changed(QString)
{
    qDebug() << "titlebar_style_changed = " << style_settings->get (STYLE_NAME).toString ();
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        // 黑色主题或默认主题
        m_pCloseButton->setStyleSheet("QPushButton{border:none;background-color:#3D3D41;border-radius:4px;}"
                                  "QPushButton:hover{border:none;background-color:#F86457;border-radius:4px;}"
                                    "QPushButton:checked{border:none;background-color:#E44C50;border-radius:4px;}");
    } else {
        // 白色主题
        m_pCloseButton->setStyleSheet("QPushButton{border:none;background-color:#DCDCDC;border-radius:4px;}"
                                  "QPushButton:hover{border:none;background-color:#F86457;border-radius:4px;}"
                                    "QPushButton:checked{border:none;background-color:#E44C50;border-radius:4px;}");
    }
}

void TitleBar::updateMaximize()
{
    QWidget *pWindow = this->window();
    if (pWindow->isWindow ())
    {
        bool bMaximize = pWindow->isMaximized();
        if (bMaximize)
        {
            m_pMaximizeButton->setToolTip(tr("Restore"));
            m_pMaximizeButton->setProperty("maximizeProperty", "restore");
            m_pMaximizeButton->setIcon (QIcon::fromTheme (ICON_THEME_RESTORE));
            emit isMax();
        }
        else
        {
            m_pMaximizeButton->setProperty("maximizeProperty", "maximize");
            m_pMaximizeButton->setToolTip(tr("Maximize"));
            m_pMaximizeButton->setIcon (QIcon::fromTheme (ICON_THEME_MAXIMAZE));
            emit isNormal();
        }
    }
}
