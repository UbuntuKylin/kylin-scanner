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
#include "scan_set.h"
QString curPath;
ScanSet::ScanSet(QWidget *parent)
    : QWidget(parent)
{

    setFixedWidth(260);
  //  setFixedHeight(567);
    labDevice = new QLabel();
    labType = new QLabel();
    labColor = new QLabel();
    labResolution = new QLabel();
    labSize = new QLabel();
    labFormat = new QLabel();
    labName = new QLabel() ;
    labLocation = new QLabel();

    line3 = new QFrame();
    line4 = new QFrame();

    btnMail = new QPushButton();
    btnSave = new QPushButton();
    btnLocation = new QPushButton();

    textDevice = new KylinComboBox();
    textType = new QLabel();
    textColor = new KylinComboBox();
    textResolution = new KylinComboBox();
    textSize = new KylinComboBox();
    textFormat = new KylinComboBox();
    textName = new QLineEdit();

    hBoxDevice = new QHBoxLayout();
    hBoxType = new QHBoxLayout();
    hBoxColor = new QHBoxLayout();
    hBoxResolution = new QHBoxLayout();
    hBoxSize = new QHBoxLayout();
    hBoxFormat = new QHBoxLayout();
    hBoxName = new QHBoxLayout();
    hBoxLocation = new QHBoxLayout();
    hBoxLine3 = new QHBoxLayout();
    hBoxLine4 = new QHBoxLayout();
    hBoxMailText = new QHBoxLayout();

    vBoxScanSet = new QVBoxLayout(this);


    line3 = new QFrame();
    line3->setObjectName(QString::fromUtf8("line3"));
    line3->setMaximumHeight(1);
    line3->setMaximumWidth(230);
    line3->setMinimumWidth(230);
    line3->setFrameShape(QFrame::HLine);
    line3->setStyleSheet("QFrame{color:rgb(32,30,29)}");

    line4 = new QFrame();
    line4->setObjectName(QString::fromUtf8("line4"));
    line4->setMaximumHeight(1);
    line4->setMaximumWidth(230);
    line4->setMinimumWidth(230);
    line4->setFrameStyle(QFrame::HLine);
    line4->setStyleSheet("QFrame{color:rgb(32,30,29)}");

    btnMail->setText(tr("Send email to"));
    btnMail->setFixedSize(120,32);
    btnSave->setText(tr("Save as"));
    btnSave->setFixedSize(100,32);

    QFontMetrics elideFont(btnLocation->font());
    if(curPath.isEmpty())
        curPath=QDir::homePath() ; //获取家目录的路径
    btnLocation->setText(elideFont.elidedText(curPath,Qt::ElideRight,150));
    btnLocation->setFixedSize(180,32);

    btnMail->setStyleSheet("QPushButton{background-color:rgb(32,30,29);border:1px solid #939393;color:rgb(232,232,232);border-radius:16px;}"
                              "QPushButton:hover{border:none;background-color:rgb(39,208,127);border:rgb(147,147,147);color:rgb(232,232,232);border-radius:16px;}"
                                "QPushButton:checked{border:none;background-color:rgb(39,208,127);border:rgb(147,147,147);color:rgb(232,232,232);border-radius:16px;}");
    btnSave->setStyleSheet("QPushButton{background-color:rgb(32,30,29);border:1px solid #939393;color:rgb(232,232,232);border-radius:16px;}"
                              "QPushButton:hover{border:none;background-color:rgb(39,208,127);color:rgb(232,232,232);border-radius:16px;}"
                                "QPushButton:checked{border:none;background-color:rgb(39,208,127);color:rgb(232,232,232)border-radius:16px;}");
    btnLocation->setStyleSheet("QPushButton{border:4px solid #0D0400;background-repeat:no-repeat;background-position:right;background-color:#0D0400;color:rgb(232,232,232);border-radius:6px;text-align:left;}");
    btnLocation->setIcon(QIcon::fromTheme("folder-open"));
    btnLocation->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

    setKylinLable();
    setKylinComboBox(false);
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

    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(32, 30, 29));
    setAutoFillBackground(true);
    setPalette(pal);

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
    if(! QString::compare("黑白", curColor)
            || ! QString::compare("Lineart", curColor))
    {
        instance.userInfo.color = "Lineart";
    }
    else if(! QString::compare("彩色", curColor)
            || ! QString::compare("Color", curColor))
    {
       instance.userInfo.color = "Color";
    }
    else if(! QString::compare("灰度", curColor)
            || ! QString::compare("Gray", curColor))
    {
        instance.userInfo.color = "Gray";
    }
    instance.userInfo.color = curColor;

    curResolution = textResolution->currentText();
    instance.userInfo.resolution = curResolution;

    // For save location
    connect(btnLocation,SIGNAL(clicked()),this,SLOT(on_btnLocation_clicked()));

    // For send email
    connect(btnMail,SIGNAL(clicked()),this,SLOT(on_btnMail_clicked()));

    // For save file
    connect(btnSave,SIGNAL(clicked()),this,SLOT(on_btnSave_clicked()));

    // For device name changed
    connect(textDevice, SIGNAL(currentTextChanged(QString)), this, SLOT(on_textDevice_current_text_changed(QString)));

    // For color mode changed
    connect(textColor, SIGNAL(currentTextChanged(QString)), this, SLOT(on_textColor_current_text_changed(QString)));

    // For resolution changed
    connect(textResolution, SIGNAL(currentTextChanged(QString)), this, SLOT(on_textResolution_current_text_changed(QString)));

    // For size changed
    connect(textSize, SIGNAL(currentTextChanged(QString)), this, SLOT(on_textSize_current_text_changed(QString)));
}

