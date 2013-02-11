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

#ifndef QTIMEZONE_H
#define QTIMEZONE_H

#include <QtCore/qsharedpointer.h>
#include <QtCore/qlocale.h>
#include <QtCore/qdatetime.h>

QT_BEGIN_NAMESPACE

class QTimeZonePrivate;

class Q_CORE_EXPORT QTimeZone
{
public:
    enum TimeType {
        StandardTime = 0,
        DaylightTime = 1,
        GenericTime = 2
    };

    enum NameType {
        DefaultName = 0,
        LongName = 1,
        ShortName = 2,
        OffsetName = 3
    };

    struct OffsetData {
        QDateTime atUTC;
        int offsetFromUTC;
        int standardTimeOffset;
        int daylightTimeOffset;
        QString abbreviation;
    };
    typedef QList<OffsetData> OffsetDataList;

    QTimeZone();
    explicit QTimeZone(const QByteArray &olsenId);
    QTimeZone(qint32 offsetSeconds);
    QTimeZone(const QByteArray &zoneId, int offsetSeconds, const QString &name,
              const QString &abbreviation, QLocale::Country country = QLocale::AnyCountry,
              const QString &comment = QString());
    QTimeZone(const QTimeZone &other);
    ~QTimeZone();

    QTimeZone &operator=(const QTimeZone &other);

    bool operator==(const QTimeZone &other) const;
    bool operator!=(const QTimeZone &other) const;

    bool isValid() const;

    QByteArray id() const;
    QLocale::Country country() const;
    QString comment() const;

    QString displayName(const QDateTime &atDateTime,
                        QTimeZone::NameType nameType = QTimeZone::DefaultName,
                        const QLocale &locale = QLocale()) const;
    QString displayName(QTimeZone::TimeType timeType,
                        QTimeZone::NameType nameType = QTimeZone::DefaultName,
                        const QLocale &locale = QLocale()) const;
    QString abbreviation(const QDateTime &atDateTime) const;

    int offsetFromUTC(const QDateTime &atDateTime) const;
    int standardTimeOffset(const QDateTime &atDateTime) const;
    int daylightTimeOffset(const QDateTime &atDateTime) const;

    bool hasDaylightTime() const;
    bool isDaylightTime(const QDateTime &atDateTime) const;

    OffsetData offsetData(const QDateTime &forDateTime) const;

    bool hasTransitions() const;
    OffsetData nextTransition(const QDateTime &afterDateTime) const;
    OffsetData previousTransition(const QDateTime &beforeDateTime) const;
    OffsetDataList transitions(const QDateTime &fromDateTime, const QDateTime &toDateTime) const;

    static QByteArray systemTimeZoneId();

    static bool isTimeZoneIdAvailable(const QByteArray &olsenId);

    static QList<QByteArray> availableTimeZoneIds();
    static QList<QByteArray> availableTimeZoneIds(QLocale::Country country);
    static QList<QByteArray> availableTimeZoneIds(int offsetSeconds);

    static QByteArray olsenIdToWindowsId(const QByteArray &olsenId);
    static QByteArray windowsIdToDefaultOlsenId(const QByteArray &windowsId);
    static QByteArray windowsIdToDefaultOlsenId(const QByteArray &windowsId,
                                                QLocale::Country country);
    static QList<QByteArray> windowsIdToOlsenIds(const QByteArray &windowsId);
    static QList<QByteArray> windowsIdToOlsenIds(const QByteArray &windowsId,
                                                 QLocale::Country country);

private:
    QTimeZone(QTimeZonePrivate *dd);
#ifndef QT_NO_DATASTREAM
    friend Q_CORE_EXPORT QDataStream &operator<<(QDataStream &ds, const QTimeZone &tz);
#endif
    friend class QTimeZonePrivate;
    friend class QDateTime;
    friend class QDateTimePrivate;
    QSharedDataPointer<QTimeZonePrivate> d;
};

Q_DECLARE_TYPEINFO(QTimeZone::OffsetData, Q_MOVABLE_TYPE);

#ifndef QT_NO_DATASTREAM
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &ds, const QTimeZone &tz);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &ds, QTimeZone &tz);
#endif

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug dbg, const QTimeZone &tz);
#endif

QT_END_NAMESPACE

#endif // QTIMEZONE_H
