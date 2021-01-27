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

#include "scanSet.h"
#include "kycsavefiledialog.h"
#include <QMessageBox>
#include <QTextCursor>

ScanSet::ScanSet(QWidget *parent)
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
    , textDevice (new KylinCmb())
    , textType (new QLabel())
    , textColor (new KylinCmb())
    , textResolution (new KylinCmb())
    , textSize (new KylinCmb())
    , textFormat (new KylinCmb())
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
    setFixedWidth(268);

    stylelist << STYLE_NAME_KEY_DARK << STYLE_NAME_KEY_BLACK;
    iconthemelist << ICON_THEME_KEY_BASIC << ICON_THEME_KEY_CLASSICAL << ICON_THEME_KEY_DEFAULT;

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
    btnMail->setFixedSize(120,32);
    btnSave->setText(tr("Save as"));
    btnSave->setFixedSize(100,32);

    QFontMetrics elideFont(btnLocation->font());
    if (curPath.isEmpty())
        curPath=QDir::homePath() ; //获取家目录的路径
    btnLocation->setText(elideFont.elidedText(curPath,Qt::ElideRight,150));
    btnLocation->setFixedSize(180,32);

    btnLocation->setIcon(QIcon::fromTheme("folder-open"));
    btnLocation->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

    setKylinLable();
    setKylinComboBox(false);
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
    setKylinHBoxLayout();

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
    vBoxScanSet->setContentsMargins(0,0,0,0);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setLayout(vBoxScanSet);

    // For current combobox text, while not change current text
    KylinSane & instance = KylinSane::getInstance();
    QString curDeviceName, curSize, curColor, curResolution;

    curDeviceName = textDevice->currentText();
    instance.userInfo.name = curDeviceName;

    curSize = textSize->currentText();
    instance.userInfo.size = curSize;

    curColor = textColor->currentText();
    // Do not direct to return color, because color has been tr()
    if (! QString::compare("黑白", curColor)
            || ! QString::compare("Lineart", curColor))
    {
        instance.userInfo.color = "Lineart";
    }
    else if (! QString::compare("彩色", curColor)
            || ! QString::compare("Color", curColor))
    {
       instance.userInfo.color = "Color";
    }
    else if (! QString::compare("灰度", curColor)
            || ! QString::compare("Gray", curColor))
    {
        instance.userInfo.color = "Gray";
    }
    qDebug() << "userInfo.color = " << instance.userInfo.color;
    instance.userInfo.color = curColor;

    curResolution = textResolution->currentText();
    instance.userInfo.resolution = curResolution;

    // For save location
    connect(btnLocation,SIGNAL(clicked()),this,SLOT(onBtnLocationClicked()));

    // For send email
    connect(btnMail,SIGNAL(clicked()),this,SLOT(onBtnMailClicked()));

    // For save file
    connect(btnSave,SIGNAL(clicked()),this,SLOT(onBtnSaveClicked()));

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

    connect(style_settings,SIGNAL(changed(QString)),this,
            SLOT(scanset_style_changed(QString)));
}

ScanSet::~ScanSet()
{

}

/**
 * @brief setKylinComboBoxAttributes 设置组合框属性
 * @param combo 需要属性设置的组合框
 * @param strList 组合框中文本框值
 */
void ScanSet::setKylinComboBoxAttributes(KylinCmb *combo, QStringList strList)
{
    QListView *listView = new QListView(this);

    combo->clear();
    combo->addItems(strList);
    combo->setFixedSize(180,32);
    combo->setInsertPolicy(QComboBox::NoInsert);  //编辑框的内容不插入到列表项
    combo->setFocusPolicy(Qt::NoFocus); //获取焦点策略：无焦点，也就是不可编辑
    combo->setModel(listView->model()); // avoid warning
    combo->setView(listView);   //使下拉选项样式生效
}
 
/**
 * @brief setKylinComboBox 统一设置麒麟扫描组合框ComboBox
 */