ScanSet::~ScanSet()
{

}

void ScanSet::setKylinComboBoxAttributes(KylinComboBox *combo, QStringList strList)
{
    QListView *listView = new QListView;

    combo->clear();
    combo->addItems(strList);
    combo->setFixedSize(180,32);
    combo->setInsertPolicy(QComboBox::NoInsert);  //编辑框的内容不插入到列表项
    combo->setFocusPolicy(Qt::NoFocus); //获取焦点策略：无焦点，也就是不可编辑
    combo->setModel(listView->model());
    combo->setView(listView);   //使下拉选项样式生效
}
 
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
       if(! QString::compare("黑白", strListColor[i], Qt::CaseSensitive)
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
       if(! QString::compare("300", strListResolution[i], Qt::CaseSensitive))
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
       if(! QString::compare("A4", strListSize[i], Qt::CaseSensitive))
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

    if(!device_status)
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
        textName->setStyleSheet("QLineEdit{border:1px solid #0D0400;background-color:rgb(15,08,01);color:gray;border-radius:4px;}");

        btnLocation->setEnabled(false);
        btnLocation->setStyleSheet("QPushButton{border:4px solid #0D0400;background-repeat:no-repeat;background-position:right;background-color:#0D0400;color:gray;border-radius:4px;text-align:left;}");

        btnMail->setEnabled(false);
        btnSave->setEnabled(false);

        textDevice->setStyleSheet("QLabel{border:1px solid #0D0400;background-color:rgb(15,08,01);color:gray;border-radius:4px;}");
        textType->setStyleSheet("QLabel{border:1px solid #0D0400;background-color:rgb(15,08,01);color:gray;border-radius:4px;}");
    }
}

void ScanSet::setKylinScanSetEnable()
{
    KylinSane& instance = KylinSane::getInstance();
    bool device_status = true;

    device_status = instance.getKylinSaneStatus();

    if(device_status)
    {
        textColor->setEnabled(true);
        textColor->colorNormal();

        textSize->setEnabled(true);
        textSize->colorNormal();

        textResolution->setEnabled(true);
        textResolution->colorNormal();

        textFormat->setEnabled(true);
        textFormat->colorNormal();

        textName->setEnabled(true);

        btnLocation->setEnabled(true);
        btnLocation->setStyleSheet("QPushButton{border:4px solid #0D0400;background-repeat:no-repeat;background-position:right;background-color:#0D0400;color:rgb(232,232,232);border-radius:4px;text-align:left;}");

        btnMail->setEnabled(true);
        btnSave->setEnabled(true);
    }
}

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

    labColor->setText(tr("Color"));
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

    if(!device_status)
    {
        // No find scan device
        textType->setText(tr("Device type"));
        textType->setStyleSheet("QLabel{border:1px solid #0D0400;background-color:rgb(15,08,01);color:rgb(232,232,232);border-radius:4px;}");
     //   textType->setStyleSheet("QLabel{background-color:rgb(15,08,01);color:gray;border-radius:6px;}");
    }
    else {
        textType->setText(instance.getKylinSaneType());
        textType->setStyleSheet("QLabel{border:1px solid #0D0400;background-color:rgb(15,08,01);color:rgb(232,232,232);border-radius:4px;}");
    }
    textType->setFixedSize(180,32);

    textName->setText(tr("Scan filename"));
    textName->setStyleSheet("QLineEdit{border:1px solid #0D0400;background-color:rgb(15,08,01);color:rgb(232,232,232);border-radius:4px;}");
    textName->setFixedSize(180,32);
}

