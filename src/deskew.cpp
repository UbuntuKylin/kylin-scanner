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

#include "deskew.h"
#include "common.h"

//度数转换
double DegreeTrans(double theta)
{
    double res = theta / CV_PI * 180;
    return res;
}

//逆时针旋转图像degree角度（原尺寸）
void rotateImage(Mat src, Mat &img_rotate, double degree)
{
    //旋转中心为图像中心
    Point2f center;
    center.x = float(src.cols / 2.0);
    center.y = float(src.rows / 2.0);
    qDebug() << "center.x" << center.x;
    qDebug() << "center.y" << center.y;

    int length = 0;
    length = sqrt(src.cols * src.cols + src.rows * src.rows);
    qDebug() << "length = " << length;
    //计算二维旋转的仿射变换矩阵
    Mat M = getRotationMatrix2D(center, degree, 1);
//    warpAffine(src, img_rotate, M, Size(length, length), 1, 0, Scalar(255,255,255));//仿射变换，背景色填充为白色
    //仿射变换，背景色填充为白色
    warpAffine(src, img_rotate, M, img_rotate.size(), 1, 0, Scalar(255, 255, 255));
}

/**
 * @brief CalcDegree 通过霍夫变换计算角度
 * 只选角度最小的作为旋转角度
 * @param srcImage
 * @param dst
 * @return
 */
double CalcDegree(const Mat &srcImage, Mat &dst)
{
    Mat midImage;
    Mat dstImage;
    int Threshold = 300;
    int sizeLineBefore;
    int sizeLineAfter;

    Canny(srcImage, midImage, 50, 200, 3);
    cvtColor(midImage, dstImage, COLOR_GRAY2BGR);


    //通过霍夫变换检测直线
    // 600 dpi: Threshold = 750
    vector<Vec2f> lines;
    HoughLines(midImage, lines, 1, CV_PI / 180, Threshold, 0,
               0);//第5个参数就是阈值，阈值越大，检测精度越高
    //qDebug() << lines.size() ;

    qDebug() << "Begin to found good threshold.";
    // 由于图像不同，阈值不好设定，因为阈值设定过高导致无法检测直线，阈值过低直线太多，速度很慢
    // 所以根据阈值由大到小设置了三个阈值(300, 200, 100)，如果经过大量试验后，可以固定一个适合的阈值。
    // While Threshold is too small, lines will be large(>5000).
    // this will make rectify process slower,
    // so we should judge lines.size();
    while (lines.size() > 100 || lines.size() < 10) {
        if (lines.size() >100)
            Threshold += 300;
        else if (lines.size() < 10)
            Threshold -= 50;

        sizeLineBefore = lines.size();

        qDebug() << "Threshold = " << Threshold << "lines.size = " << lines.size();
        HoughLines(midImage, lines, 1, CV_PI / 180, Threshold, 0, 0);

        sizeLineAfter = lines.size();

        if ((sizeLineBefore > 100) && (sizeLineAfter < 10)) {
            // break this loop
            Threshold += 50;
            HoughLines(midImage, lines, 1, CV_PI / 180, Threshold, 0, 0);
            break;
        }

        if (Threshold <= 0) {
            qDebug() << "没有检测到直线！" ;
            return ERROR;
        }
    }
    qDebug() << "after while, Threshold = " << Threshold
             << "lines.size() = " << lines.size();

    float sum = 0;
    int n = 0;
    //依次画出每条线段
    size_t i = 0;
    qDebug() << "lines.size = " << lines.size ();
    for (i = 0; i < lines.size(); ++i) {
        float rho = lines[i][0];
        float theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        pt1.x = cvRound(x0 + 1000 * (-b));
        pt1.y = cvRound(y0 + 1000 * (a));
        pt2.x = cvRound(x0 - 1000 * (-b));
        pt2.y = cvRound(y0 - 1000 * (a));
        //qDebug() << i << ", " << "DegreeTrans() = " << DegreeTrans (theta);

        // 时常会遇到角度为10度返回内的图片，此时处理机制为默认已经高度校正，过滤该线条角度
        if (((DegreeTrans(theta) >= 80) && (DegreeTrans(theta) <= 95))
                || (DegreeTrans(theta) >= 178.5)
                || (DegreeTrans(theta) <= 8 )) {
            n += 1;
            continue;
        }

        sum += theta;
        //qDebug() << "sum = " << sum;
        line(dstImage, pt1, pt2, Scalar(55, 100, 195), 1, LINE_AA); //Scalar函数用于调节线段颜色
        // mirror picture for deskew(rectify)
        imwrite("/tmp/scanner/mirror.png", dstImage);
    }

    qDebug() << "sum = " << sum << "lines.size()" << lines.size () << "n = " << n;
    if (lines.size () - n == 0 || sum == 0)
        return 0.0;

    //对所有角度求平均，这样做旋转效果会更好
    float average = sum / (lines.size() - n);

    double angle = DegreeTrans(average);
    qDebug() << "angle = " << angle;

    // angel < 0: clockwise rotation
    // 经过多次反复测试，此处应该减90，整个线条接近水平，之后可以旋转进行校正。
    angle = angle - 90;

    /*
    if (angle >= 135)
        angle = angle - 180;
    else if (angle >= 90)
        angle =angle - 90;
    else if (angle >= 45)
        angle =  angle - 90;
    */

    qDebug() << "angle = " << angle;

    rotateImage(dstImage, dst, angle);
    return angle;
}

int ImageRectify(const char *pInFileName)
{
    double degree;

    Mat src = imread(pInFileName);
    Mat dst;
    //倾斜角度矫正
    degree = CalcDegree(src, dst);
    qDebug() << "degree = " << degree;

    if (fabs (degree - ERROR) < 1e-15) {
        qDebug() << "矫正失败！" ;
        return -1;
    }
    if (fabs(degree) < 1e-15 + 0.4) {
        qDebug() << "already right, so return straight!";
        return -1;
    }

    src = imread(pInFileName);
    rotateImage(src, dst, degree);
    imwrite(SCANNING_PICTURE_PATH, dst);
    return 0;
}