void ScanSet::setKylinComboBox(bool curIndexChanged)
{
    QStringList strListDevice, strListColor, strListResolution, strListFormat, strListSize,strListLocation;
    KylinSane& instance = KylinSane::getInstance();
    bool device_status = true;
    int defaultResolution = 0;
    int defaultSize = 0;

    device_status = instance.getKylinSaneStatus();

    if (!device_status)
    {
        // If not find scan device
        strListDevice << tr("No available device");
        setKylinComboBoxAttributes(textDevice, strListDevice);

        strListColor<<tr("Lineart")<<tr("Color")<<tr("Gray");
        setKylinComboBoxAttributes(textColor, strListColor);

        strListResolution << "4800" << "2400" << "1200" << "600" << "300" << tr("Auto");
        for (int i=0; i<strListResolution.size(); i++)
        {
           if (!QString::compare("300", strListResolution[i], Qt::CaseSensitive))
           {
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
    if (!curIndexChanged) // 当选择设备时，索引发生改变，此时不应该按照读取的字符串进行默认设置
    {
        strListDevice = instance.getKylinSaneNames();
        qDebug() << "sane names: " << strListDevice;
        setKylinComboBoxAttributes(textDevice, strListDevice);
    }

    // For  default color
    strListColor = instance.getKylinSaneColors();
    int defaultColor = 0;

    for(int i=0; i<strListColor.size(); i++)
    {
       if (! QString::compare("黑白", strListColor[i], Qt::CaseSensitive)
               || ! QString::compare("Lineart", strListColor[i], Qt::CaseSensitive))
       {
           defaultColor = i;
           break;
       }
    }

    setKylinComboBoxAttributes(textColor, strListColor);
    textColor->setCurrentIndex(defaultColor);

    // For  default resolution
    strListResolution = instance.getKylinSaneResolutions();

    for(int i=0; i<strListResolution.size(); i++)
    {
       if (! QString::compare("300", strListResolution[i], Qt::CaseSensitive))
       {
           defaultResolution = i;
           break;
       }
    }

    setKylinComboBoxAttributes(textResolution, strListResolution);
    textResolution->setCurrentIndex(defaultResolution);

    // For  default sizes
    strListSize = instance.getKylinSaneSizes();

    for(int i=0; i<strListSize.size(); i++)
    {
       if (! QString::compare("A4", strListSize[i], Qt::CaseSensitive))
       {
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
 */
void ScanSet::setKylinComboBoxScanDeviceName()
{
    QStringList strListDevice;
    KylinSane& instance = KylinSane::getInstance();
    bool device_status = true;

    device_status = instance.getKylinSaneStatus();

    if (!device_status)
    {
        // If not find scan device
        strListDevice << tr("No available device");
        setKylinComboBoxAttributes(textDevice, strListDevice);

        return;
    }

    // For  default device
    strListDevice = instance.getKylinSaneNames();
    qDebug() << "sane names: " << strListDevice;
    setKylinComboBoxAttributes(textDevice, strListDevice);
}

void ScanSet::setKylinScanSetNotEnable()
{
    KylinSane& instance = KylinSane::getInstance();
    bool device_status = true;

    device_status = instance.getKylinSaneStatus();

    if (!device_status)
    {
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
void ScanSet::setKylinScanSetBtnNotEnable()
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

void ScanSet::setKylinScanSetBtnEnable()
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

void ScanSet::setKylinScanSetEnable()
{
    KylinSane& instance = KylinSane::getInstance();
    bool device_status = true;

    device_status = instance.getKylinSaneStatus();

    if (device_status)
    {
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
 */
void ScanSet::setKylinLable()
{
    KylinSane& instance = KylinSane::getInstance();
    bool device_status = true;

    device_status = instance.getKylinSaneStatus();

    labDevice->setText(tr("Device"));
    setFontSize(labDevice,10);
    setKylinLabelAttributes(labDevice);

    labType->setText(tr("Type"));
    setFontSize(labType,10);
    setKylinLabelAttributes(labType);

    labColor->setText(tr("Colour"));
    setFontSize(labColor,10);
    setKylinLabelAttributes(labColor);

    labResolution->setText(tr("Resolution"));
    setFontSize(labResolution,10);
    setKylinLabelAttributes(labResolution);

    labSize->setText(tr("Size"));
    setFontSize(labSize,10);
    setKylinLabelAttributes(labSize);

    labFormat->setText(tr("Format"));
    setFontSize(labFormat,10);
    setKylinLabelAttributes(labFormat);

    labName->setText(tr("Name"));
    setFontSize(labName,10);
    setKylinLabelAttributes(labName);

    labLocation->setText(tr("Scan to"));
    setFontSize(labLocation,10);
    setKylinLabelAttributes(labLocation);

    textType->setFixedSize(180,32);
    textName->setText("scanner01");

    textName->setMaxLength (256-4);
    textName->setFixedSize(180,32);

    if (!device_status)
    {
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
        } else {
            textType->setStyleSheet("QLabel{background-color:#E7E7E7;color:#000000;border-radius:4px;}");
            textName->setStyleSheet("QLineEdit{background-color:#E7E7E7;color:#000000;border-radius:4px;}");
        }
    }
}

/**
 * @brief setKylinLabelAttributes 设置标签属性
 * @param label 需要属性设置的标签
 */
void ScanSet::setKylinLabelAttributes(QLabel *label)
{
    label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
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
void ScanSet::setKylinHBoxLayout()
{
    setKylinHBoxLayoutAttributes(hBoxDevice, labDevice, textDevice);
    hBoxDevice->setContentsMargins(0,24,0,4);

    setKylinHBoxLayoutAttributes(hBoxType, labType, textType);
    hBoxType->setContentsMargins(0,4,0,4);

    setKylinHBoxLayoutAttributes(hBoxColor, labColor, textColor);
    hBoxColor->setContentsMargins(0,4,0,4);

    setKylinHBoxLayoutAttributes(hBoxResolution, labResolution, textResolution);
    hBoxResolution->setContentsMargins(0,4,0,4);

    setKylinHBoxLayoutAttributes(hBoxSize, labSize, textSize);
    hBoxSize->setContentsMargins(0,4,0,16);

    setKylinHBoxLayoutAttributes(hBoxFormat, labFormat, textFormat);
    hBoxFormat->setContentsMargins(0,16,0,4);

    hBoxName->setSpacing(0);
    hBoxName->addStretch();
    hBoxName->addWidget(labName);
    hBoxName->addSpacing(8);
    hBoxName->addWidget(textName);
    hBoxName->addSpacing(16);
//    setKylinHBoxLayoutAttributes(hBoxName, labName, textName);
    hBoxName->setContentsMargins(0,4,0,4);

    hBoxLocation->setSpacing(0);
    hBoxLocation->addStretch();
    hBoxLocation->addWidget(labLocation);
    hBoxLocation->addSpacing(8);
    hBoxLocation->addWidget(btnLocation);
    hBoxLocation->addSpacing(16);
    hBoxLocation->setContentsMargins(0,4,0,4);

    hBoxLine3->setSpacing(0);
    hBoxLine3->addWidget(line3);
    hBoxLine3->setContentsMargins(0,0,0,0);

    hBoxLine4->setSpacing(0);
    hBoxLine4->addWidget(line4);
    hBoxLine4->setContentsMargins(0,112,0,0);

    hBoxMailText->setSpacing(0);
    hBoxMailText->addWidget(btnMail);
    hBoxMailText->addSpacing(10);
    hBoxMailText->addWidget(btnSave);
    hBoxMailText->setContentsMargins(0,20,0,20);

}

/**
 * @brief setKylinHBoxLayoutAttributes 设置水平布局属性： 标签1和标签2
 * @param layout 需要设置的水平布局
 * @param labelFirst 水平布局中的第一个标签
 * @param labelSecond 水平布局中的第二个标签
 */
void ScanSet::setKylinHBoxLayoutAttributes(QHBoxLayout *layout, QLabel *labelFirst, QLabel *labelSecond)
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
void ScanSet::setKylinHBoxLayoutAttributes(QHBoxLayout *layout, QLabel *labelFirst, KylinCmb *combo)
{
    layout->setSpacing(0);
    layout->addStretch();
    layout->addWidget(labelFirst);
    layout->addSpacing(8);
    layout->addWidget(combo);
    layout->addSpacing(16);
}

QString ScanSet::getTextResolution()
{
    return textResolution->currentText();
}

QString ScanSet::getTextSize()
{
    return textSize->currentText();
}

QString ScanSet::getTextFormat()
{
    return textFormat->currentText();
}

QString ScanSet::getTextName()
{
    return textName->text();
}

QString ScanSet::getTextLocation()
{
    return curPath;
}

void ScanSet::warnMsg(QString msg)
{
    QMessageBox msgBox(QMessageBox::Warning, QObject::tr("warning"), msg);
    msgBox.setWindowIcon(QIcon::fromTheme("kylin-scanner"));
    msgBox.exec();
}

void ScanSet::setFontSize(QLabel *label, int n)
{
    QFont ft;
    ft.setPointSize(n);
    label->setFont(ft);
}

void ScanSet::setTextNameToolTip()
{
    int lenTextName = textName->text ().length ();
    qDebug() << "lenTextName = " << lenTextName;

    // 设置名字最大长度为256,但由于后缀名(.jpg)有4位长度，否则在文件夹中显示为隐藏文件
    if (lenTextName >= 252)
        textName->setToolTip (tr("Scanning images's length cannot be large than 252"));
    else
        textName->setToolTip ("");
}

void ScanSet::setBtnSaveText()
{
    btnSave->setText(tr("Save as"));
}

void ScanSet::onBtnLocationClicked()
{
    if (curPath.isEmpty())
        curPath=QDir::homePath() ; //获取家目录的路径

    QString dlgTitle=tr("Select a directory"); //对话框标题
    QString selectedDir=QFileDialog::getExistingDirectory(this,dlgTitle,curPath,QFileDialog::ShowDirsOnly);
    if (!selectedDir.isEmpty())
    {
        QFontMetrics elideFont(btnLocation->font());
        curPath = selectedDir;
        btnLocation->setText(elideFont.elidedText(selectedDir,Qt::ElideRight,150));
    }
}

void ScanSet::onBtnMailClicked()
{
    AppList * maillist = getAppIdList(MAILTYPE);
    qDebug() << "Get Applist success.";

    if (!maillist)
    {
        qDebug() << "maillist is null";
        NoMail *dialog = new NoMail(this);
        int ret= dialog->exec();// 以模态方式显示对话框，用户关闭对话框时返回 DialogCode值
        qDebug() << "ret = " << ret;
        if (ret==QDialog::Accepted)
        {
            QProcess *process = new QProcess();
            process->start("/usr/bin/ubuntu-kylin-software-center");
        }
        delete dialog; //删除对话框
    }
    else
    {
        qDebug() << "maillist is not null";
        SendMail *dialog = new SendMail(this);
        qDebug() << "begin";
        dialog->setBtnList();
        qDebug() << "after";
        dialog->exec();
    }
}

void ScanSet::onBtnSaveClicked()
{
    QString filename;
    QString filepath;
    QString filetype;
    QString msg;
    QString titlename;
    bool flagSave = false;

    while (!flagSave) {
        if (flag == 1) { // 进行OCR ，存储文本
            if (textName->text().endsWith(".txt"))
                filename = textName->text();
            else
                filename = textName->text() + "." + ".txt";
            titlename = tr("Store text dialog");
        } else {
            if (textName->text().endsWith(".jpg")
                    || textName->text().endsWith(".png")
                    || textName->text().endsWith(".bmp")
                    || textName->text().endsWith(".pdf"))
                filename = textName->text();
            else
                filename = textName->text() + "." + textFormat->currentText();
            titlename = tr("Save as dialog");
        }
        filepath = curPath;

        KYCSaveFileDialog *saveDialog = new KYCSaveFileDialog(this, flag, filename, titlename);
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

            if (!filename.endsWith(".jpg", Qt::CaseInsensitive)
                && !filename.endsWith("*.png", Qt::CaseInsensitive)
                && !filename.endsWith("*.pdf", Qt::CaseInsensitive)
                && !filename.endsWith("*.bmp", Qt::CaseInsensitive)) {

                filetype = saveDialog->getFileType();
                filepath = filepath.append(filetype);
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

void ScanSet::onTextDeviceCurrentTextChanged(QString device)
{
    bool status = true;

    KylinSane & instance = KylinSane::getInstance();
    instance.userInfo.name = device;
    int curTextLen = textDevice->currentText().length();

    qDebug() << "device name: "<< instance.userInfo.name;
    qDebug() << "textDevice->currentText = " << textDevice->currentText()
             << "length = " << curTextLen;

    if ( curTextLen >= 20)
    {
        textDevice->setToolTip(textDevice->currentText());
    }
    else
    {
        textDevice->setToolTip("");
    }
    int index = textDevice->currentIndex(); // index是根据所选的进行判断,或者open_device直接根据所选进行判断
    if (index == -1)
    {
        index = 0;
    }
    qDebug() << "device index: " << index;
    //char *deviceName =

    //int index = 1;
    instance.openScanDevice(index);

    status = instance.getKylinSaneStatus();
    if (status)
    {
        qDebug() << "open_device true";
        scanFlag = 1;
        emit openDeviceStatusSignal(true);
    }
    else
    {
        qDebug() << "open_device false";
        scanFlag = 0;
        emit openDeviceStatusSignal(false);
    }
}

void ScanSet::modifyBtnSave()
{
    if (flag == 0) {
        // 进行OCR，存储文本
        flag = 1;
        btnSave->setText(tr("Store text"));
    } else { // 另存为
        flag = 0;
        btnSave->setText(tr("Save as"));
    }
}

void ScanSet::onTextColorCurrentTextChanged(QString color)
{
    KylinSane & instance = KylinSane::getInstance();

    // Do not direct to return color, because color has been tr()
    if ( !QString::compare("黑白", color) || !QString::compare("Lineart", color) )
    {
        instance.userInfo.color = "Lineart";
    }
    else if ( !QString::compare("彩色", color) || !QString::compare("Lineart", color) )
    {
       instance.userInfo.color = "Color";
    }
    else
    {
        instance.userInfo.color = "Gray";
    }
    qDebug() << "color: "<< instance.userInfo.color;
}

void ScanSet::onTextResolutionCurrentTextChanged(QString resolution)
{
    KylinSane & instance = KylinSane::getInstance();
    instance.userInfo.resolution = resolution;
    qDebug() << "resolution: "<< instance.userInfo.resolution;
}

void ScanSet::onTextSizeCurrentTextChanged(QString size)
{
    KylinSane & instance = KylinSane::getInstance();
    instance.userInfo.size = size;
    qDebug() << "size: "<< instance.userInfo.size;
}

void ScanSet::onTextNameTextChanged(QString)
{
    QString msg;
    if (textName->text().contains(QChar('/'), Qt::CaseInsensitive)) {
        msg = tr("cannot contain '/' character.");
        warnMsg(msg);
        textName->cursorBackward(true,1); // 向左移动一个字符的长度
        textName->del(); // 删除光标右侧字符或选中文本
    }
    if (textName->text().startsWith(QChar('.'), Qt::CaseInsensitive)) {
        msg = tr("cannot save as hidden file.");
        warnMsg(msg);
        textName->cursorBackward(true,1);
        textName->del();
    }

    setTextNameToolTip ();
}

void ScanSet::scanset_style_changed(QString)
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
