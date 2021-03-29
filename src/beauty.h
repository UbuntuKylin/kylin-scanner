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

#ifndef BEAUTY_H
#define BEAUTY_H

#include <vector>
#include <iostream>
#include <fstream>
#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <opencv2/core/utility.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
using namespace cv;
using namespace std;

//#define CV_MINMAX       32

/******************************* opencv 图像处理 **********************************/
/// 高动态范围成像HDR
/// 基于多张不同曝光的图片才行,单张图片直接hdr效果不好,
/// 因此扫描图像的处理可参照ps，进行图片的对比度、亮度、饱和度、滤波、冷暖色调、锐化等操作。

//线性变换，进行亮度和对比度调整
void psLuminanceContrastCV(Mat src, Mat &dst);

//饱和度调节
void psSaturationCV(Mat src, Mat &dst);

//基于拉普拉斯算子的锐化美化
void psSharpenCV(Mat src, Mat &dst);

//转换色彩空间进行饱和度设置
void psHslCV(Mat src, Mat &dst);


////////////////////////start 图像平滑smoth（滤波filter，模糊blur)
/// 保留原图细节下，消除图片中噪声

//均值滤波
void psAverageFilterCV(Mat src, Mat &dst);

//高斯滤波
void psGaussianFilterCV(Mat src, Mat &dst);

//中值滤波
void psMedianFilterCV(Mat src, Mat &dst);

//双边滤波
void psBilateralFilterCV(Mat src, Mat &dst);

////////////////////////end


////////////////////////start 图像增强之增加对比度
//直方图均衡化
void psHistogramEqualizationCV(Mat src, Mat &dst);

//拉普拉斯算子
void psContrastCV(Mat src, Mat &dst);

//对数变换
void psLogarithmCV(Mat src, Mat &dst);

//伽马变换
void psGammaCV(Mat src, Mat &dst);
////////////////////////end

/**********************************************************************************/


/****************************** Qt QImage 图像处理 **********************************/
//锐化美化
QImage *psSharpen(QImage *origin);

//彩色图转换成灰度图
QImage *psGreyScale(QImage *origin);

//亮度调节
QImage *psLumimance(int delta, QImage *origin);

//饱和度调节
QImage *psSaturation(int delta, QImage *origin);

//暖色调调节
QImage *psWarmToned(int delta, QImage *origin);

//冷色调调节
QImage *psCoolToned(int delta, QImage *origin);

//添加边框
QImage *psDrawFrame(QImage *origin, const char *filename);

/**********************************************************************************/


//一键美化
void oneClickBeauty(const char *filename);



class KYCBeautyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KYCBeautyWidget(QWidget *parent = nullptr);

};

#endif // BEAUTY_H
