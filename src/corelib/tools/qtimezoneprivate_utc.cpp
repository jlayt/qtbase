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
    Private

    UTC Offset implementation, used when QT_NO_SYSTEMLOCALE set and QT_USE_ICU not set,
    or for QDateTimes with a Qt:Spec of Qt::OffsetFromUtc.
*/

static const int utcDataTableSize = sizeof(utcDataTable) / sizeof(QUtcData) - 1;

static const QUtcData *utcData(quint16 index)
{
    Q_ASSERT(index < utcDataTableSize);
    return &utcDataTable[index];
}

static QByteArray utcId(const QUtcData *utcData)
{
    return (olsenIdData + utcData->olsenIdIndex);
}

// Create default UTC time zone
QUtcTimeZonePrivate::QUtcTimeZonePrivate()
{
    const QString name = QStringLiteral("UTC");
    init(QByteArrayLiteral("UTC"), 0, name, name, QLocale::AnyCountry, name);
}

// Create a named UTC time zone
QUtcTimeZonePrivate::QUtcTimeZonePrivate(const QByteArray &id)
                   : QTimeZonePrivate()
{
    // Look for the name in the UTC list, if found set the values
    for (int i = 0; i < utcDataTableSize; ++i) {
        const QUtcData *data = utcData(i);
        const QByteArray uid = utcId(data);
        if (uid == id) {
            QString name = QString::fromUtf8(id);
            init(id, data->offsetFromUtc, name, name, QLocale::AnyCountry, name);
            break;
        }
    }
}

// Create offset from UTC
QUtcTimeZonePrivate::QUtcTimeZonePrivate(qint32 offsetSeconds)
                   : QTimeZonePrivate()
{
    QString utcId;

    if (offsetSeconds == 0)
        utcId = QStringLiteral("UTC");
    else
        utcId = isoOffsetFormat(offsetSeconds);

    init(utcId.toUtf8(), offsetSeconds, utcId, utcId, QLocale::AnyCountry, utcId);
}

QUtcTimeZonePrivate::QUtcTimeZonePrivate(const QByteArray &zoneId, int offsetSeconds,
                                         const QString &name, const QString &abbreviation,
                                         QLocale::Country country, const QString &comment)
                   : QTimeZonePrivate()
{
    init(zoneId, offsetSeconds, name, abbreviation, country, comment);
}

QUtcTimeZonePrivate::QUtcTimeZonePrivate(const QUtcTimeZonePrivate &other)
                   : QTimeZonePrivate(other), m_offsetFromUtc(other.m_offsetFromUtc), m_name(other.m_name),
                     m_abbreviation(other.m_abbreviation), m_country(other.m_country),
                     m_comment(other.m_comment)
{
}

QUtcTimeZonePrivate::~QUtcTimeZonePrivate()
{
}

QTimeZonePrivate *QUtcTimeZonePrivate::clone()
{
    return new QUtcTimeZonePrivate(*this);
}

void QUtcTimeZonePrivate::init(const QByteArray &zoneId)
{
    m_id = zoneId;
}

void QUtcTimeZonePrivate::init(const QByteArray &zoneId, int offsetSeconds, const QString &name,
                               const QString &abbreviation, QLocale::Country country,
                               const QString &comment)
{
    m_id = zoneId;
    m_offsetFromUtc = offsetSeconds;
    m_name = name;
    m_abbreviation = abbreviation;
    m_country = country;
    m_comment = comment;
}

QLocale::Country QUtcTimeZonePrivate::country() const
{
    return m_country;
}

QString QUtcTimeZonePrivate::comment() const
{
    return m_comment;
}

QString QUtcTimeZonePrivate::displayName(QTimeZone::TimeType timeType,
                                         QTimeZone::NameType nameType,
                                         const QLocale &locale) const
{
    Q_UNUSED(timeType)
    Q_UNUSED(locale)
    if (nameType == QTimeZone::ShortName)
        return m_abbreviation;
    else if (nameType == QTimeZone::OffsetName)
        return isoOffsetFormat(m_offsetFromUtc);
    return m_name;
}

QString QUtcTimeZonePrivate::abbreviation(qint64 atMSecsSinceEpoch) const
{
    Q_UNUSED(atMSecsSinceEpoch)
    return m_abbreviation;
}

qint32 QUtcTimeZonePrivate::standardTimeOffset(qint64 atMSecsSinceEpoch) const
{
    Q_UNUSED(atMSecsSinceEpoch)
    return m_offsetFromUtc;
}

qint32 QUtcTimeZonePrivate::daylightTimeOffset(qint64 atMSecsSinceEpoch) const
{
    Q_UNUSED(atMSecsSinceEpoch)
    return 0;
}

QByteArray QUtcTimeZonePrivate::systemTimeZoneId() const
{
    return QByteArrayLiteral("UTC");
}

QSet<QByteArray> QUtcTimeZonePrivate::availableTimeZoneIds() const
{
    QSet<QByteArray> set;
    for (int i = 0; i < utcDataTableSize; ++i) {
        set << utcId(utcData(i));
    }
    return set;
}

QSet<QByteArray> QUtcTimeZonePrivate::availableTimeZoneIds(QLocale::Country country) const
{
    // If AnyCountry then is request for all non-region offset codes
    if (country == QLocale::AnyCountry)
        return availableTimeZoneIds();
    return QSet<QByteArray>();
}

QSet<QByteArray> QUtcTimeZonePrivate::availableTimeZoneIds(qint32 offsetSeconds) const
{
    QSet<QByteArray> set;
    for (int i = 0; i < utcDataTableSize; ++i) {
        const QUtcData *data = utcData(i);
        if (data->offsetFromUtc == offsetSeconds)
            set << utcId(data);
    }
    return set;
}

#ifndef QT_NO_DATASTREAM
void QUtcTimeZonePrivate::serialize(QDataStream &ds) const
{
    ds << QStringLiteral("OffsetFromUtc") << QString::fromUtf8(m_id) << m_offsetFromUtc << m_name
       << m_abbreviation << (qint32) m_country << m_comment;
}
#endif // QT_NO_DATASTREAM

QT_END_NAMESPACE
