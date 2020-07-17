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
#ifndef SCAN_SET_H
#define SCAN_SET_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QDebug>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QListView>
#include <QDialog>
#include <QFileDialog>
#include <QCoreApplication>
#include <QFont>
#include <QPixmap>
#include <QLineEdit>
#include <QProcess>
#include <QFile>

#include "kylincombobox.h"
#include "send_mail.h"
#include "kylin_sane.h"

class ScanSet  : public QWidget
{
    Q_OBJECT

public:
    explicit ScanSet(QWidget *parent = nullptr);
    ~ScanSet();

    /**
     * @brief setKylinComboBox 统一设置麒麟扫描组合框ComboBox
     */
    void setKylinComboBox(bool curIndexChanged);

    /**
     * @brief setKylinComboBoxScanName 设置麒麟扫描组合框的扫描设备名
     */
    void setKylinComboBoxScanDeviceName();

    void setKylinScanSetNotEnable();
    void setKylinScanSetEnable();

    /**
     * @brief setKylinComboBoxAttributes 设置组合框属性
     * @param combo 需要属性设置的组合框
     * @param strList 组合框中文本框值
     */
    void setKylinComboBoxAttributes(KylinComboBox *combo, QStringList strList);

    /**
     * @brief setKylinLable 统一设置麒麟扫描标签Label
     */
    void setKylinLable();

    /**
     * @brief setKylinLabelAttributes 设置标签属性
     * @param label 需要属性设置的标签
     */
    void setKylinLabelAttributes(QLabel *label);

    /**
     * @brief setKylinHBoxLayout 统一设置麒麟扫描水平布局HBoxLayout
     */
    void setKylinHBoxLayout();

    /**
     * @brief setKylinHBoxLayoutAttributes 设置水平布局属性： 标签1和标签2
     * @param layout 需要设置的水平布局
     * @param labelFirst 水平布局中的第一个标签
     * @param labelSecond 水平布局中的第二个标签
     */
    void setKylinHBoxLayoutAttributes(QHBoxLayout *layout, QLabel *labelFirst, QLabel *labelSecond);
    /**
     * @brief setKylinHBoxLayoutAttributes 重载设置水平布局属性： 标签和组合框
     * @param layout 需要设置的水平布局
     * @param labelFirst 水平布局中的标签
     * @param combo 水平布局中的组合框
     */
    void setKylinHBoxLayoutAttributes(QHBoxLayout *layout, QLabel *labelFirst, KylinComboBox *combo);
    QString getTextResolution();
    QString getTextSize();
    QString getTextFormat();
    QString getTextName();
    QString getTextLocation();
    void setFontSize(QLabel *label, int n);

Q_SIGNALS:
//signals:
    void save_image_signal(QString);
    void open_device_status(bool);

private:
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

    KylinComboBox *textDevice;      /**< 设备 */
    QLabel *textType;               /**< 类型 */
    KylinComboBox *textColor;       /**< 色彩 */
    KylinComboBox *textResolution;  /**< 分辨率 */
    KylinComboBox *textSize;        /**< 尺寸 */
    KylinComboBox *textFormat;      /**< 格式 */
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
    QVBoxLayout *vBoxScanSet;
    QVBoxLayout *vBoxScanSet1;
    QHBoxLayout *hBoxScanSet;
    int flag = 0;

public slots:
    void modify_save_button();

private slots:
    void on_btnLocation_clicked();
    void on_btnMail_clicked();
    void on_btnSave_clicked();

    void on_textDevice_current_text_changed(QString device);
    void on_textColor_current_text_changed(QString color);
    void on_textResolution_current_text_changed(QString resolution);
    void on_textSize_current_text_changed(QString size);
};

#endif // SCAN_SET_H
