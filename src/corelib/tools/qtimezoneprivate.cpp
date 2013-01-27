/****************************************************************************
**
** Copyright (C) 2013 John Layt <jlayt@kde.org>
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qtimezone.h"
#include "qtimezoneprivate_p.h"
#include "qtimezoneprivate_data_p.h"

#include <qdebug.h>

QT_BEGIN_NAMESPACE

/*
    Static utilities for looking up Windows ID tables
*/

static const int windowsDataTableSize = sizeof(windowsDataTable) / sizeof(QWindowsData) - 1;
static const int zoneDataTableSize = sizeof(zoneDataTable) / sizeof(QZoneData) - 1;

static const QZoneData *zoneData(quint16 index)
{
    Q_ASSERT(index < zoneDataTableSize);
    return &zoneDataTable[index];
}

static const QWindowsData *windowsData(quint16 index)
{
    Q_ASSERT(index < windowsDataTableSize);
    return &windowsDataTable[index];
}

// Return the Windows ID literal for a given QWindowsData
static QByteArray windowsId(const QWindowsData *windowsData)
{
    return (windowsIdData + windowsData->windowsIdIndex);
}

// Return the Olsen ID literal for a given QWindowsData
static QByteArray olsenId(const QWindowsData *windowsData)
{
    return (olsenIdData + windowsData->olsenIdIndex);
}

// Return the Olsen ID literal for a given QZoneData
static QByteArray olsenId(const QZoneData *zoneData)
{
    return (olsenIdData + zoneData->olsenIdIndex);
}

static quint16 toWindowsIdKey(const QByteArray &winId)
{
    for (quint16 i = 0; i < windowsDataTableSize; ++i) {
        const QWindowsData *data = windowsData(i);
        if (windowsId(data) == winId)
            return data->windowsIdKey;
    }
    return 0;
}

static QByteArray toWindowsIdLiteral(quint16 windowsIdKey)
{
    for (quint16 i = 0; i < windowsDataTableSize; ++i) {
        const QWindowsData *data = windowsData(i);
        if (data->windowsIdKey == windowsIdKey)
            return windowsId(data);
    }
    return QByteArray();
}

/*
    Base class implementing common utility routines, only intantiate for a null tz.
*/

QTimeZonePrivate::QTimeZonePrivate()
{
}

QTimeZonePrivate::QTimeZonePrivate(const QTimeZonePrivate &other)
                : QSharedData(other), m_id(other.m_id)
{
}

QTimeZonePrivate::~QTimeZonePrivate()
{
}

QTimeZonePrivate *QTimeZonePrivate::clone()
{
    return new QTimeZonePrivate(*this);
}

bool QTimeZonePrivate::operator==(const QTimeZonePrivate &other) const
{
    // TODO Too simple, but need to solve problem of comparing different derived classes
    // Should work for all System and ICU classes as names guaranteed unique, but not for Simple.
    // Perhaps once all classes have working transitions can compare full list?
    return (m_id == other.m_id);
}

bool QTimeZonePrivate::operator!=(const QTimeZonePrivate &other) const
{
    return !(*this == other);
}

bool QTimeZonePrivate::isValid() const
{
    return !m_id.isEmpty();
}

QByteArray QTimeZonePrivate::id() const
{
    return m_id;
}

QLocale::Country QTimeZonePrivate::country() const
{
    // Default fall-back mode, use the zoneTable to find Region of known Zones
    for (int i = 0; i < zoneDataTableSize; ++i) {
        const QZoneData *data = zoneData(i);
        if (olsenId(data).split(' ').contains(m_id))
            return (QLocale::Country)data->country;
    }
    return QLocale::AnyCountry;
}

QString QTimeZonePrivate::comment() const
{
    return QString();
}

QString QTimeZonePrivate::displayName(qint64 atMSecsSinceEpoch,
                                      QTimeZone::NameType nameType,
                                      const QLocale &locale) const
{
    if (nameType == QTimeZone::OffsetName)
        return isoOffsetFormat(offsetFromUTC(atMSecsSinceEpoch));

    if (isDaylightTime(atMSecsSinceEpoch))
        return displayName(QTimeZone::DaylightTime, nameType, locale);
    else
        return displayName(QTimeZone::StandardTime, nameType, locale);
}

QString QTimeZonePrivate::displayName(QTimeZone::TimeType timeType,
                                      QTimeZone::NameType nameType,
                                      const QLocale &locale) const
{
    Q_UNUSED(timeType)
    Q_UNUSED(nameType)
    Q_UNUSED(locale)
    return QString();
}

QString QTimeZonePrivate::abbreviation(qint64 atMSecsSinceEpoch) const
{
    Q_UNUSED(atMSecsSinceEpoch)
    return QString();
}

int QTimeZonePrivate::offsetFromUTC(qint64 atMSecsSinceEpoch) const
{
    return standardTimeOffset(atMSecsSinceEpoch) + daylightTimeOffset(atMSecsSinceEpoch);
}

