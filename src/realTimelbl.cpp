#include "realTimelbl.h"

RealTimeLbl::RealTimeLbl(QWidget *parent) : QLabel(parent)
{

}

RealTimeLbl::~RealTimeLbl()
{

}

void RealTimeLbl::paintEvent(QPaintEvent *event)
{
    //先调用父类的paintEvent
    QLabel::paintEvent(event);

    QPainter painter(this);
    QImage image(data, width, height, QImage::Format_RGB888);
    painter.drawImage(QRect(0, 0, width, height), image);
}
