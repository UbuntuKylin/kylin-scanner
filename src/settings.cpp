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

#include "settings.h"
#include "savefile.h"
#include <QMessageBox>
#include <QTextCursor>
#include <QStandardItemModel>
#include <QFileInfo>
#include <QWidgetList>
#include <QApplication>

KYCScanSettingsWidget::KYCScanSettingsWidget(QWidget *parent)
    : QWidget(parent)
    , style_settings (new QGSettings(ORG_UKUI_STYLE))
    , icon_theme_settings (new QGSettings(ORG_UKUI_STYLE))
    , labDevice (new QLabel())
    , labType (new QLabel())
    , labColor (new QLabel())
    , labResolution (new QLabel())
    , labSize (new QLabel())
    , labFormat (new QLabel())
    , labName (new QLabel() )
    , labLocation (new QLabel())
    , line3 (new QFrame())
    , line4 (new QFrame())
    , btnMail (new QPushButton())
    , btnSave (new QPushButton())
    , btnLocation (new QPushButton())
    , textDevice (new KYCComboBox())
    , textType (new QLabel())
    , textColor (new KYCComboBox())
    , textResolution (new KYCComboBox())
    , textSize (new KYCComboBox())
    , textFormat (new KYCComboBox())
    , textName (new QLineEdit())
    , hBoxDevice (new QHBoxLayout())
    , hBoxType (new QHBoxLayout())
    , hBoxColor (new QHBoxLayout())
    , hBoxResolution (new QHBoxLayout())
    , hBoxSize (new QHBoxLayout())
    , hBoxFormat (new QHBoxLayout())
    , hBoxName (new QHBoxLayout())
    , hBoxLocation (new QHBoxLayout())
    , hBoxLine3 (new QHBoxLayout())
    , hBoxLine4 (new QHBoxLayout())
    , hBoxMailText (new QHBoxLayout())
    , hBoxScanSet (new QHBoxLayout())
    , vBoxScanSet (new QVBoxLayout())
    , vBoxScanSet1 (new QVBoxLayout())
{
    initWindow();

    initLayout();

    initStyle();

    initSettings();

    initConnect();
}

KYCScanSettingsWidget::~KYCScanSettingsWidget()
{

}

void KYCScanSettingsWidget::initWindow()
{
    setFixedWidth(268);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void KYCScanSettingsWidget::initLayout()
{
    line3->setObjectName(QString::fromUtf8("line3"));
    line3->setMaximumHeight(1);
    line3->setMaximumWidth(230);
    line3->setMinimumWidth(230);
    line3->setFrameShape(QFrame::HLine);

    line4->setObjectName(QString::fromUtf8("line4"));
    line4->setMaximumHeight(1);
    line4->setMaximumWidth(230);
    line4->setMinimumWidth(230);
    line4->setFrameStyle(QFrame::HLine);

    btnMail->setText(tr("Send email to"));
    btnMail->setFixedSize(120, 32);
    btnSave->setText(tr("Save as"));
    btnSave->setFixedSize(100, 32);

    QFontMetrics elideFont(btnLocation->font());
    if (curPath.isEmpty())
        curPath = QDir::homePath() ; //获取家目录的路径

    btnLocation->setText(elideFont.elidedText(curPath, Qt::ElideRight, 150));
    btnLocation->setFixedSize(180, 32);

    btnLocation->setIcon(QIcon::fromTheme("folder-open"));
    btnLocation->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

    vBoxScanSet->setSpacing(0);
    vBoxScanSet->addLayout(hBoxDevice);
    vBoxScanSet->addLayout(hBoxType);
    vBoxScanSet->addLayout(hBoxColor);
    vBoxScanSet->addLayout(hBoxResolution);
    vBoxScanSet->addLayout(hBoxSize);
    vBoxScanSet->addLayout(hBoxLine3);
    vBoxScanSet->addLayout(hBoxFormat);
    vBoxScanSet->addLayout(hBoxName);
    vBoxScanSet->addLayout(hBoxLocation);
    vBoxScanSet->addStretch();
    vBoxScanSet->addLayout(hBoxLine4);
    vBoxScanSet->addLayout(hBoxMailText);
    vBoxScanSet->setContentsMargins(0, 0, 0, 0);

    setLayout(vBoxScanSet);

}

void KYCScanSettingsWidget::initStyle()
{
    stylelist << STYLE_NAME_KEY_DARK << STYLE_NAME_KEY_BLACK;
    iconthemelist << ICON_THEME_KEY_BASIC << ICON_THEME_KEY_CLASSICAL << ICON_THEME_KEY_DEFAULT;

    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        // 背景颜色
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(32, 30, 29));
        setAutoFillBackground(true);
        setPalette(pal);

        btnMail->setStyleSheet("QPushButton{background-color:rgb(32,30,29);border:1px solid #939393;color:rgb(232,232,232);border-radius:4px;}"
                               "QPushButton:hover{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}"
                               "QPushButton:checked{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}");
        btnSave->setStyleSheet("QPushButton{background-color:rgb(32,30,29);border:1px solid #939393;color:rgb(232,232,232);border-radius:4px;}"
                               "QPushButton:hover{border:none;background-color:#3D6BE5;color:rgb(232,232,232);border-radius:4px;}"
                               "QPushButton:checked{border:none;background-color:#3D6BE5;color:rgb(232,232,232)border-radius:4px;}");
        btnLocation->setStyleSheet("QPushButton{background-color:#0D0400;border:4px solid #0D0400;background-repeat:no-repeat;background-position:right;color:rgb(232,232,232);border-radius:4px;text-align:left;}");
        line3->setStyleSheet("QFrame{color:#201E1D}");
        line4->setStyleSheet("QFrame{color:#201E1D}");
    } else {
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(249, 249, 249));
        setAutoFillBackground(true);
        setPalette(pal);

        btnMail->setStyleSheet("QPushButton{background-color:#F9F9F9;border:1px solid #939393;color:#000000;border-radius:4px;}"
                               "QPushButton:hover{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:#000000;border-radius:4px;}"
                               "QPushButton:checked{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:#000000;border-radius:4px;}");
        btnSave->setStyleSheet("QPushButton{background-color:#F9F9F9;border:1px solid #939393;color:#000000;border-radius:4px;}"
                               "QPushButton:hover{border:none;background-color:#3D6BE5;color:#000000;border-radius:4px;}"
                               "QPushButton:checked{border:none;background-color:#3D6BE5;color:#000000;border-radius:4px;}");
        btnLocation->setStyleSheet("QPushButton{background-color:#F9F9F9;background-repeat:no-repeat;background-position:right;color:#000000;border-radius:4px;text-align:left;}");
        line3->setStyleSheet("QFrame{color:#DCDCDC}");
        line4->setStyleSheet("QFrame{color:#DCDCDC}");

    }
}