int QTimeZonePrivate::standardTimeOffset(qint64 atMSecsSinceEpoch) const
{
    Q_UNUSED(atMSecsSinceEpoch)
    return invalidSeconds();
}

int QTimeZonePrivate::daylightTimeOffset(qint64 atMSecsSinceEpoch) const
{
    Q_UNUSED(atMSecsSinceEpoch)
    return invalidSeconds();
}

bool QTimeZonePrivate::hasDaylightTime() const
{
    return false;
}

bool QTimeZonePrivate::isDaylightTime(qint64 atMSecsSinceEpoch) const
{
    Q_UNUSED(atMSecsSinceEpoch)
    return false;
}

QTimeZonePrivate::Data QTimeZonePrivate::data(qint64 forMSecsSinceEpoch) const
{
    Q_UNUSED(forMSecsSinceEpoch)
    return invalidData();
}

bool QTimeZonePrivate::hasTransitions() const
{
    return false;
}

QTimeZonePrivate::Data QTimeZonePrivate::nextTransition(qint64 afterMSecsSinceEpoch) const
{
    Q_UNUSED(afterMSecsSinceEpoch)
    return invalidData();
}

QTimeZonePrivate::Data QTimeZonePrivate::previousTransition(qint64 beforeMSecsSinceEpoch) const
{
    Q_UNUSED(beforeMSecsSinceEpoch)
    return invalidData();
}

QTimeZonePrivate::DataList QTimeZonePrivate::transitions(qint64 fromMSecsSinceEpoch,
                                                         qint64 toMSecsSinceEpoch) const
{
    DataList list;
    if (toMSecsSinceEpoch > fromMSecsSinceEpoch) {
        // fromMSecsSinceEpoch is inclusive but nextTransitionTime() is exclusive so go back 1 msec
        Data next = nextTransition(fromMSecsSinceEpoch - 1);
        while (next.atMSecsSinceEpoch <= toMSecsSinceEpoch) {
            list.append(next);
            next = nextTransition(next.atMSecsSinceEpoch);
        }
    }
    return list;
}

QByteArray QTimeZonePrivate::systemTimeZoneId() const
{
    return QByteArray();
}

QSet<QByteArray> QTimeZonePrivate::availableTimeZoneIds() const
{
    return QSet<QByteArray>();
}

QSet<QByteArray> QTimeZonePrivate::availableTimeZoneIds(QLocale::Country country) const
{
    // Default fall-back mode, use the zoneTable to find Region of know Zones
    QSet<QByteArray> regionSet;

    // First get all Zones in the Zones table belonging to the Region
    for (int i = 0; i < zoneDataTableSize; ++i) {
        if (zoneData(i)->country == country)
            regionSet += olsenId(zoneData(i)).split(' ').toSet();
    }

    // Then select just those that are available
    QSet<QByteArray> set;
    foreach (const QByteArray &olsenId, availableTimeZoneIds()) {
        if (regionSet.contains(olsenId))
            set << olsenId;
    }

    return set;
}

QSet<QByteArray> QTimeZonePrivate::availableTimeZoneIds(int offsetFromUtc) const
{
    // Default fall-back mode, use the zoneTable to find Offset of know Zones
    QSet<QByteArray> offsetSet;
    // First get all Zones in the table using the Offset
    for (int i = 0; i < windowsDataTableSize; ++i) {
        const QWindowsData *winData = windowsData(i);
        if (winData->offsetFromUtc == offsetFromUtc) {
            for (int j = 0; j < zoneDataTableSize; ++j) {
                const QZoneData *data = zoneData(j);
                if (data->windowsIdKey == winData->windowsIdKey)
                    offsetSet += olsenId(data).split(' ').toSet();
            }
        }
    }

    // Then select just those that are available
    QSet<QByteArray> set;
    foreach (const QByteArray &olsenId, availableTimeZoneIds()) {
        if (offsetSet.contains(olsenId))
            set << olsenId;
    }

    return set;
}

#ifndef QT_NO_DATASTREAM
void QTimeZonePrivate::serialize(QDataStream &ds) const
{
    ds << QString::fromUtf8(m_id);
}
#endif // QT_NO_DATASTREAM

// Static Utility Methods

QTimeZonePrivate::Data QTimeZonePrivate::invalidData()
{
    Data data;
    data.atMSecsSinceEpoch = invalidMSecs();
    data.offsetFromUTC = invalidSeconds();
    data.standardTimeOffset = invalidSeconds();
    data.daylightTimeOffset = invalidSeconds();
    return data;
}

QTimeZone::OffsetData QTimeZonePrivate::invalidOffsetData()
{
    QTimeZone::OffsetData offsetData;
    offsetData.atUTC = QDateTime();
    offsetData.offsetFromUTC = invalidSeconds();
    offsetData.standardTimeOffset = invalidSeconds();
    offsetData.daylightTimeOffset = invalidSeconds();
    return offsetData;
}

