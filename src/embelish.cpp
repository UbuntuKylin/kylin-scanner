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
#include "embelish.h"

embelish::embelish(QWidget *parent) : QWidget(parent)
{

}

int readImagesAndTimesOne(Mat src, vector<Mat> &images, vector<float> &times)
{

  static const float timesArray = 1/0.03f;

  images.push_back(src);
  times.push_back(timesArray);

  return  0;
}

int readImagesAndTimes(vector<Mat> &images, vector<float> &times)
{

  int numImages = 4;

  //static const float timesArray[] = {0.25};
  static const float timesArray[] = {1/30.f, 0.25, 2.5, 15.0};
  times.assign(timesArray, timesArray + numImages);
  //times.assign(timesArray, timesArray);

  static const char* filenames[] = {"/tmp/img/img_0.033.jpg",
                                    "/tmp/img/img_0.25.jpg",
                                    "/tmp/img/img_2.5.jpg",
                                    "/tmp/img/img_15.jpg"};
  for (int i=0; i<numImages; i++)
  {
    Mat im = imread(filenames[i]);

    //判断图像是否加载成功
    if (im.data == NULL)
    {
        qDebug() << "图像加载失败!";
        return false;
    }
    else
    {
        qDebug() << "图像加载成功!";
    }

    images.push_back(im);
    //times.push_back(timesArray[i]);
  }
  return  0;
}
void psloadExposureSeq(String path, vector<Mat>& images, vector<float>& times)
{
    path = path + std::string("/");
    ifstream list_file((path + "list.txt").c_str());
    string name;
    float val;
    while(list_file >> name >> val) {
        Mat img = imread(path + name);
        images.push_back(img);
        times.push_back(1/val);
    }
    list_file.close();
}

void psHdrCV(Mat src)
{
  // Read images and exposure times
  cout << "Reading images ... " << endl;
  vector<Mat> images;
  vector<float> times;

  //readImagesAndTimes(images, times);
  readImagesAndTimesOne(src, images, times);


  // Align input images
  cout << "Aligning images ... " << endl;
  vector<Mat> images_(images);
  Ptr<AlignMTB> alignMTB = cv::createAlignMTB();
  alignMTB->process(images_, images);

  // Obtain Camera Response Function (CRF)
  cout << "Calculating Camera Response Function (CRF) ... " << endl;
  Mat responseDebevec;
  Ptr<CalibrateDebevec> calibrateDebevec = createCalibrateDebevec();
  calibrateDebevec->process(images, responseDebevec, times);


  // Merge images into an HDR linear image
  cout << "Merging images into one HDR image ... " ;
  Mat hdrDebevec;
  Ptr<MergeDebevec> mergeDebevec = createMergeDebevec();
  mergeDebevec->process(images, hdrDebevec, times, responseDebevec);
  // Save HDR image.
  imwrite("hdrDebevec.hdr", hdrDebevec);
  cout << "saved hdrDebevec.hdr "<< endl;

  // Tonemap using Drago's method to obtain 24-bit color image
  cout << "Tonemaping using Drago's method ... ";
  Mat ldrDrago;
  Ptr<TonemapDrago> tonemapDrago = createTonemapDrago(1.0, 0.7);
  tonemapDrago->process(hdrDebevec, ldrDrago);
  ldrDrago = 3 * ldrDrago;
  imwrite("/tmp/ldr-Drago.jpg", ldrDrago * 255);
  cout << "saved ldr-Drago.jpg"<< endl;

  // Tonemap using Durand's method obtain 24-bit color image
  cout << "Tonemaping using Durand's method ... ";
  /*
  Mat ldrDurand;
  Ptr<TonemapDurand> tonemapDurand = createTonemapDurand(1.5,4,1.0,1,1);
  tonemapDurand->process(hdrDebevec, ldrDurand);
  ldrDurand = 3 * ldrDurand;
  imwrite("/tmp/ldr-Durand.jpg", ldrDurand * 255);
  cout << "saved ldr-Durand.jpg"<< endl;
  */

  // Tonemap using Reinhard's method to obtain 24-bit color image
  cout << "Tonemaping using Reinhard's method ... ";
  Mat ldrReinhard;
  Ptr<TonemapReinhard> tonemapReinhard = createTonemapReinhard(1.5, 0,0,0);
  tonemapReinhard->process(hdrDebevec, ldrReinhard);
  imwrite("/tmp/ldr-Reinhard.jpg", ldrReinhard * 255);
  cout << "saved ldr-Reinhard.jpg"<< endl;

  // Tonemap using Mantiuk's method to obtain 24-bit color image
  cout << "Tonemaping using Mantiuk's method ... ";
  Mat ldrMantiuk;
  Ptr<TonemapMantiuk> tonemapMantiuk = createTonemapMantiuk(2.2,0.85, 1.2);
  tonemapMantiuk->process(hdrDebevec, ldrMantiuk);
  ldrMantiuk = 3 * ldrMantiuk;
  imwrite("/tmp/ldr-Mantiuk.jpg", ldrMantiuk * 255);
  cout << "saved ldr-Mantiuk.jpg"<< endl;
}