void KYCScanSettingsWidget::initSettings()
{
    textName->setText("scanner01");

    setKylinLable();

    setKylinComboBox(false);

    setKylinHBoxLayout();

    // For current combobox text, while not change current text
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    QString curDeviceName, curSize, curColor, curResolution;

    curDeviceName = textDevice->currentText();
    instance.userInfo.name = curDeviceName;

    curSize = textSize->currentText();
    instance.userInfo.size = curSize;

    curColor = textColor->currentText();
    // Do not direct to return color, because color has been tr()
    if (! QString::compare("黑白", curColor)
            || ! QString::compare("Lineart", curColor)) {
        instance.userInfo.color = "Lineart";
    } else if (! QString::compare("彩色", curColor)
               || ! QString::compare("Color", curColor)) {
        instance.userInfo.color = "Color";
    } else if (! QString::compare("灰度", curColor)
               || ! QString::compare("Gray", curColor)) {
        instance.userInfo.color = "Gray";
    }
    qDebug() << "userInfo.color = " << instance.userInfo.color;
    instance.userInfo.color = curColor;

    curResolution = textResolution->currentText();
    instance.userInfo.resolution = curResolution;
}

void KYCScanSettingsWidget::initConnect()
{
    // For save location
    connect(btnLocation, SIGNAL(clicked()), this, SLOT(onBtnLocationClicked()));

    // For send email
    connect(btnMail, SIGNAL(clicked()), this, SLOT(onBtnMailClicked()));

    // For save file
    connect(btnSave, SIGNAL(clicked()), this, SLOT(onBtnSaveClicked()));

    // For device name changed
    connect(textDevice, SIGNAL(currentTextChanged(QString)), this,
            SLOT(onTextDeviceCurrentTextChanged(QString)));

    // For textName changed
    connect (textName, SIGNAL (textChanged(QString)), this,
             SLOT(onTextNameTextChanged(QString)));

    // For color mode changed
    connect(textColor, SIGNAL(currentTextChanged(QString)), this,
            SLOT(onTextColorCurrentTextChanged(QString)));

    // For resolution changed
    connect(textResolution, SIGNAL(currentTextChanged(QString)), this,
            SLOT(onTextResolutionCurrentTextChanged(QString)));

    // For size changed
    connect(textSize, SIGNAL(currentTextChanged(QString)), this,
            SLOT(onTextSizeCurrentTextChanged(QString)));

    connect(style_settings, SIGNAL(changed(QString)), this,
            SLOT(scanset_style_changed(QString)));
}

/**
 * @brief setKylinComboBoxAttributes 设置组合框属性
 * @param combo 需要属性设置的组合框
 * @param strList 组合框中文本框值
 */
void KYCScanSettingsWidget::setKylinComboBoxAttributes(KYCComboBox *combo, QStringList strList)
{
    QListView *listView = new QListView(this);

    combo->clear();
    combo->addItems(strList);
    combo->setFixedSize(180, 32);
    combo->setInsertPolicy(QComboBox::NoInsert);  //编辑框的内容不插入到列表项
    combo->setFocusPolicy(Qt::NoFocus); //获取焦点策略：无焦点，也就是不可编辑
    combo->setModel(listView->model()); // avoid warning
    combo->setView(listView);   //使下拉选项样式生效
}

void KYCScanSettingsWidget::setKylinComboBoxTextDeviceAttributes(KYCComboBox *combo, QStringList strList)
{
    QListView *listView = new QListView(this);

    combo->clear();

    QStandardItemModel *model = new QStandardItemModel();
    for (int i = 0; i < strList.size(); ++i) {
        //添加提示tootip
        QStandardItem *item = new QStandardItem(strList.at(i));;
        int curTextLen = strList.at(i).length();
        if ( curTextLen >= 20) {
            item->setToolTip(strList.at(i));
        } else {
            item->setToolTip("");
        }
        model->appendRow(item);
        //editor->addItem(Items.at(i));
    }
    combo->setModel(model);
    combo->setFixedSize(180, 32);
    combo->setInsertPolicy(QComboBox::NoInsert);  //编辑框的内容不插入到列表项
    combo->setFocusPolicy(Qt::NoFocus); //获取焦点策略：无焦点，也就是不可编辑
    combo->setView(listView);   //使下拉选项样式生效
}

/**
 * @brief setKylinComboBox 统一设置麒麟扫描组合框ComboBox
 */