QTimeZone::OffsetData QTimeZonePrivate::toOffsetData(const QTimeZonePrivate::Data &data)
{
    QTimeZone::OffsetData offsetData = invalidOffsetData();
    if (data.atMSecsSinceEpoch != invalidMSecs()) {
        offsetData.atUTC = QDateTime::fromMSecsSinceEpoch(data.atMSecsSinceEpoch, Qt::UTC);
        offsetData.offsetFromUTC = data.offsetFromUTC;
        offsetData.standardTimeOffset = data.standardTimeOffset;
        offsetData.daylightTimeOffset = data.daylightTimeOffset;
        offsetData.abbreviation = data.abbreviation;
    }
    return offsetData;
}

// If the format of the ID is valid
bool QTimeZonePrivate::isValidId(const QByteArray &olsenId)
{
    // Rules for defining TZ/Olsen names as per ftp://ftp.iana.org/tz/code/Theory
    // * Use only valid POSIX file name components
    // * Within a file name component, use only ASCII letters, `.', `-' and `_'.
    // * Do not use digits
    // * A file name component must not exceed 14 characters or start with `-'
    // Aliases such as "Etc/GMT+7" and "SystemV/EST5EDT" are valid so we need to accept digits
    if (olsenId.contains(' '))
        return false;
    QList<QByteArray> parts = olsenId.split('\\');
    foreach (const QByteArray &part, parts) {
        if (part.size() > 14)
            return false;
        if (part.at(0) == '-')
            return false;
        for (int i = 0; i < part.size(); ++i) {
            QChar ch = part.at(i);
            if (!(ch >= 'a' && ch <= 'z')
                && !(ch >= 'A' && ch <= 'Z')
                && !(ch == '_')
                && !(ch >= '0' && ch <= '9')
                && !(ch == '-')
                && !(ch == '.'))
                return false;
        }
    }
    return true;
}

QString QTimeZonePrivate::isoOffsetFormat(int offsetFromUtc)
{
    const int mins = offsetFromUtc / 60;
    return QString::fromUtf8("UTC%1%2:%3").arg(mins >= 0 ? QLatin1Char('+') : QLatin1Char('-'))
                                          .arg(qAbs(mins) / 60, 2, 10, QLatin1Char('0'))
                                          .arg(qAbs(mins) % 60, 2, 10, QLatin1Char('0'));
}

QByteArray QTimeZonePrivate::olsenIdToWindowsId(const QByteArray &id)
{
    for (int i = 0; i < zoneDataTableSize; ++i) {
        const QZoneData *data = zoneData(i);
        if (olsenId(data).split(' ').contains(id))
            return toWindowsIdLiteral(data->windowsIdKey);
    }
    return QByteArray();
}

QByteArray QTimeZonePrivate::windowsIdToDefaultOlsenId(const QByteArray &windowsId)
{
    const quint16 windowsIdKey = toWindowsIdKey(windowsId);
    for (int i = 0; i < windowsDataTableSize; ++i) {
        const QWindowsData *data = windowsData(i);
        if (data->windowsIdKey == windowsIdKey)
            return olsenId(data);
    }
    return QByteArray();
}

QByteArray QTimeZonePrivate::windowsIdToDefaultOlsenId(const QByteArray &windowsId,
                                                       QLocale::Country country)
{
    const QList<QByteArray> list = windowsIdToOlsenIds(windowsId, country);
    if (list.count() > 0)
        return list.first();
    else
        return QByteArray();
}

QList<QByteArray> QTimeZonePrivate::windowsIdToOlsenIds(const QByteArray &windowsId)
{
    const quint16 windowsIdKey = toWindowsIdKey(windowsId);
    QList<QByteArray> list;

    for (int i = 0; i < zoneDataTableSize; ++i) {
        const QZoneData *data = zoneData(i);
        if (data->windowsIdKey == windowsIdKey)
            list << olsenId(data).split(' ');
    }

    // Return the full list in alpha order
    qSort(list);
    return list;
}

QList<QByteArray> QTimeZonePrivate::windowsIdToOlsenIds(const QByteArray &windowsId,
                                                        QLocale::Country country)
{
    const quint16 windowsIdKey = toWindowsIdKey(windowsId);
    for (int i = 0; i < zoneDataTableSize; ++i) {
        const QZoneData *data = zoneData(i);
        if (data->windowsIdKey == windowsIdKey && data->country == (quint16) country)
                // Return the region matches in preference order
                return olsenId(data).split(' ');
    }

    return QList<QByteArray>();
}

// Define template for derived classes to reimplement so QSharedDataPointer clone() works correctly
template<> QTimeZonePrivate *QSharedDataPointer<QTimeZonePrivate>::clone()
{
    return d->clone();
}

QT_END_NAMESPACE
