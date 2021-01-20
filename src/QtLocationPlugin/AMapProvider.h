/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "MapProvider.h"
#include <QNetworkReply>
#include <QMutex>

class AMapProvider : public MapProvider {
    Q_OBJECT

public:
    AMapProvider(const QString& imageFormat, const quint32 averageSize,
                      const QGeoMapType::MapStyle _mapType, QObject* parent = nullptr);

    ~AMapProvider();


};

static const quint32 AVERAGE_AMAP_STREET_MAP  = 4913;
static const quint32 AVERAGE_AMAP_SAT_MAP = 56887;
static const quint32 AVERAGE_AMAP_TERRAIN_MAP = 19391;

// -----------------------------------------------------------
// AMAP Street Map

class AMAPStreetMapProvider : public AMapProvider
{
    Q_OBJECT

public:
    AMAPStreetMapProvider(QObject *parent = nullptr)
        : AMapProvider(QStringLiteral("png"), AVERAGE_AMAP_STREET_MAP, QGeoMapType::StreetMap, parent) {}

protected:
     QString _getURL(const int x, const int y, const int zoom, QNetworkAccessManager* networkManager) override;
};

// -----------------------------------------------------------
// AMAP Street Map

class AMAPSatelliteMapProvider : public AMapProvider
{
    Q_OBJECT

public:
    AMAPSatelliteMapProvider(QObject *parent = nullptr)
        : AMapProvider(QStringLiteral("jpg"), AVERAGE_AMAP_SAT_MAP,
                       QGeoMapType::SatelliteMapDay, parent) {}

protected:
    QString _getURL(const int x, const int y, const int zoom, QNetworkAccessManager* networkManager) override;
};

// -----------------------------------------------------------
// AMAP Labels Map

class AMAPLabelsMapProvider : public AMapProvider
{
    Q_OBJECT

public:
    AMAPLabelsMapProvider(QObject *parent = nullptr)
        : AMapProvider(QStringLiteral("png"), AVERAGE_TILE_SIZE, QGeoMapType::CustomMap, parent) {}

protected:
    QString _getURL(const int x, const int y, const int zoom, QNetworkAccessManager* networkManager) override;
};

// -----------------------------------------------------------
// AMAP Terrain Map

class AMAPTerrainMapProvider : public AMapProvider
{
    Q_OBJECT

public:
    AMAPTerrainMapProvider(QObject *parent = nullptr)
        : AMapProvider(QStringLiteral("png"), AVERAGE_AMAP_TERRAIN_MAP, QGeoMapType::TerrainMap, parent) {}

protected:
    QString _getURL(const int x, const int y, const int zoom, QNetworkAccessManager* networkManager) override;
};

// -----------------------------------------------------------
// AMAP Hybrid Map

class AMAPHybridMapProvider : public AMapProvider
{
    Q_OBJECT

public:
    AMAPHybridMapProvider(QObject *parent = nullptr)
        : AMapProvider(QStringLiteral("png"), AVERAGE_AMAP_SAT_MAP, QGeoMapType::HybridMap, parent) {}

protected:
    QString _getURL(const int x, const int y, const int zoom, QNetworkAccessManager* networkManager) override;
};