void ScanSet::setKylinLabelAttributes(QLabel *label)
{
    label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    label->setStyleSheet("color:rgb(232,232,232)");
    label->setFixedSize(40,32);
}


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
    hBoxName->addSpacing(16);
    hBoxName->addWidget(labName);
    hBoxName->addSpacing(8);
    hBoxName->addWidget(textName);
    hBoxName->addSpacing(16);
//    setKylinHBoxLayoutAttributes(hBoxName, labName, textName);
    hBoxName->setContentsMargins(0,4,0,4);

//    setKylinHBoxLayoutAttributes(hBoxLocation, labLocation, btnLocation);
//    hBoxLocation->setContentsMargins(0,4,0,4);
    hBoxLocation->setSpacing(0);
    hBoxLocation->addSpacing(16);
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

void ScanSet::setKylinHBoxLayoutAttributes(QHBoxLayout *layout, QLabel *labelFirst, QLabel *labelSecond)
{
    layout->setSpacing(0);
    layout->addSpacing(16);
    layout->addWidget(labelFirst);
    layout->addSpacing(8);
    layout->addWidget(labelSecond);
    layout->addSpacing(16);
}

void ScanSet::setKylinHBoxLayoutAttributes(QHBoxLayout *layout, QLabel *labelFirst, KylinComboBox *combo)
{
    layout->setSpacing(0);
    layout->addSpacing(16);
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

void ScanSet::setFontSize(QLabel *label, int n)
{
    QFont ft;
    ft.setPointSize(n);
    label->setFont(ft);
}

void ScanSet::on_btnLocation_clicked()
{
    if(curPath.isEmpty())
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

void ScanSet::on_btnMail_clicked()
{
    AppList * maillist = getAppIdList(MAILTYPE);
    if(!maillist)
    {
        no_mail *dialog = new no_mail(this);
        int ret= dialog->exec();// 以模态方式显示对话框，用户关闭对话框时返回 DialogCode值
        if(ret==QDialog::Accepted)
        {
            QProcess *process = new QProcess();
            process->start("/usr/bin/ubuntu-kylin-software-center");
        }
        delete dialog; //删除对话框
    }
    else
    {
        send_mail *dialog = new send_mail(this);
        dialog->set_btnList();
        dialog->exec();
    }
}

QString filter="*.jpg;;*.png;;*.pdf;;*.bmp"; //文件过滤器

void ScanSet::on_btnSave_clicked()
{
    //保存文件
    QString dlgTitle=tr("Save as ..."); //对话框标题
    //QString filter="文本文件(*.txt);;h文件(*.h);;C++文件(.cpp);;所有文件(*.*)"; //文件过滤器
    QString pathName = curPath + "/" + textName->text();
    QString aFileName=QFileDialog::getSaveFileName(this,dlgTitle,pathName,filter);
    if (!aFileName.isEmpty())
        emit save_image_signal(aFileName);
}

void ScanSet::on_textDevice_current_text_changed(QString device)
{
    bool status = true;

    KylinSane & instance = KylinSane::getInstance();
    instance.userInfo.name = device;
    qDebug() << "device name: "<< instance.userInfo.name;

    int index = textDevice->currentIndex(); // index是根据所选的进行判断,或者open_device直接根据所选进行判断
    if (index == -1)
    {
        index = 0;
    }
    qDebug() << "device index: " << index;
    //char *deviceName =

    //int index = 1;
    instance.open_device(index);

    status = instance.getKylinSaneStatus();
    if (status)
    {
        qDebug() << "open_device true";
        emit open_device_status(true);
    }
    else
    {
        qDebug() << "open_device false";
        emit open_device_status(false);
    }
}

void ScanSet::modify_save_button()
{
    if(flag == 0)
    {
        flag = 1;
        btnSave->setText(tr("Store text"));
        filter ="*.txt";
    }
    else {
        flag = 0;
        btnSave->setText(tr("Save as"));
        filter="*.jpg;;*.png;;*.pdf;;*.bmp;;"; //文件过滤器
    }
}

void ScanSet::on_textColor_current_text_changed(QString color)
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

void ScanSet::on_textResolution_current_text_changed(QString resolution)
{
    KylinSane & instance = KylinSane::getInstance();
    instance.userInfo.resolution = resolution;
    qDebug() << "resolution: "<< instance.userInfo.resolution;
}

void ScanSet::on_textSize_current_text_changed(QString size)
{
    KylinSane & instance = KylinSane::getInstance();
    instance.userInfo.size = size;
    qDebug() << "size: "<< instance.userInfo.size;
}
