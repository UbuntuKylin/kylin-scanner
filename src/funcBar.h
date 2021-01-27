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

#ifndef FUNC_BAR_H
#define FUNC_BAR_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>
#include <QString>
#include <QDebug>
#include <QInputEvent>
#include <QStack>
#include <QThread>
#include <QMovie>
#include <QTextDocument>
#include <QMessageBox>
#include "kylinSane.h"
#include "beauty.h"
#include "theme.h"

class ThreadScanFuncBar : public QThread
{
    Q_OBJECT

public:
    void run() Q_DECL_OVERRIDE;

signals:
    void scanFinishedFuncBar(int);
};

class FuncBar : public QWidget
{
    Q_OBJECT

public:
    explicit FuncBar(QWidget *parent = nullptr);
    ~FuncBar();

    int flagBeautify = 0; //一键美化标志
    int flagRectify = 0; //智能纠偏标志
    int flagOrc = 0; //文字识别标志

    void keyPressEvent(QKeyEvent *e);
    void setKylinScanSetNotEnable();
    void setKylinScanSetEnable();
    void setBtnScanEnable();
    void setFontSize(QLabel *label, int n);
    void setStackClear();
    void warnMsg(QString msg);

private:
    QStringList stylelist;
    QStringList iconthemelist;
    QGSettings *style_settings;
    QGSettings *icon_theme_settings;

    QPushButton *btnNorScan ;
    QPushButton *btnBeautify;
    QPushButton *btnRectify;
    QPushButton *btnOrc;
    QPushButton *btnScan;
    QMovie *movieScan; // 加载扫描动态图片GIF
    QLabel *labMovieScan; // 加载扫描动态图片标签
    QLabel *labNorScan;
    QLabel *labBeautify;
    QLabel *labRectify;
    QLabel *labOrc;
    QFrame *line1; // 左边第一个竖线，已屏蔽
    QFrame *line2;
    QVBoxLayout *vBoxLay1;
    QVBoxLayout *vBoxLay2;
    QVBoxLayout *vBoxLay3;
    QVBoxLayout *vBoxLay4;
    QHBoxLayout *hBoxLay1;
    QHBoxLayout *hBoxLay2;
    QHBoxLayout *hBoxLay3;
    QHBoxLayout *hBoxLay4;
    QStack<QString> stack;
    ThreadScanFuncBar thread;

private slots:
    void onBtnOrcClicked();
    void onBtnScanClicked();
    void onBtnRectifyClicked();
    void onBtnBeautyClicked();
    void scanResult(int ret);
    void funcbar_style_changed(QString); // 系统主题样式变更

Q_SIGNALS:
    void sendOrcBegin();
    void sendOrcEnd();
    void sendScanEnd(bool);
    void sendRectifyBegin();
    void sendRectifyEnd();
    void sendBeautifyBegin();
    void sendBeautifyEnd();

};
#endif // FUNC_BAR_H