void KYCScanSettingsWidget::setKylinComboBox(bool curIndexChanged)
{
    QStringList strListDevice, strListColor, strListResolution, strListFormat, strListSize,
                strListLocation;
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    bool device_status = true;
    int defaultResolution = 0;
    int defaultSize = 0;

    device_status = instance.getKylinSaneStatus();

    if (!device_status) {
        // If not find scan device
        strListDevice << tr("No available device");
        setKylinComboBoxTextDeviceAttributes(textDevice, strListDevice);

        strListColor << tr("Lineart") << tr("Color") << tr("Gray");
        setKylinComboBoxAttributes(textColor, strListColor);

        //strListResolution << "4800" << "2400" << "1200" << "600" << "300" << tr("Auto");
        strListResolution << "300" << "600" << "1200" << "2400" << "4800" << tr("Auto");
        for (int i = 0; i < strListResolution.size(); ++i) {
            if (!QString::compare("300", strListResolution[i], Qt::CaseSensitive)) {
                defaultResolution = i;
                break;
            }
        }
        setKylinComboBoxAttributes(textResolution, strListResolution);
        textResolution->setCurrentIndex(defaultResolution);

        strListSize << "A4" << "A5";
        setKylinComboBoxAttributes(textSize, strListSize);

        strListFormat << "jpg" << "png" << "pdf" << "bmp";
        setKylinComboBoxAttributes(textFormat, strListFormat);

        return;
    }

    // For  default device
    if (!curIndexChanged) { // 当选择设备时，索引发生改变，此时不应该按照读取的字符串进行默认设置
        strListDevice = instance.getKylinSaneNames();
        qDebug() << "sane names: " << strListDevice;
        setKylinComboBoxTextDeviceAttributes(textDevice, strListDevice);
    }

    // For  default color
    strListColor = instance.getKylinSaneColors();
    int defaultColor = 0;

    for (int i = 0; i < strListColor.size(); ++i) {
        if (! QString::compare("黑白", strListColor[i], Qt::CaseSensitive)
                || ! QString::compare("Lineart", strListColor[i], Qt::CaseSensitive)) {
            defaultColor = i;
            break;
        }
    }

    setKylinComboBoxAttributes(textColor, strListColor);
    textColor->setCurrentIndex(defaultColor);

    // For  default resolution
    strListResolution = instance.getKylinSaneResolutions();

    for (int i = 0; i < strListResolution.size(); ++i) {
        if (! QString::compare("300", strListResolution[i], Qt::CaseSensitive)) {
            defaultResolution = i;
            break;
        }
    }

    setKylinComboBoxAttributes(textResolution, strListResolution);
    textResolution->setCurrentIndex(defaultResolution);

    // For  default sizes
    strListSize = instance.getKylinSaneSizes();

    for (int i = 0; i < strListSize.size(); ++i) {
        if (! QString::compare("A4", strListSize[i], Qt::CaseSensitive)) {
            defaultSize = i;
            break;
        }
    }

    setKylinComboBoxAttributes(textSize, strListSize);
    textSize->setCurrentIndex(defaultSize);

    strListFormat << "jpg" << "png" << "pdf" << "bmp";
    setKylinComboBoxAttributes(textFormat, strListFormat);
}

/**
* @brief setKylinComboBoxScanName 设置麒麟扫描组合框的扫描设备名
* set textDevice while find scanners
 */
void KYCScanSettingsWidget::setKylinComboBoxScanDeviceName()
{
    QStringList strListDevice;
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    bool device_status = true;

    device_status = instance.getKylinSaneStatus();

    if (!device_status) {
        // If not find scan device
        strListDevice << tr("No available device");
        setKylinComboBoxTextDeviceAttributes(textDevice, strListDevice);

        return;
    }

    // For  default device
    strListDevice = instance.getKylinSaneNames();
    qDebug() << "sane names: " << strListDevice;
    setKylinComboBoxTextDeviceAttributes(textDevice, strListDevice);
}

/**
 * @brief ScanSet::setkylinScanStatusFalse
 * set connect scan device status false for widget.cpp
 */
void KYCScanSettingsWidget::setkylinScanStatus(bool status)
{
    qDebug() << "set kylin scanner status: " << status;
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    instance.setKylinSaneStatus(status);

    if (!status)
        scanExecFlag = 0;

    // update
//    QStringList strListDevice;
//    // update textDevice will emit signal
//    strListDevice << tr("No available device");
//    setKylinComboBoxTextDeviceAttributes(textDevice, strListDevice);
}

/**
 * @brief KYCScanSettingsWidget::setKylinScanSetNotEnable
 * Case 1: Cannot click btnMail and btnSave before click btnScan
 */
void KYCScanSettingsWidget::setKylinScanSetNotEnable()
{
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    bool device_status = true;

    device_status = instance.getKylinSaneStatus();

    if (!device_status) {
        textColor->setEnabled(false);
        textColor->colorGray();

        textSize->setEnabled(false);
        textSize->colorGray();

        textResolution->setEnabled(false);
        textResolution->colorGray();

        textFormat->setEnabled(false);
        textFormat->colorGray();

        textName->setEnabled(false);

        if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
            textName->setStyleSheet("QLineEdit{border:1px solid #0D0400;background-color:rgb(15,08,01);color:gray;border-radius:4px;}");
            textType->setStyleSheet("QLabel{border:1px solid #0D0400;background-color:rgb(15,08,01);color:gray;border-radius:4px;}");
            btnLocation->setStyleSheet("QPushButton{border:4px solid #0D0400;background-repeat:no-repeat;background-position:right;background-color:#0D0400;color:gray;border-radius:4px;text-align:left;}");
            btnMail->setStyleSheet("QPushButton{background-color:rgb(32,30,29);border:1px solid #939393;color:gray;border-radius:4px;}"
                                   "QPushButton:hover{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}"
                                   "QPushButton:checked{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}");
            btnSave->setStyleSheet("QPushButton{background-color:rgb(32,30,29);border:1px solid #939393;color:gray;border-radius:4px;}"
                                   "QPushButton:hover{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}"
                                   "QPushButton:checked{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}");
            //textDevice->colorGray (); // 主题颜色变换
        } else {
            textType->setStyleSheet("QLabel{background-color:#E7E7E7;color:gray;border-radius:4px;}");
            textName->setStyleSheet("QLineEdit{background-color:#E7E7E7;color:gray;border-radius:4px;}");
            btnLocation->setStyleSheet("QPushButton{background-color:#E7E7E7;background-repeat:no-repeat;background-position:right;color:gray;border-radius:4px;text-align:left;}");
            btnMail->setStyleSheet("QPushButton{background-color:#F9F9F9;border:1px solid #8E8E8E;color:gray;opacity:0.85;border-radius:4px;}"
                                   "QPushButton:hover{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}"
                                   "QPushButton:checked{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}");
            btnSave->setStyleSheet("QPushButton{background-color:#F9F9F9;border:1px solid #8E8E8E;color:gray;opacity:0.85;border-radius:4px;}"
                                   "QPushButton:hover{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}"
                                   "QPushButton:checked{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}");
            //textDevice->colorGray (); // 主题颜色变换
        }

        btnLocation->setEnabled(false);
        btnMail->setEnabled(false);
        btnSave->setEnabled(false);
        textType->setEnabled(false);
    }
}