void psAverageFilterCV(Mat src, Mat &dst)
{
    blur(src, dst, Size(3, 3), Point(-1, -1));
}

void psGaussianFilterCV(Mat src, Mat &dst)
{
    GaussianBlur(src, dst, Size(5, 5), 5, 5);
}

void psMedianFilterCV(Mat src, Mat &dst)
{
    medianBlur(src, dst, 5);
}

void psBilateralFilterCV(Mat src, Mat &dst)
{
    bilateralFilter(src, dst, 5, 100, 3);
}

QImage * psSharpen(QImage * origin)
{
    QImage * novelImage = new QImage(* origin);

    int kernel [3][3]= {{0,-1,0},
                        {-1,5,-1},
                        {0,-1,0}};
    int kernelSize = 3;
    int sumKernel = 1;
    int r,g,b;
    QColor color;

    for (int x=kernelSize/2; x<novelImage->width()-(kernelSize/2); x++){
        for (int y=kernelSize/2; y<novelImage->height()-(kernelSize/2); y++){

            r = 0;
            g = 0;
            b = 0;

            for (int i = -kernelSize/2; i<= kernelSize/2; i++){
                for (int j = -kernelSize/2; j<= kernelSize/2; j++){
                    color = QColor(origin->pixel(x+i, y+j));
                    r += color.red()*kernel[kernelSize/2+i][kernelSize/2+j];
                    g += color.green()*kernel[kernelSize/2+i][kernelSize/2+j];
                    b += color.blue()*kernel[kernelSize/2+i][kernelSize/2+j];
                }
            }

            r = qBound(0, r/sumKernel, 255);
            g = qBound(0, g/sumKernel, 255);
            b = qBound(0, b/sumKernel, 255);

            novelImage->setPixel(x,y, qRgb(r,g,b));

        }
    }
    return novelImage;
}


/**
 * @brief psSharpenCV
 * sharp fileter
 *
 * @param src src_image
 * @param dst dst_image
 * @return
 */
void psSharpenCV(Mat src, Mat &dst)
{
    qDebug() << "sharp()";

    Point2i anchor(-1,-1);
    double delta = 0;

    Mat kernel = (Mat_<char>(3, 3) <<
                  0, -1, 0,
                  -1, 5, -1,
                  0, -1, 0);

    filter2D(src, dst, dst.depth(), kernel, anchor, delta);
    qDebug() << "sharp() end";
}

static void checkHsl(int &hue, int &saturation, int &lumination)
{
    if ( hue<-180 )
        hue = -180;

    if ( saturation<-255)
        saturation = -255;

    if ( lumination<-255 )
        lumination = -255;

    if ( hue>180)
        hue = 180;

    if ( saturation>255)
        saturation = 255;

    if ( lumination>255)
        lumination = 255;

}

/**
 * @brief psHslCV
 * 图像色调/饱和度调节，就是让用户按照自己的感觉对图像的色调、饱和度二行亮度三个分量进行调整，以满足用户的需求
 * 实现原理：将输入的RGB空间的图像转换到HSL颜色空间，根据用户输入的H、S、L三个调整参数，
 * 分别加到对应的色彩分量上，从而改变原来图像的色彩。
 *
 * @param src
 * @param dst
 */
