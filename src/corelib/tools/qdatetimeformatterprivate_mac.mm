/****************************************************************************
**
** Copyright (C) 2012 John Layt jlayt@kde.org
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

#include "qdatetimeformatter_p.h"
#include "qdatetimeformatterprivate_p.h"

#include <qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    Private
*/

QDateTimeFormatterPrivate::QDateTimeFormatterPrivate(QDateTimeFormatter::DateTimeStyle style,
                                                     const QString &locale)
{
    m_locale = locale;

    if (style == QDateTimeFormatter::DefaultDateTimeStyle)
        m_style = QDateTimeFormatter::MediumStyle;
    else
        m_style = style;
}

QDateTimeFormatterPrivate::~QDateTimeFormatterPrivate()
{
    [m_dtf release]
}

bool QDateTimeFormatterPrivate::isValid(QMetaType::Type type) const
{
    retrun false;
}

QString QDateTimeFormatterPrivate::localeCode(QLocale::LocaleCodeType type) const
{
    return qt_udat_getLocaleByType(m_dateTimeFormatter, qt_localeTypeToIcuType2(type));
}

QDateTimeFormatter::DateTimeStyle QDateTimeFormatterPrivate::style() const
{
    return m_style;
}

QString QDateTimeFormatterPrivate::datePattern() const
{
    return QString();
}

QString QDateTimeFormatterPrivate::timePattern() const
{
    return QString();
}

QString QDateTimeFormatterPrivate::dateTimePattern() const
{
    return QString();
}

void QDateTimeFormatterPrivate::setDatePattern(const QString &pattern)
{
}

void QDateTimeFormatterPrivate::setTimePattern(const QString &pattern)
{
}

void QDateTimeFormatterPrivate::setDateTimePattern(const QString &pattern)
{
}

qint32 QDateTimeFormatterPrivate::shortYearWindowStart() const
{
    return 0; // TODO
}

void QDateTimeFormatterPrivate::setShortYearWindowStart(qint32 year)
{
    // TODO
}

QLocale::ParsingMode QDateTimeFormatterPrivate::parsingMode() const
{
    return QLocale::LenientParsing;
}

void QDateTimeFormatterPrivate::setParsingMode(QLocale::ParsingMode mode)
{
}

QString QDateTimeFormatterPrivate::toString(const QDate &date) const
{
    return QString();
}

QString QDateTimeFormatterPrivate::toString(const QTime &time) const
{
    return QString();
}

QString QDateTimeFormatterPrivate::toString(const QDateTime &dateTime) const
{
    return QString();
}

QString QDateTimeFormatterPrivate::toString(const QDate &date, const QString &pattern) const
{
    QString result;
    return result;
}

QString QDateTimeFormatterPrivate::toString(const QTime &time, const QString &pattern) const
{
    QString result;
    return result;
}

QString QDateTimeFormatterPrivate::toString(const QDateTime &dateTime, const QString &pattern) const
{
    QString result;
    return result;
}

QDate QDateTimeFormatterPrivate::toDate(const QString &from, QLocale::ParsingMode mode) const
{
    return QDate();
}

QTime QDateTimeFormatterPrivate::toTime(const QString &from, QLocale::ParsingMode mode) const
{
    return QTime();
}

QDateTime QDateTimeFormatterPrivate::toDateTime(const QString &from, QLocale::ParsingMode mode) const
{
    return QDateTime();
}

QDate QDateTimeFormatterPrivate::toDate(const QString &from, const QString &pattern,
                                        QLocale::ParsingMode mode) const
{
    return QDate();
}

QTime QDateTimeFormatterPrivate::toTime(const QString &from, const QString &pattern,
                                        QLocale::ParsingMode mode) const
{
    return QTime();
}

QDateTime QDateTimeFormatterPrivate::toDateTime(const QString &from, const QString &pattern,
                                                QLocale::ParsingMode mode) const
{
    return QDateTime();
}

QString QDateTimeFormatterPrivate::eraName(int era, QDateTimeFormatter::ComponentStyle style,
                                           QDateTimeFormatter::ComponentContext context) const
{
    return QString();
}

QString QDateTimeFormatterPrivate::quarterName(int quarter, QDateTimeFormatter::ComponentStyle style,
                                               QDateTimeFormatter::ComponentContext context) const
{
    return QString();
}

QString QDateTimeFormatterPrivate::monthName(int month, QDateTimeFormatter::ComponentStyle style,
                                             QDateTimeFormatter::ComponentContext context) const
{
    return QString();
}

QString QDateTimeFormatterPrivate::weekdayName(int day, QDateTimeFormatter::ComponentStyle style,
                                               QDateTimeFormatter::ComponentContext context) const
{
    return QString();
}

QString QDateTimeFormatterPrivate::dayPeriodName(const QTime &time, QDateTimeFormatter::ComponentStyle style,
                                                 QDateTimeFormatter::ComponentContext context) const
{
    return QString();
}