/**
 * @brief ScanSet::setKylinScanSetBtnNotEnable
 * 发送邮件和另存为按钮置灰，不能点击
 */
void KYCScanSettingsWidget::setKylinScanSetBtnNotEnable()
{
    btnMail->setEnabled(false);
    btnSave->setEnabled(false);
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        btnMail->setStyleSheet("QPushButton{background-color:rgb(32,30,29);border:1px solid #939393;color:gray;border-radius:4px;}"
                               "QPushButton:hover{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:gray;border-radius:4px;}"
                               "QPushButton:checked{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:gray;border-radius:4px;}");

        btnSave->setStyleSheet("QPushButton{background-color:rgb(32,30,29);border:1px solid #939393;color:gray;border-radius:4px;}"
                               "QPushButton:hover{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:gray;border-radius:4px;}"
                               "QPushButton:checked{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:gray;border-radius:4px;}");
    } else {
        btnMail->setStyleSheet("QPushButton{background-color:#CCCCCC;border:1px solid #8E8E8E;color:#000000;opacity:0.85;border-radius:4px;}"
                               "QPushButton:hover{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:gray;border-radius:4px;}"
                               "QPushButton:checked{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:gray;border-radius:4px;}");
        btnSave->setStyleSheet("QPushButton{background-color:#CCCCCC;border:1px solid #8E8E8E;color:#000000;opacity:0.85;border-radius:4px;}"
                               "QPushButton:hover{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:gray;border-radius:4px;}"
                               "QPushButton:checked{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:gray;border-radius:4px;}");
    }
}

void KYCScanSettingsWidget::setKylinScanSetBtnEnable()
{
    btnMail->setEnabled(true);
    btnSave->setEnabled(true);
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        btnMail->setStyleSheet("QPushButton{background-color:rgb(32,30,29);border:1px solid #939393;color:rgb(232,232,232);border-radius:4px;}"
                               "QPushButton:hover{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}"
                               "QPushButton:checked{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}");

        btnSave->setStyleSheet("QPushButton{background-color:rgb(32,30,29);border:1px solid #939393;color:rgb(232,232,232);border-radius:4px;}"
                               "QPushButton:hover{border:none;background-color:#3D6BE5;color:rgb(232,232,232);border-radius:4px;}"
                               "QPushButton:checked{border:none;background-color:#3D6BE5;color:rgb(232,232,232)border-radius:4px;}");
    } else {
        btnMail->setStyleSheet("QPushButton{background-color:#F9F9F9;border:1px solid #8E8E8E;color:#000000;opacity:0.85;border-radius:4px;}"
                               "QPushButton:hover{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}"
                               "QPushButton:checked{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}");
        btnSave->setStyleSheet("QPushButton{background-color:#F9F9F9;border:1px solid #8E8E8E;color:#000000;opacity:0.85;border-radius:4px;}"
                               "QPushButton:hover{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}"
                               "QPushButton:checked{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}");
    }
}

void KYCScanSettingsWidget::setKylinScanSetEnable()
{
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    bool device_status = true;

    device_status = instance.getKylinSaneStatus();

    if (device_status) {
        textDevice->setEnabled(true);
        textDevice->colorInit();

        textColor->setEnabled(true);
        textColor->colorInit();

        textSize->setEnabled(true);
        textSize->colorInit();

        textResolution->setEnabled(true);
        textResolution->colorInit();

        textFormat->setEnabled(true);
        textFormat->colorInit();

        textName->setEnabled(true);
        btnLocation->setEnabled(true);

        if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
            btnLocation->setStyleSheet("QPushButton{border:4px solid #0D0400;background-position:right;background-color:#0D0400;color:rgb(232,232,232);border-radius:4px;text-align:left;}");
        } else {
            btnLocation->setStyleSheet("QPushButton{background-color:#E7E7E7;background-position:right;color:#000000;border-radius:4px;text-align:left;}");
        }
    }
}

/**
 * @brief setKylinLable 统一设置麒麟扫描标签Label
 * This function should not set variable value,
 * because it will be called by other functions.
 */
