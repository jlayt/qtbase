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

QT_BEGIN_NAMESPACE

// Singleton class to hold global default Decimal Number Formatter for efficiency
class QDateTimeFormatterSingleton
{
public:
    QDateTimeFormatterSingleton() { backend = QDateTimeFormatterPrivate(QDateTimeFormatter::DefaultDateTimeStyle,
                                                                    QString()); }

    QSharedDataPointer<QDateTimeFormatterPrivate> backend;
};

Q_GLOBAL_STATIC(QDateTimeFormatterSingleton, global_dtf);

QDateTimeFormatter::QDateTimeFormatter()
                : d(global_dtf.backend))
{
}

QDateTimeFormatter::QDateTimeFormatter(QDateTimeFormatter::DateTimeStyle style, const QString &localeCode)
                : d(QDateTimeFormatterPrivate(style, QString(), localeCode))
{
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

QString QDateTimeFormatter::localeCode(QLocale::LocaleCodeType style) const
{
    return d->localeCode(style);
}

QDateTimeFormatter::DateTimeStyle QDateTimeFormatter::style() const
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

QLocale::ParsingMode QDateTimeFormatter::parsingMode() const
{
    if (d->isValid(QMetaType::QDateTime))
        return d->parsingMode();
    else
        return QLocale::StrictParsing;
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

QDate QDateTimeFormatter::toDate(const QString &from, QLocale::ParsingMode mode) const
{
    if (d->isValid(QMetaType::QDate))
        return d->toDate(from, mode);
    else
        return QDate();
}

QTime QDateTimeFormatter::toTime(const QString &from, QLocale::ParsingMode mode) const
{
    if (d->isValid(QMetaType::QTime))
        return d->toTime(from, mode);
    else
        return QTime();
}

QDateTime QDateTimeFormatter::toDateTime(const QString &from, QLocale::ParsingMode mode) const
{
    if (d->isValid(QMetaType::QDateTime))
        return d->toDateTime(from, mode);
    else
        return QDateTime();
}

QDate QDateTimeFormatter::toDate(const QString &from, const QString &pattern,
                                 QLocale::ParsingMode mode) const
{
    if (d->isValid(QMetaType::QDate))
        return d->toDate(from, pattern, mode);
    else
        return QDate();
}

QTime QDateTimeFormatter::toTime(const QString &from, const QString &pattern,
                                 QLocale::ParsingMode mode) const
{
    if (d->isValid(QMetaType::QTime))
        return d->toTime(from, pattern, mode);
    else
        return QTime();
}

QDateTime QDateTimeFormatter::toDateTime(const QString &from, const QString &pattern,
                                         QLocale::ParsingMode mode) const
{
    if (d->isValid(QMetaType::QDateTime))
        return d->toDateTime(from, pattern, mode);
    else
        return QDateTime();
}

QString QDateTimeFormatter::eraName(int era, QDateTimeFormatter::ComponentStyle style,
                                    QDateTimeFormatter::ComponentContext context) const

{
    if (isValid())
        return d->eraName(era, style, context);
    else
        return QString();
}

QString QDateTimeFormatter::quarterName(int quarter, QDateTimeFormatter::ComponentStyle style,
                                        QDateTimeFormatter::ComponentContext context) const
{
    if (isValid())
        return d->quarterName(quarter, style, context);
    else
        return QString();
}

QString QDateTimeFormatter::monthName(int month, QDateTimeFormatter::ComponentStyle style,
                                      QDateTimeFormatter::ComponentContext context) const
{
    if (isValid())
        return d->monthName(month, style, context);
    else
        return QString();
}

QString QDateTimeFormatter::weekdayName(int day, QDateTimeFormatter::ComponentStyle style,
                                        QDateTimeFormatter::ComponentContext context) const
{
    if (isValid())
        return d->weekdayName(day, style, context);
    else
        return QString();
}

QString QDateTimeFormatter::dayPeriodName(const QTime &time, QDateTimeFormatter::ComponentStyle style,
                                          QDateTimeFormatter::ComponentContext context) const
{
    if (isValid())
        return d->dayPeriodName(time, style, context);
    else
        return QString();
}

QCustomDateTimeFormatter::QCustomDateTimeFormatter(QDateTimeFormatter::DateTimeStyle style,
                                                   const QString &localeCode)
                        : QDateTimeFormatter(*new QDateTimeFormatterPrivate(style, localeCode))
{
}

QCustomDateTimeFormatter::QCustomDateTimeFormatter(const QCustomDateTimeFormatter &other)
{
    // TODO clone formatters in d so can be safely mutated
    d = other.d;
}

QCustomDateTimeFormatter::~QCustomDateTimeFormatter()
{
}

QCustomDateTimeFormatter &QCustomDateTimeFormatter::operator=(const QCustomDateTimeFormatter &other)
{
    // TODO clone formatters!
    d = other.d;
    return *this;
}

void QCustomDateTimeFormatter::setDatePattern(const QString & pattern)
{
    if (d->isValid(QMetaType::QDate))
        d->setDatePattern(pattern);
}

void QCustomDateTimeFormatter::setTimePattern(const QString & pattern)
{
    if (d->isValid(QMetaType::QTime))
        d->setTimePattern(pattern);
}

void QCustomDateTimeFormatter::setDateTimePattern(const QString & pattern)
{
    if (d->isValid(QMetaType::QDateTime))
        d->setDateTimePattern(pattern);
}

void QCustomDateTimeFormatter::setShortYearWindowStart(qint32 year)
{
    if (d->isValid())
        d->setShortYearWindowStart(year);
}

void QCustomDateTimeFormatter::setParsingMode(QLocale::ParsingMode mode)
{
    if (d->isValid())
        d->setParsingMode(mode);
}
