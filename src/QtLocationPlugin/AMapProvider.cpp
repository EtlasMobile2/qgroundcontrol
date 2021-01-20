/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "AMapProvider.h"
#if defined(DEBUG_AMAP_MAPS)
#include <QFile>
#include <QStandardPaths>
#endif
#include <QtGlobal>

#include "QGCMapEngine.h"

AMapProvider::AMapProvider(const QString &imageFormat, const quint32 averageSize, const QGeoMapType::MapStyle mapType, QObject *parent)
    : MapProvider(QStringLiteral("https://www.google.cn/maps/preview"), imageFormat, averageSize, mapType, parent)
{

}

AMapProvider::~AMapProvider() {

}


QString AMAPStreetMapProvider::_getURL(const int x, const int y, const int zoom, QNetworkAccessManager* networkManager) {
    Q_UNUSED(networkManager)
    return QString("http://webst03.is.autonavi.com/appmaptile?x=%1&y=%2&z=%3&lang=zh_cn&size=1&scale=1&style=8").arg(x).arg(y).arg(zoom);

}

QString AMAPSatelliteMapProvider::_getURL(const int x, const int y, const int zoom, QNetworkAccessManager* networkManager) {
    Q_UNUSED(networkManager)
    QString url;
    url= QString("http://webst03.is.autonavi.com/appmaptile?x=%1&y=%2&z=%3&lang=zh_cn&size=1&scale=1&style=6").arg(x).arg(y).arg(zoom);
    return url;
}

QString AMAPLabelsMapProvider::_getURL(const int x, const int y, const int zoom, QNetworkAccessManager* networkManager) {
    Q_UNUSED(networkManager)
    Q_UNUSED(x)
    Q_UNUSED(y)
    Q_UNUSED(zoom)
    return "";
}

QString AMAPTerrainMapProvider::_getURL(const int x, const int y, const int zoom, QNetworkAccessManager* networkManager) {
    Q_UNUSED(networkManager)
    Q_UNUSED(x)
    Q_UNUSED(y)
    Q_UNUSED(zoom)
    return "";
}

QString AMAPHybridMapProvider::_getURL(const int x, const int y, const int zoom, QNetworkAccessManager* networkManager) {
    Q_UNUSED(networkManager)
    QString key = _tileXYToQuadKey(x, y, zoom);
    return QString("http://webst03.is.autonavi.com/appmaptile?x=%1&y=%2&z=%3&lang=zh_cn&size=1&scale=1&style=7").arg(x).arg(y).arg(zoom);

}