void KYCScanSettingsWidget::setKylinLable()
{
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    bool device_status = true;

    device_status = instance.getKylinSaneStatus();

    labDevice->setText(tr("Device"));
    //setFontSize(labDevice, 10);
    setKylinLabelAttributes(labDevice);

    labType->setText(tr("Type"));
    //setFontSize(labType, 10);
    setKylinLabelAttributes(labType);

    labColor->setText(tr("Colour"));
    //setFontSize(labColor, 10);
    setKylinLabelAttributes(labColor);

    labResolution->setText(tr("Resolution"));
    //setFontSize(labResolution, 10);
    setKylinLabelAttributes(labResolution);

    labSize->setText(tr("Size"));
    //setFontSize(labSize, 10);
    setKylinLabelAttributes(labSize);

    labFormat->setText(tr("Format"));
    //setFontSize(labFormat, 10);
    setKylinLabelAttributes(labFormat);

    labName->setText(tr("Name"));
    //setFontSize(labName, 10);
    setKylinLabelAttributes(labName);

    labLocation->setText(tr("Scan to"));
    //setFontSize(labLocation, 10);
    setKylinLabelAttributes(labLocation);

    textType->setFixedSize(180, 32);

    textName->setMaxLength (256 - 4);
    textName->setFixedSize(180, 32);

    if (!device_status) {
        // No find scan device
        textType->setText(tr("Device type"));

        textDevice->colorInit();
        if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
            textName->setStyleSheet("QLineEdit{border:1px solid #0D0400;background-color:rgb(15,08,01);color:gray;border-radius:4px;}");
            textType->setStyleSheet("QLabel{border:1px solid #0D0400;background-color:rgb(15,08,01);color:gray;border-radius:4px;}");
            btnLocation->setStyleSheet("QPushButton{border:4px solid #0D0400;background-repeat:no-repeat;background-position:right;background-color:#0D0400;color:gray;border-radius:4px;text-align:left;}");
        } else {
            textType->setStyleSheet("QLabel{background-color:#E7E7E7;color:#000000;border-radius:4px;}");
            textName->setStyleSheet("QLineEdit{background-color:#E7E7E7;color:#000000;border-radius:4px;}");
            btnLocation->setStyleSheet("QPushButton{background-color:#E7E7E7;background-repeat:no-repeat;background-position:right;color:gray;border-radius:4px;text-align:left;}");
        }

    } else {
        textType->setText(instance.getKylinSaneType());

        if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
            textType->setStyleSheet("QLabel{border:1px solid #0D0400;background-color:rgb(15,08,01);color:rgb(232,232,232);border-radius:4px;}");
            textName->setStyleSheet("QLineEdit{border:1px solid #0D0400;background-color:rgb(15,08,01);color:rgb(232,232,232);border-radius:4px;}");
#ifdef DEBUG_EDIT
            btnLocation->setStyleSheet("QPushButton{border:4px solid #0D0400;background-repeat:no-repeat;background-position:right;background-color:#0D0400;color:gray;border-radius:4px;text-align:left;}");
#endif
        } else {
            textType->setStyleSheet("QLabel{background-color:#E7E7E7;color:#000000;border-radius:4px;}");
            textName->setStyleSheet("QLineEdit{background-color:#E7E7E7;color:#000000;border-radius:4px;}");
#ifdef DEBUG_EDIT
            btnLocation->setStyleSheet("QPushButton{border:4px solid #E7E7E7;background-color:#E7E7E7;background-repeat:no-repeat;background-position:right;color:gray;border-radius:4px;text-align:left;}");
#endif
        }
    }
}

/**
 * @brief setKylinLabelAttributes 设置标签属性
 * @param label 需要属性设置的标签
 */
void KYCScanSettingsWidget::setKylinLabelAttributes(QLabel *label)
{
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        label->setStyleSheet("color:rgb(232,232,232)");
    } else {
        label->setStyleSheet("color:#000000");
    }
    //label->setFixedSize(68,32);
    label->setFixedHeight(32);
    label->adjustSize();
}


/**
 * @brief setKylinHBoxLayout 统一设置麒麟扫描水平布局HBoxLayout
 */
void KYCScanSettingsWidget::setKylinHBoxLayout()
{
    setKylinHBoxLayoutAttributes(hBoxDevice, labDevice, textDevice);
    hBoxDevice->setContentsMargins(0, 24, 0, 4);

    setKylinHBoxLayoutAttributes(hBoxType, labType, textType);
    hBoxType->setContentsMargins(0, 4, 0, 4);

    setKylinHBoxLayoutAttributes(hBoxColor, labColor, textColor);
    hBoxColor->setContentsMargins(0, 4, 0, 4);

    setKylinHBoxLayoutAttributes(hBoxResolution, labResolution, textResolution);
    hBoxResolution->setContentsMargins(0, 4, 0, 4);

    setKylinHBoxLayoutAttributes(hBoxSize, labSize, textSize);
    hBoxSize->setContentsMargins(0, 4, 0, 16);

    setKylinHBoxLayoutAttributes(hBoxFormat, labFormat, textFormat);
    hBoxFormat->setContentsMargins(0, 16, 0, 4);

    hBoxName->setSpacing(0);
    hBoxName->addStretch();
    hBoxName->addWidget(labName);
    hBoxName->addSpacing(8);
    hBoxName->addWidget(textName);
    hBoxName->addSpacing(16);
//    setKylinHBoxLayoutAttributes(hBoxName, labName, textName);
    hBoxName->setContentsMargins(0, 4, 0, 4);

    hBoxLocation->setSpacing(0);
    hBoxLocation->addStretch();
    hBoxLocation->addWidget(labLocation);
    hBoxLocation->addSpacing(8);
    hBoxLocation->addWidget(btnLocation);
    hBoxLocation->addSpacing(16);
    hBoxLocation->setContentsMargins(0, 4, 0, 4);

    hBoxLine3->setSpacing(0);
    hBoxLine3->addWidget(line3);
    hBoxLine3->setContentsMargins(0, 0, 0, 0);

    hBoxLine4->setSpacing(0);
    hBoxLine4->addWidget(line4);
    hBoxLine4->setContentsMargins(0, 112, 0, 0);

    hBoxMailText->setSpacing(0);
    hBoxMailText->addWidget(btnMail);
    hBoxMailText->addSpacing(10);
    hBoxMailText->addWidget(btnSave);
    hBoxMailText->setContentsMargins(0, 20, 0, 20);

}

