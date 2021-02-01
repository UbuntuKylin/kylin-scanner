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

#ifndef SCAN_SET_H
#define SCAN_SET_H

#include <QStyledItemDelegate>
#include <QListView>
#include <QFileDialog>
#include <QCoreApplication>
#include <QFont>
#include <QPixmap>
#include <QLineEdit>
#include <QFile>
#include <QHash>
#include "kylinCmb.h"
#include "sendMail.h"
#include "kylinSane.h"
#include "theme.h"

class ScanSet  : public QWidget
{
    Q_OBJECT

public:
    explicit ScanSet(QWidget *parent = nullptr);
    ~ScanSet();

    void setKylinComboBox(bool curIndexChanged);
    void setKylinComboBoxScanDeviceName();
    void setKylinScanSetNotEnable();
    void setKylinScanSetBtnNotEnable();
    void setKylinScanSetBtnEnable();
    void setKylinScanSetEnable();
    void setKylinComboBoxAttributes(KylinCmb *combo, QStringList strList);
    void setKylinComboBoxTextDeviceAttributes(KylinCmb *combo, QStringList strList);
    void setKylinLable();
    void setKylinLabelAttributes(QLabel *label);
    void setKylinHBoxLayout();
    void setKylinHBoxLayoutAttributes(QHBoxLayout *layout, QLabel *labelFirst, QLabel *labelSecond);
    void setKylinHBoxLayoutAttributes(QHBoxLayout *layout, QLabel *labelFirst, KylinCmb *combo);
    void setFontSize(QLabel *label, int n);
    void setTextNameToolTip();
    void setBtnSaveText();

    QString getTextResolution();
    QString getTextSize();
    QString getTextFormat();
    QString getTextName();
    QString getTextLocation();

    void warnMsg(QString msg);

Q_SIGNALS:
    void saveImageSignal(QString);
    void openDeviceStatusSignal(bool);

private:
    int scanFlag = 0;
    QStringList stylelist;
    QStringList iconthemelist;
    QGSettings *style_settings;
    QGSettings *icon_theme_settings;
    QLabel *labDevice;              /**< 设备标签 */
    QLabel *labType;                /**< 类型标签 */
    QLabel *labColor;               /**< 色彩标签 */
    QLabel *labResolution;          /**< 分辨率标签 */
    QLabel *labSize;                /**< 尺寸标签 */
    QLabel *labFormat;              /**< 格式标签 */
    QLabel *labName;                /**< 名称标签 */
    QLabel *labLocation;            /**< 扫描至标签 */

    QFrame *line3;
    QFrame *line4;

    QPushButton *btnMail;           /**< 发送至邮件 */
    QPushButton *btnSave;           /**< 另存为 */
    QPushButton *btnLocation;       /**< 扫描至 */
    QString curPath;

    KylinCmb *textDevice;      /**< 设备 */
    QLabel *textType;               /**< 类型 */
    KylinCmb *textColor;       /**< 色彩 */
    KylinCmb *textResolution;  /**< 分辨率 */
    KylinCmb *textSize;        /**< 尺寸 */
    KylinCmb *textFormat;      /**< 格式 */
    QLineEdit *textName;            /**< 名称 */

    QHBoxLayout *hBoxDevice;
    QHBoxLayout *hBoxType;
    QHBoxLayout *hBoxColor;
    QHBoxLayout *hBoxResolution;
    QHBoxLayout *hBoxSize;
    QHBoxLayout *hBoxFormat;
    QHBoxLayout *hBoxName;
    QHBoxLayout *hBoxLocation;
    QHBoxLayout *hBoxLine3;
    QHBoxLayout *hBoxLine4;
    QHBoxLayout *hBoxMailText;
    QHBoxLayout *hBoxScanSet;
    QVBoxLayout *vBoxScanSet;
    QVBoxLayout *vBoxScanSet1;

    int flag = 0; // 当前按钮为另存为还是文字识别后的存储文本

public slots:
    void modifyBtnSave();

private slots:
    void onBtnLocationClicked();
    void onBtnMailClicked();
    void onBtnSaveClicked();
    void onTextDeviceCurrentTextChanged(QString device);
    void onTextColorCurrentTextChanged(QString color);
    void onTextResolutionCurrentTextChanged(QString resolution);
    void onTextSizeCurrentTextChanged(QString size);
    void onTextNameTextChanged(QString);
    void scanset_style_changed(QString); // 系统主题风格变化
};

#endif // SCAN_SET_H
