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
#include "rectify.h"

//度数转换
double DegreeTrans(double theta)
{
    double res = theta / CV_PI * 180;
    return res;
}


//逆时针旋转图像degree角度（原尺寸）
void rotateImage(Mat src, Mat& img_rotate, double degree)
{
    //旋转中心为图像中心
    Point2f center;
    center.x = float(src.cols / 2.0);
    center.y = float(src.rows / 2.0);
    cout<<"center.x"<<center.x<<endl;
    cout<<"center.y"<<center.y<<endl;

    int length = 0;
    length = sqrt(src.cols*src.cols + src.rows*src.rows);
    //计算二维旋转的仿射变换矩阵
    Mat M = getRotationMatrix2D(center, degree, 1);
//    warpAffine(src, img_rotate, M, Size(length, length), 1, 0, Scalar(255,255,255));//仿射变换，背景色填充为白色
    warpAffine(src, img_rotate, M, img_rotate.size(), 1, 0, Scalar(255,255,255));//仿射变换，背景色填充为白色
}
//通过霍夫变换计算角度
double CalcDegree(const Mat &srcImage, Mat &dst)
{
    Mat midImage, dstImage;

    Canny(srcImage, midImage, 50, 200, 3);
    cvtColor(midImage, dstImage, COLOR_GRAY2BGR);

    //通过霍夫变换检测直线
    vector<Vec2f> lines;
    HoughLines(midImage, lines, 1, CV_PI / 180, 300, 0, 0);//第5个参数就是阈值，阈值越大，检测精度越高
    //cout << lines.size() << endl;

    //由于图像不同，阈值不好设定，因为阈值设定过高导致无法检测直线，阈值过低直线太多，速度很慢
    //所以根据阈值由大到小设置了三个阈值，如果经过大量试验后，可以固定一个适合的阈值。

    if (!lines.size())
    {
        HoughLines(midImage, lines, 1, CV_PI / 180, 200, 0, 0);
    }

    if (!lines.size())
    {
        HoughLines(midImage, lines, 1, CV_PI / 180, 150, 0, 0);
    }
    if (!lines.size())
    {
        cout << "没有检测到直线！" << endl;
        return ERROR;
    }

    float sum = 0;
    int n = 0;
    //依次画出每条线段
    for (size_t i = 0; i < lines.size(); i++)
    {
        float rho = lines[i][0];
        float theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000 * (-b));
        pt1.y = cvRound(y0 + 1000 * (a));
        pt2.x = cvRound(x0 - 1000 * (-b));
        pt2.y = cvRound(y0 - 1000 * (a));
        //只选角度最小的作为旋转角度
        if((DegreeTrans(theta) >= 88.5) && (DegreeTrans(theta) <= 91))
        {
            n +=1;
            continue;
        }
        if((DegreeTrans(theta) >= 178.5))
        {
            n +=1;
            continue;
        }
        if((DegreeTrans(theta) <= 1))
        {
            n +=1;
            continue;
        }
        sum += theta;
        line(dstImage, pt1, pt2, Scalar(55, 100, 195), 1, LINE_AA); //Scalar函数用于调节线段颜色
    }
    float average = sum / (lines.size() - n); //对所有角度求平均，这样做旋转效果会更好

    double angle = DegreeTrans(average);
    if(angle >= 135)
        angle = angle - 180;
    else if(angle >= 90)
        angle =angle - 90;
    else if(angle >= 45)
        angle =  angle - 90;

    rotateImage(dstImage, dst, angle);
    return angle;
}


int ImageRectify(const char * pInFileName)
{
    double degree;

    Mat src = imread(pInFileName);
    Mat dst;
    //倾斜角度矫正
    degree = CalcDegree(src,dst);
    if (degree == ERROR)
    {
        cout << "矫正失败！" << endl;
        return -1;
    }
    if(abs(degree) < 0.4)
    {
        cout<<"return"<<endl;
        return -1;

    }
    else
    {   src = imread(pInFileName);
        rotateImage(src, dst, degree);
        imwrite("/tmp/scanner/scan1.png",dst);
        return 0;
    }

}
