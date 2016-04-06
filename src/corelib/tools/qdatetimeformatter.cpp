/****************************************************************************
**
** Copyright (C) 2016 John Layt jlayt@kde.org
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdatetimeformatter.h"
#include "qdatetimeformatterprivate_p.h"

QT_BEGIN_NAMESPACE

// Create Date Time Formatter using appropriate platform backend
static QDateTimeFormatterPrivate *newBackendNumberFormatter(QLocale::DateTimeStyle style, const QLocaleCode &localeCode)
{
    return new QDateTimeFormatterPrivate(style, localeCode);
}

static QDateTimeFormatterPrivate *newBackendPatternNumberFormatter(const QString date, const QString time, const QLocaleCode &localeCode)
{
    return new QDateTimeFormatterPrivate(date, time, localeCode);
}

// Singleton class to hold global default Date Time Formatter for efficiency
class QDateTimeFormatterSingleton
{
public:
    QDateTimeFormatterSingleton() { backend = newBackendNumberFormatter(QLocale::MediumStyle, QLocaleCode()); }

    QSharedDataPointer<QDateTimeFormatterPrivate> backend;
};

// App default Date Time Formatter, able to be set by app to override global_system_cf
Q_GLOBAL_STATIC(QDateTimeFormatterSingleton, global_dtf);

// System Date Time Formatter, used for available styles, etc, never changed by app
Q_GLOBAL_STATIC(QDateTimeFormatterSingleton, global_system_dtf);

static QDateTimeFormatterPrivate *defaultDateTimeFormatter()
{
    if (global_dtf)
        return global_dtf->backend;
    return global_system_dtf->backend;
}


QDateTimeFormatter::QDateTimeFormatter()
{
    d = defaultDateTimeFormatter();
}

QDateTimeFormatter::QDateTimeFormatter(QDateTimeFormatterPrivate &dd)
                  : d(&dd)
{
}

QDateTimeFormatter::QDateTimeFormatter(const QDateTimeFormatter &other)
{
    d = other.d;
}

QDateTimeFormatter::~QDateTimeFormatter()
{
}

QDateTimeFormatter &QDateTimeFormatter::operator=(const QDateTimeFormatter &other)
{
    d = other.d;
    return *this;
}

bool QDateTimeFormatter::isValid() const
{
    return d->isValid(QMetaType::QDateTime);
}

QLocaleCode QDateTimeFormatter::locale() const
{
    return d->locale();
}

QLocale::DateTimeStyle QDateTimeFormatter::style() const
{
    return d->style();
}

QString QDateTimeFormatter::datePattern() const
{
    if (d->isValid(QMetaType::QDate))
        return d->datePattern();
    else
        return QString();
}

QString QDateTimeFormatter::timePattern() const
{
    if (d->isValid(QMetaType::QTime))
        return d->timePattern();
    else
        return QString();
}

QString QDateTimeFormatter::dateTimePattern() const
{
    if (d->isValid(QMetaType::QDateTime))
        return d->dateTimePattern();
    else
        return QString();
}

qint32 QDateTimeFormatter::shortYearWindowStart() const
{
    if (d->isValid(QMetaType::QDateTime))
        return d->shortYearWindowStart();
    else
        return -1;
}

QDateTimeFormatter::ParsingMode QDateTimeFormatter::parsingMode() const
{
    if (d->isValid(QMetaType::QDateTime))
        return d->parsingMode();
    else
        return QDateTimeFormatter::DefaultParsingMode;
}

QString QDateTimeFormatter::toString(const QDate &date) const
{
    if (d->isValid(QMetaType::QDate))
        return d->toString(date);
    else
        return QString();
}

QString QDateTimeFormatter::toString(const QTime &time) const
{
    if (d->isValid(QMetaType::QTime))
        return d->toString(time);
    else
        return QString();
}

QString QDateTimeFormatter::toString(const QDateTime &dateTime) const
{
    if (d->isValid(QMetaType::QDateTime))
        return d->toString(dateTime);
    else
        return QString();
}

QString QDateTimeFormatter::toString(const QDate &date, const QString &pattern) const
{
    if (d->isValid(QMetaType::QDate))
        return d->toString(date, pattern);
    else
        return QString();
}

QString QDateTimeFormatter::toString(const QTime &time, const QString &pattern) const
{
    if (d->isValid(QMetaType::QTime))
        return d->toString(time, pattern);
    else
        return QString();
}

QString QDateTimeFormatter::toString(const QDateTime &dateTime, const QString &pattern) const
{
    if (d->isValid(QMetaType::QDateTime))
        return d->toString(dateTime, pattern);
    else
        return QString();
}

QDate QDateTimeFormatter::toDate(const QString &from, QDateTimeFormatter::ParsingMode mode) const
{
    if (d->isValid(QMetaType::QDate))
        return d->toDate(from, mode);
    else
        return QDate();
}

QTime QDateTimeFormatter::toTime(const QString &from, QDateTimeFormatter::ParsingMode mode) const
{
    if (d->isValid(QMetaType::QTime))
        return d->toTime(from, mode);
    else
        return QTime();
}

QDateTime QDateTimeFormatter::toDateTime(const QString &from, QDateTimeFormatter::ParsingMode mode) const
{
    if (d->isValid(QMetaType::QDateTime))
        return d->toDateTime(from, mode);
    else
        return QDateTime();
}

QDate QDateTimeFormatter::toDate(const QString &from, const QString &pattern, QDateTimeFormatter::ParsingMode mode) const
{
    if (d->isValid(QMetaType::QDate))
        return d->toDate(from, pattern, mode);
    else
        return QDate();
}

QTime QDateTimeFormatter::toTime(const QString &from, const QString &pattern, QDateTimeFormatter::ParsingMode mode) const
{
    if (d->isValid(QMetaType::QTime))
        return d->toTime(from, pattern, mode);
    else
        return QTime();
}

QDateTime QDateTimeFormatter::toDateTime(const QString &from, const QString &pattern, QDateTimeFormatter::ParsingMode mode) const
{
    if (d->isValid(QMetaType::QDateTime))
        return d->toDateTime(from, pattern, mode);
    else
        return QDateTime();
}

QString QDateTimeFormatter::eraName(int era, QLocale::ComponentStyle style,
                                    QLocale::ComponentContext context) const

{
    if (isValid())
        return d->eraName(era, style, context);
    else
        return QString();
}

QString QDateTimeFormatter::quarterName(int quarter, QLocale::ComponentStyle style,
                                        QLocale::ComponentContext context) const
{
    if (isValid())
        return d->quarterName(quarter, style, context);
    else
        return QString();
}

QString QDateTimeFormatter::monthName(int month, QLocale::ComponentStyle style,
                                      QLocale::ComponentContext context) const
{
    if (isValid())
        return d->monthName(month, style, context);
    else
        return QString();
}

QString QDateTimeFormatter::weekdayName(int day, QLocale::ComponentStyle style,
                                        QLocale::ComponentContext context) const
{
    if (isValid())
        return d->weekdayName(day, style, context);
    else
        return QString();
}

QString QDateTimeFormatter::dayPeriodName(const QTime &time, QLocale::ComponentStyle style,
                                          QLocale::ComponentContext context) const
{
    if (isValid())
        return d->dayPeriodName(time, style, context);
    else
        return QString();
}

QCustomDateTimeFormatter::QCustomDateTimeFormatter(QLocale::DateTimeStyle style, const QLocaleCode &locale)
    : QDateTimeFormatter(*newBackendNumberFormatter(style, locale))
{
}

QCustomDateTimeFormatter::QCustomDateTimeFormatter(QDateTimeFormatterPrivate &dd)
{
    d = &dd;
}

QCustomDateTimeFormatter::QCustomDateTimeFormatter(const QCustomDateTimeFormatter &other)
    : QDateTimeFormatter(other)
{
}

QCustomDateTimeFormatter::~QCustomDateTimeFormatter()
{
}

QCustomDateTimeFormatter &QCustomDateTimeFormatter::operator=(const QCustomDateTimeFormatter &other)
{
    d = other.d;
    return *this;
}

void QCustomDateTimeFormatter::setShortYearWindowStart(qint32 year)
{
    if (d->isValid())
        d->setShortYearWindowStart(year);
}

void QCustomDateTimeFormatter::setParsingMode(QDateTimeFormatter::ParsingMode mode)
{
    if (d->isValid())
        d->setParsingMode(mode);
}

QPatternDateTimeFormatter::QPatternDateTimeFormatter(const QString &date, const QString & time, const QLocaleCode &locale)
    : QDateTimeFormatter(*newBackendPatternNumberFormatter(date, time, locale))
{
}

QPatternDateTimeFormatter::QPatternDateTimeFormatter(QDateTimeFormatterPrivate &dd)
{
    d = &dd;
}

QPatternDateTimeFormatter::QPatternDateTimeFormatter(const QPatternDateTimeFormatter &other)
    : QDateTimeFormatter(other)
{
}

QPatternDateTimeFormatter::~QPatternDateTimeFormatter()
{
}

QPatternDateTimeFormatter &QPatternDateTimeFormatter::operator=(const QPatternDateTimeFormatter &other)
{
    d = other.d;
    return *this;
}

void QPatternDateTimeFormatter::setDatePattern(const QString & pattern)
{
    if (d->isValid(QMetaType::QDate))
        d->setDatePattern(pattern);
}

void QPatternDateTimeFormatter::setTimePattern(const QString & pattern)
{
    if (d->isValid(QMetaType::QTime))
        d->setTimePattern(pattern);
}

void QPatternDateTimeFormatter::setDateTimePattern(const QString & pattern)
{
    if (d->isValid(QMetaType::QDateTime))
        d->setDateTimePattern(pattern);
}

void QPatternDateTimeFormatter::setShortYearWindowStart(qint32 year)
{
    if (d->isValid())
        d->setShortYearWindowStart(year);
}

void QPatternDateTimeFormatter::setParsingMode(QDateTimeFormatter::ParsingMode mode)
{
    if (d->isValid())
        d->setParsingMode(mode);
}