void psHslCV(Mat src, Mat &dst)
{
    // H:0~180, S:0~255, V:0~255
    int hue = 5; // 色调
    int saturation = 10; // 饱和度
    int lumination = 0; // 亮度
    if ( dst.empty())
            dst.create(src.rows, src.cols, src.type());

    Mat temp;
    temp.create(src.rows, src.cols, src.type());

    cvtColor(src, temp, CV_RGB2HSV);

    int i, j;
    Size size = src.size();
    int chns = src.channels();

    // creat()创建的矩阵都是连续的，但是也不绝对，依然需要判断才可以进行连续性操作
    if (temp.isContinuous())
    {
        size.width *= size.height;
        size.height = 1;
    }

    // 验证参数范围
    checkHsl(hue, saturation, lumination);

    for ( i= 0; i<size.height; ++i)
    {
        unsigned char* src = (unsigned char*)temp.data+temp.step*i;
        for ( j=0; j<size.width; ++j)
        {
            float val = src[j*chns]+hue;
            if ( val < 0) val = 0.0;
            if ( val > 180 ) val = 180;
            src[j*chns] = static_cast<unsigned char>(val);

            val = src[j*chns+1]+saturation;
            if ( val < 0) val = 0;
            if ( val > 255 ) val = 255;
            src[j*chns+1] = static_cast<unsigned char>(val);

            val = src[j*chns+2]+lumination;
            if ( val < 0) val = 0;
            if ( val > 255 ) val = 255;
            src[j*chns+2] = static_cast<unsigned char>(val);
        }
    }

    cvtColor(temp, dst, CV_HSV2RGB);
    if (temp.empty())
        temp.release();
}

/**
 * @brief luminanceContrast
 * g(i,j) = alpha * f(i,j) + beta
 * enhance brightness(alpha) and contrast(belta) of images
 * 亮度和对比度调整
 *
 * @param src src_image
 * @param dst dst_image
 * @return
 */
void psLuminanceContrastCV(Mat src, Mat &dst)
{
    qDebug() << "psLuminanceContrastCV()";
    Mat new_image = Mat::zeros(src.size(), src.type());
    double alpha = 1.0; // contrast
    int beta = 5;       // brightness
    int i=0, j=0;
    int c=0;

    for (i=0; i<src.rows; i++)
    {
        for (j=0; j<src.cols; j++)
        {
            for (c=0; c<3; c++)
            {
                new_image.at<Vec3b>(i,j)[c] =
                        saturate_cast<uchar>( alpha*(src.at<Vec3b>(i,j)[c]) + beta );
            }

        }
    }

    dst = new_image.clone();
    qDebug() << "psLuminanceContrastCV() end";
}


/**
 * @brief psSaturationCV
 * 饱和度使色彩更鲜艳
 *
 * @param src src_image
 * @param dst dst_image
 */
void psSaturationCV(Mat src, Mat &dst)
{
    qDebug() << "saturation()";
    Mat new_img;

    const int max_increment = 200;
    const int saturation = 100;
    float increment = (saturation - 80) * 1.0 / max_increment;

    for (int col = 0; col < src.cols; col++)
    {
        for (int row = 0; row < src.rows; row++)
        {
            // R,G,B 分别对应数组中下标的 2,1,0
            uchar r = src.at<Vec3b> (row, col)[2];
            uchar g = src.at<Vec3b> (row, col)[1];
            uchar b = src.at<Vec3b> (row, col)[0];

            float maxn = max (r, max (g, b));
            float minn = min (r, min (g, b));

            float delta, value;
            delta = (maxn - minn) / 255;
            value = (maxn + minn) / 255;

            float new_r=0.0f, new_g=0.0f, new_b=0.0f;

            const double eps=1.0e-6;
            if (fabs(delta - 0.0) <= eps)		 // 差为 0 不做操作，保存原像素点
            {
                new_img.at<Vec3b> (row, col)[0] = new_b;
                new_img.at<Vec3b> (row, col)[1] = new_g;
                new_img.at<Vec3b> (row, col)[2] = new_r;
                continue;
            }

            float light=0.0f, sat=0.0f, alpha=0.0f;
            light = value / 2;

            if (light < 0.5f)
            {
                sat = delta / value;
            }
            else
            {
                sat = delta / (2 - value);
            }

            cout << "1" <<endl;
            if (increment >= 0)
            {
                if ((increment + sat) >= 1)
                {
                    alpha = sat;
                }
                else
                {
                    alpha = 1 - increment;
                }
                alpha = 1 / alpha - 1;
                new_r = r + (r - light * 255) * alpha;
                new_g = g + (g - light * 255) * alpha;
                new_b = b + (b - light * 255) * alpha;
            }
            else
            {
                alpha = increment;
                new_r = light * 255 + (r - light * 255) * (1 + alpha);
                new_g = light * 255 + (g - light * 255) * (1 + alpha);
                new_b = light * 255 + (b - light * 255) * (1 + alpha);
            }
            cout << "2" <<endl;
            new_img.at<Vec3f> (row, col)[0] = new_b;
            cout << "3" <<endl;
            new_img.at<Vec3f> (row, col)[1] = new_g;
            cout << "4" <<endl;
            new_img.at<Vec3f> (row, col)[2] = new_r;
            cout << "5" <<endl;
        }
    }

    dst = new_img.clone();
    qDebug() << "saturation() end";
}


