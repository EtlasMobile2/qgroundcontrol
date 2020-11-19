/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "VideoSettings.h"
#include "QGCApplication.h"
#include "VideoManager.h"

#include <QQmlEngine>
#include <QtQml>
#include <QVariantList>

#ifndef QGC_DISABLE_UVC
#include <QCameraInfo>
#endif

#ifdef __android__
#include <QtAndroidExtras/QAndroidJniObject>
#define VIDEO_SHARE_PROP_NAME "persist.fpv.ext.disp"
#endif

const char* VideoSettings::videoSourceNoVideo   = "No Video Available";
const char* VideoSettings::videoDisabled        = "Video Stream Disabled";
const char* VideoSettings::videoSourceRTSP      = "RTSP Video Stream";
const char* VideoSettings::videoSourceUDPH264   = "UDP h.264 Video Stream";
const char* VideoSettings::videoSourceUDPH265   = "UDP h.265 Video Stream";
const char* VideoSettings::videoSourceTCP       = "TCP-MPEG2 Video Stream";
const char* VideoSettings::videoSourceMPEGTS    = "MPEG-TS (h.264) Video Stream";
const char* VideoSettings::videoSourceAuto      = "Auto Connection Video Stream";

DECLARE_SETTINGGROUP(Video, "Video")
{
    qmlRegisterUncreatableType<VideoSettings>("QGroundControl.SettingsManager", 1, 0, "VideoSettings", "Reference only");

    // Setup enum values for videoSource settings into meta data
    QStringList videoSourceList;
    videoSourceList.append(videoDisabled);
#ifdef QGC_GST_STREAMING
    videoSourceList.append(videoSourceRTSP);
#ifndef NO_UDP_VIDEO
    videoSourceList.append(videoSourceUDPH264);
    videoSourceList.append(videoSourceUDPH265);
#endif
    videoSourceList.append(videoSourceTCP);
    videoSourceList.append(videoSourceMPEGTS);
    //videoSourceList.append(videoSourceAuto);
#endif
#ifndef QGC_DISABLE_UVC
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo: cameras) {
        videoSourceList.append(cameraInfo.description());
    }
#endif
    if (videoSourceList.count() == 0) {
        _noVideo = true;
        videoSourceList.append(videoSourceNoVideo);
    } else {
        videoSourceList.insert(0, videoSourceAuto);
    }
    QVariantList videoSourceVarList;
    for (const QString& videoSource: videoSourceList) {
        videoSourceVarList.append(QVariant::fromValue(videoSource));
    }

    _videoShareSettings = new WifiSettings();
    _nameToMetaDataMap[videoSourceName]->setEnumInfo(videoSourceList, videoSourceVarList);
    // Set default value for videoSource
    _setDefaults();
}

bool VideoSettings::setVideoShareEnabled(bool enabled)
{
#ifdef __android__
    if(!_videoShareSettings->setVideoShareApEnabled(enabled)) {
        qWarning() << "Set wifi AP hotspot failed.";
        return false;
    }

    QAndroidJniObject prop = QAndroidJniObject::fromString(VIDEO_SHARE_PROP_NAME);
    QAndroidJniObject value;
    if(enabled) {
        value = QAndroidJniObject::fromString("1");
    } else {
        value = QAndroidJniObject::fromString("0");
    }
    QAndroidJniObject::callStaticObjectMethod("android/os/SystemProperties", "set", "(Ljava/lang/String;Ljava/lang/String;)V",
                                                                                prop.object<jstring>(), value.object<jstring>());
    qDebug() << "Andoid property" << prop.toString() << "is be set to" << value.toString();
#endif
    return true;
}

void VideoSettings::_setDefaults()
{
    if (_noVideo) {
        _nameToMetaDataMap[videoSourceName]->setRawDefaultValue(videoSourceNoVideo);
    } else {
        _nameToMetaDataMap[videoSourceName]->setRawDefaultValue(videoDisabled);
    }
}

DECLARE_SETTINGSFACT(VideoSettings, aspectRatio)
DECLARE_SETTINGSFACT(VideoSettings, videoFit)
DECLARE_SETTINGSFACT(VideoSettings, gridLines)
DECLARE_SETTINGSFACT(VideoSettings, showRecControl)
DECLARE_SETTINGSFACT(VideoSettings, recordingFormat)
DECLARE_SETTINGSFACT(VideoSettings, maxVideoSize)
DECLARE_SETTINGSFACT(VideoSettings, enableStorageLimit)
DECLARE_SETTINGSFACT(VideoSettings, rtspTimeout)
DECLARE_SETTINGSFACT(VideoSettings, streamEnabled)
DECLARE_SETTINGSFACT(VideoSettings, disableWhenDisarmed)
DECLARE_SETTINGSFACT(VideoSettings, lowLatencyMode)

