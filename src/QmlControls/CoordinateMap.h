#ifndef CoordinateMap_H
#define CoordinateMap_H

#include <QObject>
#include <QGeoCoordinate>

class CoordinateMap : public QObject
{
    Q_OBJECT
public:
    /* wgs84坐标转换到火星坐标*/
    QGeoCoordinate wgs2gcj(QGeoCoordinate wgs);
    QGeoCoordinate gcj2wgs(QGeoCoordinate gcj);
    /** 火星坐标转换到wgs84坐标(二分精确位置)*/
    QGeoCoordinate gcj2wgs_exact(QGeoCoordinate gcj);

    double transformLat(double x, double y);
    double transformLon(double x, double y);
    bool outOfChina(double lat, double lon);
    QGeoCoordinate calculateDelta(double wgLat, double wgLon);
 };

#endif

