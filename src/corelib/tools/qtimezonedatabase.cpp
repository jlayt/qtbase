/****************************************************************************
**
** Copyright (C) 2014 John Layt <jlayt@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qtimezonedatabase.h"
#include "qtimezoneprivate_p.h"

#include <QtCore/qdatetime.h>

#include <qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QTimeZoneDatabase
    \inmodule QtCore
    \since 5.3
    \brief The QTimeZoneDatabase class provides a database of available IANA time zones.

    \threadsafe

    This class provides access to a database of IANA time zones, either the system
    database on platforms that ship a native IANA database, such as Linux and Mac,
    or a user supplied database.  This class will be most useful for applications
    on Windows that wish to use their own copy of the IANA database rather than the
    native Windows time zone database.  QTimeZone objects created from this database
    can then be used in the creation of QDateTime objects.

    The QTimeZoneDatabase instance must be created with the path to the IANA database
    root directory that contains the zone.tab file listing all the available time zones
    and all the region subdirectories of time zone files.  The time zone files themselves
    must be in the compiled IANA file format as produced by the zic utility.

    No caching is implemented in this class, it is left to the application to decide on
    the best caching policy to use in their circumstances.  This means repeated requests
    to a QTimeZoneDatabase instance for the same IANA ID will lead to the file being
    reloaded from disk every time.
*/

/*!
    Create a null/invalid time zone database.
*/

QTimeZoneDatabase::QTimeZoneDatabase()
    : d(0)
{
}

/*!
    Creates an instance of the time zone database found at the
    \a databasePath.

    The path must hold the zone.tab file listing the available times
    zones and the region subdirectories for all the time zone files
    included in the database.

    \sa systemDatabasePath()
*/

QTimeZoneDatabase::QTimeZoneDatabase(const QString &databasePath)
    : d(new QTimeZoneDatabasePrivate(databasePath))
{
}

/*!
    Copy constructor, copy \a other to this.
*/

QTimeZoneDatabase::QTimeZoneDatabase(const QTimeZoneDatabase &other)
    : d(other.d)
{
}

/*!
    Destroys the time zone database.
*/

QTimeZoneDatabase::~QTimeZoneDatabase()
{
}

/*!
    Assignment operator, assign \a other to this.
*/

QTimeZoneDatabase &QTimeZoneDatabase::operator=(const QTimeZoneDatabase &other)
{
    d = other.d;
    return *this;
}

/*!
    \fn QTimeZoneDatabase &QTimeZoneDatabase::operator=(QTimeZoneDatabase &&other)

    Move-assigns \a other to this QTimeZoneDatabase instance, transferring the
    ownership of the managed pointer to this instance.
*/

/*!
    Returns \c true if this time zone database is equal to the \a other time zone.
*/

bool QTimeZoneDatabase::operator==(const QTimeZoneDatabase &other) const
{
    if (d && other.d)
        return (*d == *other.d);
    else
        return (d == other.d);
}

/*!
    Returns \c true if this time zone database is not equal to the \a other
    time zone database.
*/

bool QTimeZoneDatabase::operator!=(const QTimeZoneDatabase &other) const
{
    if (d && other.d)
        return (*d != *other.d);
    else
        return (d != other.d);
}

/*!
    Returns \c true if this time zone database is valid.
*/

bool QTimeZoneDatabase::isValid() const
{
    if (d)
        return d->isValid();
    else
        return false;
}

/*!
    Returns the path of the time zone database.
*/

QString QTimeZoneDatabase::databasePath() const
{
    if (isValid())
        return d->databasePath();
    else
        return QString();
}

/*!
    Creates a QTimeZone instance for the given \a ianaId if it is available in
    this database.

    If the IANA ID is not available in this database then a null QTimeZone will
    be returned.  You should always check the validity of all returned time zone
    instances before using them.

    \sa availableTimeZoneIds()
*/

QTimeZone QTimeZoneDatabase::createTimeZone(const QByteArray &ianaId)
{
    QTimeZone tz;
    if (isValid())
        tz.d =  d->createTimeZonePrivate(ianaId);
    return tz;
}

/*!
    Returns \c true if a given time zone \a ianaId is available in this database.

    \sa availableTimeZoneIds()
*/

bool QTimeZoneDatabase::isTimeZoneIdAvailable(const QByteArray &ianaId)
{
    // isValidId is not strictly required, but faster to weed out invalid
    // IDs as availableTimeZoneIds() may be slow
    return isValid() && QTimeZonePrivate::isValidId(ianaId) && availableTimeZoneIds().contains(ianaId);
}

/*!
    Returns a list of all available IANA time zone IDs in this database.

    \sa isTimeZoneIdAvailable()
*/

QList<QByteArray> QTimeZoneDatabase::availableTimeZoneIds() const
{
    QList<QByteArray> list;
    if (!isValid())
        return list;
    QSet<QByteArray> set = d->availableTimeZoneIds();
    list = set.toList();
    std::sort(list.begin(), list.end());
    return list;
}

/*!
    Returns a list of all available IANA time zone IDs for a given \a country.

    As a special case, a \a country of Qt::AnyCountry returns those time zones
    that do not have any country related to them, such as UTC.  If you require
    a list of all time zone IDs for all countries then use the standard
    availableTimeZoneIds() method.

    \sa isTimeZoneIdAvailable()
*/

QList<QByteArray> QTimeZoneDatabase::availableTimeZoneIds(QLocale::Country country) const
{
    QList<QByteArray> list;
    if (!isValid())
        return list;
    QSet<QByteArray> set = d->availableTimeZoneIds(country);
    list = set.toList();
    std::sort(list.begin(), list.end());
    return list;
}

/*!
    Returns the path to the system IANA database if one exists.

    On Mac, Linux, and other UNIX based systems this is usually /usr/share/zoneinfo.

    On platforms such as Windows where there is no system IANA database this
    path will be empty.
*/
QString QTimeZoneDatabase::systemDatabasePath()
{
    return QTimeZoneDatabasePrivate::systemDatabasePath();
}

/*!
    Returns the system IANA database if one exists.  On platforms such as Windows
    where there is no system IANA database this database instance will be invalid.
*/
QTimeZoneDatabase QTimeZoneDatabase::systemDatabase()
{
    return QTimeZoneDatabase(QTimeZoneDatabase::systemDatabasePath());
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QTimeZoneDatabase &tzdb)
{
    if (tzdb.isValid())
        dbg.nospace() << QStringLiteral("QTimeZoneDatabase(") << tzdb.databasePath() << ')';
    else
        dbg.nospace() << QStringLiteral("QTimeZoneDatabase()");
    return dbg.space();
}
#endif

QT_END_NAMESPACE
