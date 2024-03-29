#include "svghandler.h"
#include <QApplication>
#include <QDebug>

#define THEME_SCHEMA "org.ukui.style"
#define THEME_KEY "styleName"

SVGHandler::SVGHandler(QObject *parent,bool highLight)
    : QObject(parent)
    , themeSettings(new QGSettings(ORG_UKUI_STYLE))
{
    m_color = "black";
    stylelist << STYLE_NAME_KEY_DARK << STYLE_NAME_KEY_BLACK;

    if (highLight) {
        if (stylelist.contains(themeSettings->get(STYLE_NAME).toString())) {
            // 黑色主题
            m_color = "white";
        } else {
            m_color = "black";
        }

        qDebug() << "m_color = " << m_color;
        connect(themeSettings,&QGSettings::changed,this,[=] (const QString &key) {
           if (key == STYLE_NAME) {
               if (stylelist.contains(themeSettings->get(STYLE_NAME).toString())) {
                   m_color = "white";
               } else {
                   m_color = "black";
               }
               qDebug() << "m_color = " << m_color;
           }
        });
    }
}


const QPixmap SVGHandler::loadSvg(const QString &fileName,int size)
{
    const auto ratio = qApp->devicePixelRatio();
    if ( 2 == ratio) {
        size = 2 * size;
    } else if (3 == ratio) {
        size = 3 * size;
    }
    QPixmap pixmap(size, size);
    QSvgRenderer renderer(fileName);
    pixmap.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pixmap);
    renderer.render(&painter);
    painter.end();

    pixmap.setDevicePixelRatio(ratio);
    return pixmap;
}

const QPixmap SVGHandler::loadSvgColor(const QString &path, const QString &color, int size)
{
    int origSize = size;
    const auto ratio = qApp->devicePixelRatio();
    if ( 2 == ratio) {
        size += origSize;
    } else if (3 == ratio) {
        size += origSize;
    }
    QPixmap pixmap(size, size);
    QSvgRenderer renderer(path);
    pixmap.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pixmap);
    renderer.render(&painter);
    painter.end();

    pixmap.setDevicePixelRatio(ratio);

    return drawSymbolicColoredPixmap(pixmap, color);

}

const QPixmap SVGHandler::loadSvgColorXY(const QString &path, const QString &color, int sizeX, int sizeY)
{
    int origSize = sizeX;
    const auto ratio = qApp->devicePixelRatio();
    if ( 2 == ratio) {
        sizeX += origSize;
    } else if (3 == ratio) {
        sizeX += origSize;
    }
    QPixmap pixmap(sizeX, sizeY);
    QSvgRenderer renderer(path);
    pixmap.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pixmap);
    renderer.render(&painter);
    painter.end();

    pixmap.setDevicePixelRatio(ratio);

    return drawSymbolicColoredPixmap(pixmap, color);

}

QPixmap SVGHandler::drawSymbolicColoredPixmap(const QPixmap &source, QString cgColor)
{
    QImage img = source.toImage();
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            auto color = img.pixelColor(x, y);
            if (color.alpha() > 0) {
                if ( "white" == cgColor) {
                    color.setRed(255);
                    color.setGreen(255);
                    color.setBlue(255);
                    img.setPixelColor(x, y, color);
                } else if ( "black" == cgColor) {
                    color.setRed(0);
                    color.setGreen(0);
                    color.setBlue(0);
                    img.setPixelColor(x, y, color);
                } else if ("gray"== cgColor) {
                    color.setRed(152);
                    color.setGreen(163);
                    color.setBlue(164);
                    img.setPixelColor(x, y, color);
                } else if ("blue" == cgColor){
                    color.setRed(61);
                    color.setGreen(107);
                    color.setBlue(229);
                    img.setPixelColor(x, y, color);
                } else {
                    return source;
                }
            }
        }
    }
    return QPixmap::fromImage(img);
}
