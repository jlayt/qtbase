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

#include <qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    Private
    Default C locale formatter
*/

QDateTimeFormatterPrivate::QDateTimeFormatterPrivate(QLocale::DateTimeStyle style, const QLocaleCode &locale)
    : m_style(style),
      m_locale(locale)
{
    if (style == QLocale::DefaultDateTimeStyle)
        m_style = QLocale::MediumStyle;
}

QDateTimeFormatterPrivate::QDateTimeFormatterPrivate(const QString date, const QString time, const QLocaleCode &locale)
    : m_style(QLocale::MediumStyle),
      m_locale(locale)
{
    Q_UNUSED(date)
    Q_UNUSED(time)
}

QDateTimeFormatterPrivate::QDateTimeFormatterPrivate(const QDateTimeFormatterPrivate &other)
    : QSharedData(other),
      m_style(other.m_style),
      m_locale(other.m_locale)
{
}

QDateTimeFormatterPrivate::~QDateTimeFormatterPrivate()
{
}

QDateTimeFormatterPrivate *QDateTimeFormatterPrivate::clone()
{
    return new QDateTimeFormatterPrivate(*this);
}

bool QDateTimeFormatterPrivate::isValid(QMetaType::Type type) const
{
    Q_UNUSED(type)
    return true;
}

QLocaleCode QDateTimeFormatterPrivate::locale() const
{
    return m_locale;
}

QLocale::DateTimeStyle QDateTimeFormatterPrivate::style() const
{
    return m_style;
}

QList<QLocale::DateTimeStyle> QDateTimeFormatterPrivate::availableStyles()
{
    QList<QLocale::DateTimeStyle> list;
    list << QLocale::LongStyle << QLocale::MediumStyle;
    return list;
}

QString QDateTimeFormatterPrivate::datePattern() const
{
    //TODO Get the right value
    return QString();
}

QString QDateTimeFormatterPrivate::timePattern() const
{
    //TODO Get the right value
    return QString();
}

QString QDateTimeFormatterPrivate::dateTimePattern() const
{
    //TODO Get the right value
    return QString();
}

void QDateTimeFormatterPrivate::setDatePattern(const QString &pattern)
{
    Q_UNUSED(pattern)
}

void QDateTimeFormatterPrivate::setTimePattern(const QString &pattern)
{
    Q_UNUSED(pattern)
}

void QDateTimeFormatterPrivate::setDateTimePattern(const QString &pattern)
{
    Q_UNUSED(pattern)
}

qint32 QDateTimeFormatterPrivate::shortYearWindowStart() const
{
    return 0;
}

void QDateTimeFormatterPrivate::setShortYearWindowStart(qint32 year)
{
    Q_UNUSED(year)
}

QDateTimeFormatter::ParsingMode QDateTimeFormatterPrivate::parsingMode() const
{
    return QDateTimeFormatter::DefaultParsingMode;
}

void QDateTimeFormatterPrivate::setParsingMode(QDateTimeFormatter::ParsingMode mode)
{
    Q_UNUSED(mode)
}

QString QDateTimeFormatterPrivate::toString(const QDate &date) const
{
    Q_UNUSED(date)
    return QString();
}

QString QDateTimeFormatterPrivate::toString(const QTime &time) const
{
    Q_UNUSED(time)
    return QString();
}

QString QDateTimeFormatterPrivate::toString(const QDateTime &dateTime) const
{
    Q_UNUSED(dateTime)
    return QString();
}

QString QDateTimeFormatterPrivate::toString(const QDate &date, const QString &pattern) const
{
    Q_UNUSED(date)
    Q_UNUSED(pattern)
    return QString();
}

QString QDateTimeFormatterPrivate::toString(const QTime &time, const QString &pattern) const
{
    Q_UNUSED(time)
    Q_UNUSED(pattern)
    return QString();
}

QString QDateTimeFormatterPrivate::toString(const QDateTime &dateTime, const QString &pattern) const
{
    Q_UNUSED(dateTime)
    Q_UNUSED(pattern)
    return QString();
}

QDate QDateTimeFormatterPrivate::toDate(const QString &from,  QDateTimeFormatter::ParsingMode mode) const
{
    Q_UNUSED(from)
    Q_UNUSED(mode)
    return QDate();
}

QTime QDateTimeFormatterPrivate::toTime(const QString &from,  QDateTimeFormatter::ParsingMode mode) const
{
    Q_UNUSED(from)
    Q_UNUSED(mode)
    return QTime();
}

QDateTime QDateTimeFormatterPrivate::toDateTime(const QString &from,  QDateTimeFormatter::ParsingMode mode) const
{
    Q_UNUSED(from)
    Q_UNUSED(mode)
    return QDateTime();
}

QDate QDateTimeFormatterPrivate::toDate(const QString &from, const QString &pattern,
                                        QDateTimeFormatter::ParsingMode mode) const
{
    Q_UNUSED(from)
    Q_UNUSED(pattern)
    Q_UNUSED(mode)
    return QDate();
}

QTime QDateTimeFormatterPrivate::toTime(const QString &from, const QString &pattern,
                                        QDateTimeFormatter::ParsingMode mode) const
{
    Q_UNUSED(from)
    Q_UNUSED(pattern)
    Q_UNUSED(mode)
    return QTime();
}

QDateTime QDateTimeFormatterPrivate::toDateTime(const QString &from, const QString &pattern,
                                                QDateTimeFormatter::ParsingMode mode) const
{
    Q_UNUSED(from)
    Q_UNUSED(pattern)
    Q_UNUSED(mode)
    return QDateTime();
}

QString QDateTimeFormatterPrivate::eraName(int era, QLocale::ComponentStyle style,
                                           QLocale::ComponentContext context) const

{
    Q_UNUSED(era)
    Q_UNUSED(style)
    Q_UNUSED(context)
    return QString();
}

QString QDateTimeFormatterPrivate::quarterName(int quarter, QLocale::ComponentStyle style,
                                               QLocale::ComponentContext context) const
{
    Q_UNUSED(quarter)
    Q_UNUSED(style)
    Q_UNUSED(context)
    return QString();
}

QString QDateTimeFormatterPrivate::monthName(int month, QLocale::ComponentStyle style,
                                             QLocale::ComponentContext context) const
{
    Q_UNUSED(month)
    Q_UNUSED(style)
    Q_UNUSED(context)
    return QString();
}

QString QDateTimeFormatterPrivate::weekdayName(int day, QLocale::ComponentStyle style,
                                               QLocale::ComponentContext context) const
{
    Q_UNUSED(day)
    Q_UNUSED(style)
    Q_UNUSED(context)
    return QString();
}

QString QDateTimeFormatterPrivate::dayPeriodName(const QTime &time, QLocale::ComponentStyle style,
                                                 QLocale::ComponentContext context) const
{
    Q_UNUSED(time)
    Q_UNUSED(style)
    Q_UNUSED(context)
    return QString();
}

// Define template for derived classes to reimplement so QSharedDataPointer clone() works correctly
template<> QDateTimeFormatterPrivate *QSharedDataPointer<QDateTimeFormatterPrivate>::clone()
{
    return d->clone();
}
