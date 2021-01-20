
#include <math.h>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <stdio.h>
#include "CoordinateMap.h"

double CoordinateMap::transformLat(double x, double y)
{
  double num = ((((-100.0 + (2.0 * x)) + (3.0 * y)) + ((0.2 * y) * y)) + ((0.1 * x) * y)) + (0.2 * sqrt(abs(x)));
  num += (((20.0 * sin((6.0 * x) * 3.1415926535897931)) + (20.0 * sin((2.0 * x) * 3.1415926535897931))) * 2.0) / 3.0;
  num += (((20.0 * sin(y * 3.1415926535897931)) + (40.0 * sin((y / 3.0) * 3.1415926535897931))) * 2.0) / 3.0;
  return (num + ((((160.0 * sin((y / 12.0) * 3.1415926535897931)) + (320.0 * sin((y * 3.1415926535897931) / 30.0))) * 2.0) / 3.0));
}

double CoordinateMap::transformLon(double x, double y)
{
  double num = ((((300.0 + x) + (2.0 * y)) + ((0.1 * x) * x)) + ((0.1 * x) * y)) + (0.1 * sqrt(abs(x)));
  num += (((20.0 * sin((6.0 * x) * 3.1415926535897931)) + (20.0 * sin((2.0 * x) * 3.1415926535897931))) * 2.0) / 3.0;
  num += (((20.0 * sin(x * 3.1415926535897931)) + (40.0 * sin((x / 3.0) * 3.1415926535897931))) * 2.0) / 3.0;
  return (num + ((((150.0 * sin((x / 12.0) * 3.1415926535897931)) + (300.0 * sin((x / 30.0) * 3.1415926535897931))) * 2.0) / 3.0));
}
bool CoordinateMap::outOfChina(double lat, double lon)
{
    if (((lon >= 72.004) && (lon <= 137.8347)) && ((lat >= 0.8293) && (lat <= 55.8271)))
    {
        return false;
    }
    return true;
}

/* 火星坐标变换 */
QGeoCoordinate CoordinateMap::calculateDelta(double lat, double lon)
{
   double pi = 3.14159265358979324;
   QGeoCoordinate d_latlon;
   double a = 6378245.0; //  a: 卫星椭球坐标投影到平面地图坐标系的投影因子。
   double ee = 0.00669342162296594323; //  ee: 椭球的偏心率。

   d_latlon.setLatitude(transformLat(lon - 105.0, lat - 35.0));
   d_latlon.setLongitude(transformLon(lon - 105.0, lat - 35.0));
   double radLat = lat / 180.0 * pi;
   double magic = sin(radLat);
   magic = 1 - ee * magic * magic;
   double sqrtMagic = sqrt(magic);
   d_latlon.setLatitude((d_latlon.latitude() * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi));
   d_latlon.setLongitude((d_latlon.longitude() * 180.0) / (a / sqrtMagic * cos(radLat) * pi));
   return d_latlon;
}
/* wgs -----> gcj */
QGeoCoordinate CoordinateMap::wgs2gcj(QGeoCoordinate wgs)
{
    QGeoCoordinate gcj = wgs;

    if(outOfChina(wgs.latitude(),wgs.longitude()))
        return wgs;
    QGeoCoordinate d = calculateDelta(wgs.latitude(),wgs.longitude());
    gcj.setLatitude(wgs.latitude() + d.latitude());
    gcj.setLongitude(wgs.longitude() + d.longitude());
    //设置高度
    gcj.setAltitude(wgs.altitude());
    return gcj;

}

QGeoCoordinate CoordinateMap::gcj2wgs(QGeoCoordinate gcj)
{
    QGeoCoordinate wgs;
    if(outOfChina(gcj.latitude(),gcj.longitude()))
        return gcj;
    QGeoCoordinate d = calculateDelta(gcj.latitude(),gcj.longitude());
    wgs.setLatitude(gcj.latitude() - d.latitude());
    wgs.setLongitude(gcj.longitude() - d.longitude());
    //设置高度（有待和黄龙瑞商讨是否要设置高度） **刘悬
    wgs.setAltitude(gcj.altitude());
    return wgs;
}

/* gcj ----> wgs */
QGeoCoordinate CoordinateMap::gcj2wgs_exact(QGeoCoordinate gcj)
{
    double initDelta = 0.01;
    double threshold = 0.000000001;
    double dLat = initDelta, dLon = initDelta;
    double mLat = gcj.latitude() - dLat, mLon = gcj.longitude() - dLon;
    double pLat = gcj.latitude() + dLat, pLon = gcj.longitude() + dLon;
    QGeoCoordinate wgs;
    int i = 0;
    while (1) {
        wgs.setLatitude((mLat + pLat) / 2);
        wgs.setLongitude((mLon + pLon) / 2);
        QGeoCoordinate tmp = wgs2gcj(wgs);
        dLat = tmp.latitude() - gcj.latitude();
        dLon = tmp.longitude() - gcj.longitude();
        if ((abs(dLat) < threshold) && (abs(dLon) < threshold))
            break;

        if (dLat > 0) pLat = wgs.latitude(); else mLat = wgs.latitude();
        if (dLon > 0) pLon = wgs.longitude(); else mLon = wgs.longitude();

        if (++i > 10000) break;
    }

    return wgs;
}