/**
 * @brief psHistogramEqualizationCV
 * 直方图均衡化是通过调整图像的灰阶分布，使得在0~255灰阶上的分布更加均衡，提高了图像的对比度，
 * 达到改善图像主观视觉效果的目的。
 * 对比度较低的图像适合使用直方图均衡化方法来增强图像细节。
 *
 * @param src
 * @param dst
 */
void psHistogramEqualizationCV(Mat src, Mat &dst)
{
    //定义一个Mat向量容器保存拆分后的数据
    vector<Mat> channels;
    Mat imageBlue, imageGreen, imageRed;

    //判断文件加载是否正确
    assert(src.data!=NULL);

    //通道的拆分
    split(src, channels);

    //提取蓝色通道的数据
    imageBlue = channels.at(0);
    equalizeHist(imageBlue, imageBlue);

    //提取绿色通道的数据
    imageGreen = channels.at(1);
    equalizeHist(imageGreen, imageGreen);

    //提取红色通道的数据
    imageRed = channels.at(2);
    equalizeHist(imageRed, imageRed);

    //对拆分的通道数据合并
    merge(channels, dst);
}

/**
 * @brief psContrastCV
 * 使用中心为5的8邻域拉普拉斯算子与图像卷积可以达到锐化增强图像的目的。
 * 拉普拉斯算子可以增强局部的图像对比度。
 *
 * @param src src_image
 * @param dst dst_image
 */
void psContrastCV(Mat src, Mat & dst)
{
    Mat kernel = (Mat_<float>(3, 3) <<
                                      0, -1, 0,
                                      0, 5, 0,
                                      0, -1, 0);

    filter2D(src, dst, CV_8UC3, kernel);
}


/**
 * @brief psLogarithmCV
 * 对数变换可以将图像的低灰度值部分扩展，显示出低灰度部分更多的细节，
 * 将其高灰度值部分压缩，减少高灰度值部分的细节，从而达到强调图像低灰度部分的目的。
 * 对数变换对于整体对比度偏低并且灰度值偏低的图像增强效果较好。
 *
 * @param src src_image
 * @param dst dst_image
 */
void psLogarithmCV(Mat src, Mat &dst)
{
    Mat imageLog(src.size(), CV_32FC3);
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            imageLog.at<Vec3f>(i, j)[0] = log(1 + src.at<Vec3b>(i, j)[0]);
            imageLog.at<Vec3f>(i, j)[1] = log(1 + src.at<Vec3b>(i, j)[1]);
            imageLog.at<Vec3f>(i, j)[2] = log(1 + src.at<Vec3b>(i, j)[2]);
        }
    }
    //归一化到0~255
    normalize(imageLog, imageLog, 0, 255, CV_MINMAX);
    //转换成8bit图像显示
    convertScaleAbs(imageLog, imageLog);

    dst = imageLog.clone();
}

/**
 * @brief psGammaCV
 * 伽马变换主要用于图像的校正，将灰度过高或者灰度过低的图片进行修正，增强对比度。
 * 伽马变换对于图像对比度偏低，并且整体亮度值偏高（对于于相机过曝）情况下的图像增强效果明显。
 *
 * @param src
 * @param dst
 */
void psGammaCV(Mat src, Mat &dst)
{
    Mat imageGamma(src.size(), CV_32FC3);
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            imageGamma.at<Vec3f>(i, j)[0] = (src.at<Vec3b>(i, j)[0])*(src.at<Vec3b>(i, j)[0])*(src.at<Vec3b>(i, j)[0]);
            imageGamma.at<Vec3f>(i, j)[1] = (src.at<Vec3b>(i, j)[1])*(src.at<Vec3b>(i, j)[1])*(src.at<Vec3b>(i, j)[1]);
            imageGamma.at<Vec3f>(i, j)[2] = (src.at<Vec3b>(i, j)[2])*(src.at<Vec3b>(i, j)[2])*(src.at<Vec3b>(i, j)[2]);
        }
    }
    //归一化到0~255
    normalize(imageGamma, imageGamma, 0, 255, CV_MINMAX);
    //转换成8bit图像显示
    convertScaleAbs(imageGamma, imageGamma);
    dst = imageGamma.clone();
}

