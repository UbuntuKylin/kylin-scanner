#ifndef QL_SVG_HANDLER_H
#define QL_SVG_HANDLER_H

#include <QObject>
#include <QPixmap>
#include <QSvgRenderer>
#include <QPainter>
#include <QApplication>
#include <QGSettings/QGSettings>
#include "theme.h"

class SVGHandler : public QObject
{
    Q_OBJECT
public:
    explicit SVGHandler(QObject *parent = nullptr,bool highLight = false);
    const QPixmap loadSvg(const QString &fileName,int size = 24);
    const QPixmap loadSvgColor(const QString &path, const QString &color, int size = 48);
    const QPixmap loadSvgColorXY(const QString &path, const QString &color, int sizeX = 48, int sizeY = 48);
    QPixmap drawSymbolicColoredPixmap(const QPixmap &source, QString cgColor);
private:
    QString m_color;
    QStringList stylelist;
    QStringList iconthemelist;

    QGSettings *themeSettings;

Q_SIGNALS:

};

#endif // QL_SVG_HANDLER_H