/**
 * @brief setKylinHBoxLayoutAttributes 设置水平布局属性： 标签1和标签2
 * @param layout 需要设置的水平布局
 * @param labelFirst 水平布局中的第一个标签
 * @param labelSecond 水平布局中的第二个标签
 */
void KYCScanSettingsWidget::setKylinHBoxLayoutAttributes(QHBoxLayout *layout, QLabel *labelFirst,
                                           QLabel *labelSecond)
{
    layout->setSpacing(0);
    layout->addStretch();
    layout->addWidget(labelFirst);
    layout->addSpacing(8);
    layout->addWidget(labelSecond);
    layout->addSpacing(16);
}


/**
 * @brief setKylinHBoxLayoutAttributes 重载设置水平布局属性： 标签和组合框
 * @param layout 需要设置的水平布局
 * @param labelFirst 水平布局中的标签
 * @param combo 水平布局中的组合框
 */
void KYCScanSettingsWidget::setKylinHBoxLayoutAttributes(QHBoxLayout *layout, QLabel *labelFirst, KYCComboBox *combo)
{
    layout->setSpacing(0);
    layout->addStretch();
    layout->addWidget(labelFirst);
    layout->addSpacing(8);
    layout->addWidget(combo);
    layout->addSpacing(16);
}

QString KYCScanSettingsWidget::getTextResolution()
{
    return textResolution->currentText();
}

QString KYCScanSettingsWidget::getTextSize()
{
    return textSize->currentText();
}

QString KYCScanSettingsWidget::getTextFormat()
{
    return textFormat->currentText();
}

QString KYCScanSettingsWidget::getTextName()
{
    return textName->text();
}

QString KYCScanSettingsWidget::getTextLocation()
{
    return curPath;
}

void KYCScanSettingsWidget::warnMsg(QString msg)
{
#if 0
    QWidget *parent = QApplication::activeWindow();
    if (!parent)
        parent = QApplication::activeWindow();
#endif
    QMessageBox *msgBox = new QMessageBox(); // Without this pointer

    msgBox->setText(msg);
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setWindowIcon(QIcon::fromTheme("kylin-scanner")); // This not work
    msgBox->setWindowTitle(tr("Scanner")); // This not work
    msgBox->setStandardButtons(QMessageBox::Yes); // Add buttons by `|`
    msgBox->setContextMenuPolicy(Qt::NoContextMenu);
    msgBox->button(QMessageBox::Yes)->setText(tr("Yes")); // set buttonText

    // center saveDialog in mainwindow
    QWidget *widget = nullptr;
    QWidgetList widgetList = QApplication::allWidgets();
    for (int i=0; i<widgetList.length(); ++i) {
        if (widgetList.at(i)->objectName() == "MainWindow") {
            widget = widgetList.at(i);
        }
    }
    if (widget) {
        msgBox->setParent(widget); // this is important
        QRect rect = widget->geometry();
        int x = rect.x() + rect.width()/2 - msgBox->width()/2;
        int y = rect.y() + rect.height()/2 - msgBox->height()/2;
        qDebug() << "x = " << x << "y = " << y;
        msgBox->move(x,y);
    }

    int result = msgBox->exec();
    qDebug() << "result = " << result;
}

void KYCScanSettingsWidget::setFontSize(QLabel *label, int n)
{
    QFont ft;
    ft.setPointSize(n);
    label->setFont(ft);
}

/**
 * @brief KYCScanSettingsWidget::setSendMailButtonStyle
 * Recover btnMail style while pressed it after first scanning
 */
void KYCScanSettingsWidget::setSendMailButtonStyle()
{
    qDebug() << "simple btnMail style.";
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        btnMail->setStyleSheet("QPushButton{background-color:rgb(32,30,29);border:1px solid #939393;color:rgb(232,232,232);border-radius:4px;}");
    } else {
        btnMail->setStyleSheet("QPushButton{background-color:#F9F9F9;border:1px solid #939393;color:#000000;border-radius:4px;}");
    }
}

void KYCScanSettingsWidget::setSendMailButtonStyleNormal()
{
    qDebug() << "recover btnMail style.";
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        btnMail->setStyleSheet("QPushButton{background-color:rgb(32,30,29);border:1px solid #939393;color:rgb(232,232,232);border-radius:4px;}"
                               "QPushButton:hover{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}"
                               "QPushButton:checked{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}");
    } else {
        btnMail->setStyleSheet("QPushButton{background-color:#F9F9F9;border:1px solid #939393;color:#000000;border-radius:4px;}"
                               "QPushButton:hover{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:#000000;border-radius:4px;}"
                               "QPushButton:checked{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:#000000;border-radius:4px;}");
    }
}

void KYCScanSettingsWidget::setTextNameToolTip()
{
    int lenTextName = textName->text ().length ();
    qDebug() << "lenTextName = " << lenTextName;

    // 设置名字最大长度为256,但由于后缀名(.jpg)有4位长度，否则在文件夹中显示为隐藏文件
    if (lenTextName >= 252)
        textName->setToolTip (tr("Scanning images's length cannot be large than 252"));
    else
        textName->setToolTip ("");
}

void KYCScanSettingsWidget::setBtnSaveText()
{
    btnSave->setText(tr("Save as"));
}

void KYCScanSettingsWidget::setFlagTextDeviceChangedWork()
{
    // onTextDeviceCurrentTextChanged will work
    flagTextDeviceChangedWork = 1;
}