DECLARE_SETTINGSFACT_NO_FUNC(VideoSettings, videoSource)
{
    if (!_videoSourceFact) {
        _videoSourceFact = _createSettingsFact(videoSourceName);
        //-- Check for sources no longer available
        if(!_videoSourceFact->enumStrings().contains(_videoSourceFact->rawValue().toString())) {
            if (_noVideo) {
                _videoSourceFact->setRawValue(videoSourceNoVideo);
            } else {
                _videoSourceFact->setRawValue(videoDisabled);
            }
        }
        connect(_videoSourceFact, &Fact::valueChanged, this, &VideoSettings::_configChanged);
    }
    return _videoSourceFact;
}

DECLARE_SETTINGSFACT_NO_FUNC(VideoSettings, udpPort)
{
    if (!_udpPortFact) {
        _udpPortFact = _createSettingsFact(udpPortName);
        connect(_udpPortFact, &Fact::valueChanged, this, &VideoSettings::_configChanged);
    }
    return _udpPortFact;
}

DECLARE_SETTINGSFACT_NO_FUNC(VideoSettings, rtspUrl)
{
    if (!_rtspUrlFact) {
        _rtspUrlFact = _createSettingsFact(rtspUrlName);
        connect(_rtspUrlFact, &Fact::valueChanged, this, &VideoSettings::_configChanged);
    }
    return _rtspUrlFact;
}

DECLARE_SETTINGSFACT_NO_FUNC(VideoSettings, tcpUrl)
{
    if (!_tcpUrlFact) {
        _tcpUrlFact = _createSettingsFact(tcpUrlName);
        connect(_tcpUrlFact, &Fact::valueChanged, this, &VideoSettings::_configChanged);
    }
    return _tcpUrlFact;
}
DECLARE_SETTINGSFACT_NO_FUNC(VideoSettings, videoResolution)
{
    if (!_videoResolutionFact) {
        _videoResolutionFact = _createSettingsFact(videoResolutionName);
        connect(_videoResolutionFact, &Fact::valueChanged, this, &VideoSettings::_configChanged);
    }
    return _videoResolutionFact;
}

DECLARE_SETTINGSFACT_NO_FUNC(VideoSettings, cameraId)
{
    if (!_cameraIdFact) {
        _cameraIdFact = _createSettingsFact(cameraIdName);
        connect(_cameraIdFact, &Fact::valueChanged, this, &VideoSettings::_configChanged);
    }
    return _cameraIdFact;
}

DECLARE_SETTINGSFACT_NO_FUNC(VideoSettings, videoShareEnable)
{
    if (!_videoShareEnableFact) {
        _videoShareEnableFact = _createSettingsFact(videoShareEnableName);
        connect(_videoShareEnableFact, &Fact::valueChanged, this, &VideoSettings::_configChanged);
#ifdef __android__
        //Set video share state by property
        QAndroidJniObject prop = QAndroidJniObject::fromString(VIDEO_SHARE_PROP_NAME);
        QAndroidJniObject defaultValue = QAndroidJniObject::fromString("0");
        QAndroidJniObject value = QAndroidJniObject::callStaticObjectMethod("android/os/SystemProperties", "get",
                                    "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", prop.object<jstring>(), defaultValue.object<jstring>());
        _videoShareEnableFact->setRawValue(value.toString().toInt());
        if(_videoShareEnableFact->rawValue().toBool()) {
            _videoShareSettings->setVideoShareApEnabled(true);
        }
#endif
    }
    return _videoShareEnableFact;
}


bool VideoSettings::streamConfigured(void)
{
#if !defined(QGC_GST_STREAMING)
    return false;
#endif
    //-- First, check if it's autoconfigured
    if(qgcApp()->toolbox()->videoManager()->autoStreamConfigured()) {
        qCDebug(VideoManagerLog) << "Stream auto configured";
        return true;
    }
    //-- Check if it's disabled
    QString vSource = videoSource()->rawValue().toString();
    if(vSource == videoSourceNoVideo || vSource == videoDisabled) {
        return false;
    }
    //-- If UDP, check if port is set
    if(vSource == videoSourceUDPH264 || vSource == videoSourceUDPH265) {
        qCDebug(VideoManagerLog) << "Testing configuration for UDP Stream:" << udpPort()->rawValue().toInt();
        return udpPort()->rawValue().toInt() != 0;
    }
    //-- If RTSP, check for URL
    if(vSource == videoSourceRTSP) {
        qCDebug(VideoManagerLog) << "Testing configuration for RTSP Stream:" << rtspUrl()->rawValue().toString();
        return !rtspUrl()->rawValue().toString().isEmpty();
    }
    //-- If TCP, check for URL
    if(vSource == videoSourceTCP) {
        qCDebug(VideoManagerLog) << "Testing configuration for TCP Stream:" << tcpUrl()->rawValue().toString();
        return !tcpUrl()->rawValue().toString().isEmpty();
    }
    //-- If MPEG-TS, check if port is set
    if(vSource == videoSourceMPEGTS) {
        qCDebug(VideoManagerLog) << "Testing configuration for MPEG-TS Stream:" << udpPort()->rawValue().toInt();
        return udpPort()->rawValue().toInt() != 0;
    }
    if(vSource == videoSourceAuto) {
        return true;
    }
    return false;
}

void VideoSettings::_configChanged(QVariant)
{
    emit streamConfiguredChanged();
}
