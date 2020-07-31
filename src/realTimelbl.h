#ifndef REALTIMELBL_H
#define REALTIMELBL_H

#include <QWidget>
#include <QtGui>
#include <QQueue>
#include <QLabel>
#include <QPainter>
#include <QPicture>

class RealTimeLbl : public QLabel
{
    Q_OBJECT

public:
    explicit RealTimeLbl(QWidget *parent = nullptr);
    ~RealTimeLbl();

    int width;
    int height;
    const uchar * data;   //图像信息

protected:
    void paintEvent(QPaintEvent *event);

signals:

};

#endif // REALTIMELBL_H