void KYCScanSettingsWidget::onBtnLocationClicked()
{
    if (curPath.isEmpty())
        curPath = QDir::homePath() ; //获取家目录的路径

    QString midPath = curPath;
    QString dlgTitle = tr("Select a directory"); //对话框标题
    QString selectedDir = QFileDialog::getExistingDirectory(this, dlgTitle, curPath,
                                                            QFileDialog::ShowDirsOnly);
    qDebug() << "selected directory: " << selectedDir;

    if (!selectedDir.isEmpty()) {
        QFileInfo file(selectedDir);
        if (file.permission(QFileDevice::WriteUser | QFileDevice::ReadGroup)) {
            qDebug() << "The user could read and write " << selectedDir;

            QFontMetrics elideFont(btnLocation->font());
            curPath = selectedDir;
            btnLocation->setText(elideFont.elidedText(selectedDir, Qt::ElideRight, 150));
        } else {
            qDebug() << "The user can't read and write " << selectedDir;

            QString msg = tr("Currently user has no permission to modify directory ") + selectedDir;
            warnMsg(msg);
        }
    }
}

void KYCScanSettingsWidget::onBtnMailClicked()
{
    setSendMailButtonStyle();

    AppList *maillist = getAppIdList(MAILTYPE);
    qDebug() << "Get Applist success.";

    if (!maillist) {
        qDebug() << "maillist is null";
        KYCNoMailDialog *dialog = new KYCNoMailDialog(this);
        int ret = dialog->exec(); // 以模态方式显示对话框，用户关闭对话框时返回 DialogCode值
        qDebug() << "ret = " << ret;
        if (ret == QDialog::Accepted) {
            QProcess *process = new QProcess();
            process->start("/usr/bin/kylin-software-center");
        }
        delete dialog; //删除对话框
    } else {
        qDebug() << "maillist is not null";
        KYCSendMailDialog *dialog = new KYCSendMailDialog(this);
        qDebug() << "begin";
        dialog->setBtnList();
        qDebug() << "after";
        dialog->exec();
    }

    emit sendMailSignal();
}

void KYCScanSettingsWidget::onBtnSaveClicked()
{
    QString filename;
    QString filepath;
    QString filetype;
    QString msg;
    QString titlename;
    bool flagSave = false;

    while (!flagSave) {
        if (flag == 1) { // 进行OCR ，存储文本
            if (textName->text().endsWith(".txt")) {
                filename = textName->text();
            } else {
                filename = textName->text() + ".txt";
            }
            titlename = tr("Store text dialog");
        } else {
            if (textName->text().endsWith(".jpg")
                    || textName->text().endsWith(".png")
                    || textName->text().endsWith(".bmp")
                    || textName->text().endsWith(".pdf")) {
                filename = textName->text();
            } else {
                filename = textName->text() + "." + textFormat->currentText();
            }
            titlename = tr("Save as dialog");
        }
        filepath = curPath;

        KYCSaveFileDialog *saveDialog = new KYCSaveFileDialog(this->parentWidget(), flag, filename, titlename);

        // center saveDialog in mainwindow
        QWidget *widget = nullptr;
        QWidgetList widgetList = QApplication::allWidgets();
        for (int i=0; i<widgetList.length(); ++i) {
            if (widgetList.at(i)->objectName() == "MainWindow") {
                widget = widgetList.at(i);
            }
        }
        if (widget) {
            QRect rect = widget->geometry();
            int x = rect.x() + rect.width()/2 - saveDialog->width()/2;
            int y = rect.y() + rect.height()/2 - saveDialog->height()/2;
            qDebug() << "x = " << x << "y = " << y;
            saveDialog->move(x,y);
        }

        saveDialog->kycSetDirectory(filepath);

        if (saveDialog->exec() == QFileDialog::Accepted) {
            filepath = saveDialog->selectedFiles().at(0);
            filename = saveDialog->getFileName();
            qDebug() << "filepath = " << filepath
                     << "filename = " << filename;

            if (filepath.isNull())
                break;

            if (filename.contains(QChar('/'), Qt::CaseInsensitive)) {
                msg = tr("cannot contain '/' character.");
                warnMsg(msg);
                continue;
            }
            if (filename.startsWith(QChar('.'), Qt::CaseInsensitive)) {
                msg = tr("cannot save as hidden file.");
                warnMsg(msg);
                continue;
            }

            // While not endsWith (".jpg" | ".png" | "pdf" | "bmp"),
            // we neet add suffix behind.
            if (flag == 1) {
                if (!filename.endsWith(".txt", Qt::CaseInsensitive)) {
                    // 进行OCR ，存储文本
                    filetype = saveDialog->getFileType();
                    filepath = filepath.append(filetype);
                }
            } else {
                if (!filename.endsWith(".jpg", Qt::CaseInsensitive)
                        && !filename.endsWith(".png", Qt::CaseInsensitive)
                        && !filename.endsWith(".pdf", Qt::CaseInsensitive)
                        && !filename.endsWith(".bmp", Qt::CaseInsensitive)) {

                    filetype = saveDialog->getFileType();
                    filepath = filepath.append(filetype);
                }
            }
            flagSave = true;
        } else {
            flagSave = false;
            return;
        }
    }

    qDebug() << "filepath = " << filepath;

    if (flagSave)
        emit saveImageSignal(filepath);
}

void KYCScanSettingsWidget::onTextDeviceCurrentTextChanged(QString device)
{
    if (0 == flagTextDeviceChangedWork) {
        qDebug() << "Device changed before openSaneDevice, so this function will not work.";
        return;
    }
    qDebug() << "Device changed, therefore, we need open device again.";
    bool status = true;

    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    instance.userInfo.name = device;
    int curTextLen = textDevice->currentText().length();

    qDebug() << "device name: " << instance.userInfo.name;
    qDebug() << "textDevice->currentText = " << textDevice->currentText()
             << "length = " << curTextLen;

    if ( curTextLen >= 20) {
        textDevice->setToolTip(textDevice->currentText());
    } else {
        textDevice->setToolTip("");
    }

    // index是根据所选的进行判断,或者open_device直接根据所选进行判断
    int index = textDevice->currentIndex();
    if (index == -1)
        index = 0;

    qDebug() << "device index: " << index;
    //char *deviceName =

    //int index = 1;
    instance.openScanDevice(index);

    status = instance.getKylinSaneStatus();
    if (status) {
        qDebug() << "open_device true";
        scanOpenFlag = 1;
        emit openDeviceStatusSignal(true);
    } else {
        qDebug() << "open_device false";
        scanOpenFlag = 0;
        emit openDeviceStatusSignal(false);
    }
}