QImage * psGreyScale(QImage * origin)
{
    QImage * novelImage = new QImage(origin->width(), origin->height(), QImage::Format_ARGB32);

    QColor oldColor;

    for (int x = 0; x<novelImage->width(); x++)
    {
        for (int y = 0; y<novelImage->height(); y++)
        {
            oldColor = QColor(origin->pixel(x,y));

            // gray RGB is set average of oldRGB
            int average = (oldColor.red()+oldColor.green()+oldColor.blue())/3;
            novelImage->setPixel(x,y,qRgb(average,average,average));
        }
    }

    return novelImage;
}


QImage * psLumimance(int delta, QImage * origin)
{
    QImage * novelImage = new QImage(origin->width(), origin->height(), QImage::Format_ARGB32);

    QColor oldColor;

    for (int x = 0; x<novelImage->width(); x++)
    {
        for (int y = 0; y<novelImage->height(); y++)
        {
            oldColor = QColor(origin->pixel(x,y));

            //　RGB common add delta
            novelImage->setPixel(x,y,qRgb(oldColor.red() + delta,
                                        oldColor.green() + delta,
                                        oldColor.blue() + delta));
        }
    }

    return novelImage;

}

QImage *psSaturation(int delta, QImage * origin)
{
    QImage * novelImage = new QImage(origin->width(), origin->height(), QImage::Format_ARGB32);

    QColor oldColor;
    QColor novelColor;
    int h,s,l;

    for (int x=0; x<novelImage->width(); x++)
    {
        for (int y=0; y<novelImage->height(); y++){
            oldColor = QColor(origin->pixel(x,y));

            novelColor = oldColor.toHsl();
            h = novelColor.hue();
            s = novelColor.saturation()+delta;
            l = novelColor.lightness();

            //Check if the new value is [0, 255]
            s = qBound(0, s, 255);

            novelColor.setHsl(h, s, l);

            novelImage->setPixel(x, y, qRgb(novelColor.red(), novelColor.green(), novelColor.blue()));
        }
    }

    return novelImage;
}

QImage * psWarmToned(int delta, QImage * origin)
{
    QImage *newImage = new QImage(origin->width(), origin->height(), QImage::Format_ARGB32);

    QColor oldColor;
    int r,g,b;

    for (int x=0; x<newImage->width(); x++)
    {
        for (int y=0; y<newImage->height(); y++)
        {
            oldColor = QColor(origin->pixel(x,y));

            r = oldColor.red() + delta;
            g = oldColor.green() + delta;
            b = oldColor.blue();

            //we check if the new values are between 0 and 255
            r = qBound(0, r, 255);
            g = qBound(0, g, 255);

            newImage->setPixel(x,y, qRgb(r,g,b));
        }
    }

    return newImage;
}


QImage * psCoolToned(int delta, QImage * origin)
{
    QImage *newImage = new QImage(origin->width(), origin->height(), QImage::Format_ARGB32);

    QColor oldColor;
    int r,g,b;

    for (int x=0; x<newImage->width(); x++)
    {
        for (int y=0; y<newImage->height(); y++)
        {
            oldColor = QColor(origin->pixel(x,y));

            r = oldColor.red();
            g = oldColor.green();
            b = oldColor.blue()+delta;

            //we check if the new value is between 0 and 255
            b = qBound(0, b, 255);

            newImage->setPixel(x,y, qRgb(r,g,b));
        }
    }

    return newImage;
}

QImage * psDrawFrame(QImage * origin, const char *filename)
{
    // 边框和图片不一样大时，需要resize()
    QImage * newImage = new QImage(* origin);
    QPainter painter;

    painter.begin(newImage);

    painter.drawImage(0,0, QImage(filename));

    painter.end();

    return newImage;
}

void oneClickEmbelish(const char *filename)
{
    Mat src, dst;

    src = imread(filename);

    //判断图像是否加载成功
    if (src.data == NULL)
    {
        qDebug() << "图像加载失败!";
        return;
    }
    else
    {
        qDebug() << "图像加载成功!";
    }

    // 双边滤波
    psBilateralFilterCV(src, dst);

    // 线性变换，进行亮度和对比度调整
    psLuminanceContrastCV(dst, dst);

    // 转换色彩空间进行饱和度设置
    psHslCV(dst, dst);

    // 锐化
    psSharpenCV(dst, dst);

    imwrite(filename, dst);
}
