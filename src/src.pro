#-------------------------------------------------
# Copyright (C) 2020, KylinSoft Co., Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
#
#-------------------------------------------------

QT += core gui network printsupport concurrent KWindowSystem dbus x11extras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 1.0.12
TARGET = kylin-scanner
TEMPLATE = app

CONFIG        += link_pkgconfig c++11
PKGCONFIG     += gio-2.0 gio-unix-2.0 opencv4 gsettings-qt
LIBS += -llept -ltesseract -lpthread -lX11

include(src.pri)

DEFINES += QT_MESSAGELOGCONTEXT # for message line, file and date
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
#release:DEFINES += QT_NO_WARNING_OUTPUT QT_NO_DEBUG_OUTPUT
#DEFINES += DEBUG_EDIT
#DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#DISTRIBUTION = $$system(cat /etc/kylin-build | grep Kylin | cut -d\' \' -f2)
#count($$DISTRIBUTION,"V10"){
#        DEFINES += __V10__
#        QT      -= x11extras
#        LIBS    -= -lpthread
#        LIBS    -= -lX11
#}

RESOURCES += res.qrc

unix:!macx: LIBS += -L$$PWD/../../../usr/lib/x86_64-linux-gnu/ -lsane

INCLUDEPATH += $$PWD/../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../usr/lib/x86_64-linux-gnu

TRANSLATIONS = translations/kylin-scanner.zh_CN.ts
qm_files.files = translations/*.qm
qm_files.path = /usr/share/kylin-scanner/translations/

# target
target.files += $$TARGET
target.path = /usr/bin/

# icons
icons.files += data/scanner.png
icons.path = /usr/share/pixmaps/

# desktop
desktop.files += data/kylin-scanner.desktop
desktop.path = /usr/share/applications/

INSTALLS += target icons desktop qm_files

DISTFILES += \
    icon/scanner.png \
    translations/generate_translations_pm.sh \
    translations/local.zh_CN.qm