void KYCScanSettingsWidget::modifyBtnSave()
{
    if (flag == 0) {
        // 进行OCR，存储文本
        flag = 1;
        qDebug() << "btnSave text: " << btnSave;
        btnSave->setText(tr("Store text"));
    } else { // 另存为
        flag = 0;
        qDebug() << "btnSave text: " << btnSave;
        btnSave->setText(tr("Save as"));
    }
}

void KYCScanSettingsWidget::setOcrFlagInit()
{
    // Init btnSave text to save as
    flag = 0;
}

void KYCScanSettingsWidget::onTextColorCurrentTextChanged(QString color)
{
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();

    // Do not direct to return color, because color has been tr()
    if ( !QString::compare("黑白", color) || !QString::compare("Lineart", color) ) {
        instance.userInfo.color = "Lineart";
    } else if ( !QString::compare("彩色", color) || !QString::compare("Color", color) ) {
        instance.userInfo.color = "Color";
    } else {
        instance.userInfo.color = "Gray";
    }
    qDebug() << "color: " << instance.userInfo.color;
}

void KYCScanSettingsWidget::onTextResolutionCurrentTextChanged(QString resolution)
{
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    QString curResolution = textResolution->currentText();

    if (instance.getKylinSaneStatus()
            && (curResolution == "2400" || curResolution == "4800")) {
        qDebug() << "curResolution = " << curResolution;

        QString msg;
        msg = tr("This resolution will take a loog time to scan, please choose carelly.");
        warnMsg(msg);
    }
    instance.userInfo.resolution = resolution;
    qDebug() << "resolution: " << instance.userInfo.resolution;
}

void KYCScanSettingsWidget::onTextSizeCurrentTextChanged(QString size)
{
    KYCSaneWidget &instance = KYCSaneWidget::getInstance();
    instance.userInfo.size = size;
    qDebug() << "size: " << instance.userInfo.size;
}

void KYCScanSettingsWidget::onTextNameTextChanged(QString)
{
    QString msg;
    if (textName->text().contains(QChar('/'), Qt::CaseInsensitive)) {
        msg = tr("cannot contain '/' character.");
        warnMsg(msg);
        textName->cursorBackward(true, 1); // 向左移动一个字符的长度
        textName->del(); // 删除光标右侧字符或选中文本
    }
    if (textName->text().startsWith(QChar('.'), Qt::CaseInsensitive)) {
        msg = tr("cannot save as hidden file.");
        warnMsg(msg);
        textName->cursorBackward(true, 1);
        textName->del();
    }

    setTextNameToolTip ();
}

void KYCScanSettingsWidget::scanset_style_changed(QString)
{
    setKylinLable();
    if (stylelist.contains(style_settings->get(STYLE_NAME).toString())) {
        // 背景颜色
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(32, 30, 29));
        setAutoFillBackground(true);
        setPalette(pal);

        btnMail->setStyleSheet("QPushButton{background-color:rgb(32,30,29);border:1px solid #939393;color:rgb(232,232,232);border-radius:4px;}"
                               "QPushButton:hover{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}"
                               "QPushButton:checked{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:rgb(232,232,232);border-radius:4px;}");
        btnSave->setStyleSheet("QPushButton{background-color:rgb(32,30,29);border:1px solid #939393;color:rgb(232,232,232);border-radius:4px;}"
                               "QPushButton:hover{border:none;background-color:#3D6BE5;color:rgb(232,232,232);border-radius:4px;}"
                               "QPushButton:checked{border:none;background-color:#3D6BE5;color:rgb(232,232,232)border-radius:4px;}");
        btnLocation->setStyleSheet("QPushButton{border:4px solid #0D0400;background-repeat:no-repeat;background-position:right;background-color:#0D0400;color:rgb(232,232,232);border-radius:4px;text-align:left;}");
        line3->setStyleSheet("QFrame{color:#201E1D}");
        line4->setStyleSheet("QFrame{color:#201E1D}");
    } else {
        QPalette pal(palette());
        pal.setColor(QPalette::Background, QColor(249, 249, 249));
        setAutoFillBackground(true);
        setPalette(pal);

        btnMail->setStyleSheet("QPushButton{background-color:#F9F9F9;border:1px solid #939393;color:#000000;border-radius:4px;}"
                               "QPushButton:hover{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:#000000;border-radius:4px;}"
                               "QPushButton:checked{border:none;background-color:#3D6BE5;border:rgb(147,147,147);color:#000000;border-radius:4px;}");
        btnSave->setStyleSheet("QPushButton{background-color:#F9F9F9;border:1px solid #939393;color:#000000;border-radius:4px;}"
                               "QPushButton:hover{border:none;background-color:#3D6BE5;color:#000000;border-radius:4px;}"
                               "QPushButton:checked{border:none;background-color:#3D6BE5;color:#000000;border-radius:4px;}");
        btnLocation->setStyleSheet("QPushButton{background-repeat:no-repeat;background-position:right;background-color:#E7E7E7;color:#000000;border-radius:4px;text-align:left;}");
        line3->setStyleSheet("QFrame{color:#DCDCDC}");
        line4->setStyleSheet("QFrame{color:#DCDCDC}");
    }
}
