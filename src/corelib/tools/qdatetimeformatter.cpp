/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdatetimeformatter_p.h"

#include "qplatformdefs.h"
#include "qstring.h"
#include "qlocale.h"
#include "qdatetime.h"
#include "qdatecalculator.h"
#include "qdebug.h"

QT_BEGIN_NAMESPACE

#ifndef QT_BOOTSTRAPPED

#ifdef QT4_COMPAT
static bool timeFormatContainsAP(const QString &format)
{
    int i = 0;
    while (i < format.size()) {
        if (format.at(i).unicode() == '\'') {
            qt_readEscapedFormatString(format, &i);
            continue;
        }

        if (format.at(i).toLower().unicode() == 'a')
            return true;

        ++i;
    }
    return false;
}
#endif

QString qt_readEscapedFormatString(const QString &format, int *idx)
{
    int &i = *idx;

    Q_ASSERT(format.at(i) == QLatin1Char('\''));
    ++i;
    if (i == format.size())
        return QString();
    if (format.at(i).unicode() == '\'') { // "''" outside of a quoted stirng
        ++i;
        return QLatin1String("'");
    }

    QString result;

    while (i < format.size()) {
        if (format.at(i).unicode() == '\'') {
            if (i + 1 < format.size() && format.at(i + 1).unicode() == '\'') {
                // "''" inside of a quoted string
                result.append(QLatin1Char('\''));
                i += 2;
            } else {
                break;
            }
        } else {
            result.append(format.at(i++));
        }
    }
    if (i < format.size())
        ++i;

    return result;
}

int qt_repeatCount(const QString &s, int i)
{
    QChar c = s.at(i);
    int j = i + 1;
    while (j < s.size() && s.at(j) == c)
        ++j;
    return j - i;
}

static QString zeroPad(long value, int width)
{
    if ( value < 0 )
        return QString::number(abs(value)).rightJustified(width, QLatin1Char('0')).prepend(QLatin1Char('-'));
    else
        return QString::number(value).rightJustified(width, QLatin1Char('0'));
}

static QString timeZone()
{
#if defined(Q_OS_WINCE)
    TIME_ZONE_INFORMATION info;
    DWORD res = GetTimeZoneInformation(&info);
    if (res == TIME_ZONE_ID_UNKNOWN)
        return QString();
    return QString::fromWCharArray(info.StandardName);
#elif defined(Q_OS_WIN)
    _tzset();
# if defined(_MSC_VER) && _MSC_VER >= 1400
    size_t returnSize = 0;
    char timeZoneName[512];
    if (_get_tzname(&returnSize, timeZoneName, 512, 1))
        return QString();
    return QString::fromLocal8Bit(timeZoneName);
# else
    return QString::fromLocal8Bit(_tzname[1]);
# endif
#elif defined(Q_OS_VXWORKS)
    return QString();
#else
    tzset();
    return QString::fromLocal8Bit(tzname[1]);
#endif
}

QString QDateTimeFormatter::dateTimeToString(const QString &format,
                                             const QDate *date,
                                             const QTime *time,
                                             const QLocale *q)
{
    Q_ASSERT(date || time);
    if ((date && !q->calendar().isValid(*date)) || (time && !time->isValid()))
        return QString();

#ifdef QT4_COMPAT
    const bool format_am_pm = time && timeFormatContainsAP(format);

    enum { AM, PM } am_pm = AM;
    int hour12 = time ? time->hour() : -1;
    if (time) {
        if (hour12 == 0) {
            am_pm = AM;
            hour12 = 12;
        } else if (hour12 < 12) {
            am_pm = AM;
        } else if (hour12 == 12) {
            am_pm = PM;
        } else {
            am_pm = PM;
            hour12 -= 12;
        }
    }
#endif

    QString result;

    int i = 0;
    while (i < format.size()) {
        if (format.at(i).unicode() == '\'') {
            result.append(qt_readEscapedFormatString(format, &i));
            continue;
        }

        const QChar c = format.at(i);
        int repeat = qt_repeatCount(format, i);
        bool used = false;
        if (date) {
            switch (c.unicode()) {
            // Year 1..n
            case 'y': {
                used = true;
#ifdef QT4_COMPAT
                // Year 2,4
                if (repeat >= 4)
                    repeat = 4;
                else if (repeat >= 2)
                    repeat = 2;
                switch (repeat) {
                case 4:
                    result.append(zeroPad(date->year(), 4));
                    break;
                case 2:
                    result.append(zeroPad(date->year() % 100, 2));
                    break;
                default:
                    repeat = 1;
                    result.append(c);
                    break;
                }
                break;
#else
                int year = q->calendar().year(*date);
                if (repeat == 2)
                    year = year % 100;
                result.append(zeroPad(year, repeat));
                break;
#endif
            }

            // Week Year 1..n
            case 'Y': {
                used = true;
                int weekYear = 0;
                q->calendar().weekNumber(*date, &weekYear);
                if (repeat == 2)
                    weekYear = weekYear % 100;
                result.append(zeroPad(weekYear, repeat));
                break;
            }

            // Quarter 1..4
            case 'Q': {
                used = true;
                repeat = qMin(repeat, 4);
                int quarter = q->calendar().quarter(*date);
                switch (repeat) {
                case 1:
                    result.append(QString::number(quarter));
                    break;
                case 2:
                    result.append(zeroPad(quarter, repeat));
                    break;
                case 3:
                    result.append(q->quarterName(quarter, QLocale::ShortName));
                    break;
                case 4:
                    result.append(q->quarterName(quarter, QLocale::LongName));
                    break;
                }
                break;
            }

            // Quarter Standalone 1..4
            case 'q': {
                used = true;
                repeat = qMin(repeat, 4);
                int quarter = q->calendar().quarter(*date);
                switch (repeat) {
                case 1:
                    result.append(QString::number(quarter));
                    break;
                case 2:
                    result.append(zeroPad(quarter, repeat));
                    break;
                case 3:
                    result.append(q->quarterName(quarter, QLocale::ShortName, QLocale::StandaloneContext));
                    break;
                case 4:
                    result.append(q->quarterName(quarter, QLocale::LongName, QLocale::StandaloneContext));
                    break;
                }
                break;
            }

            // Month 1..5
            case 'M': {
                used = true;
                repeat = qMin(repeat, 5);
                int month = q->calendar().month(*date);
                switch (repeat) {
                case 1:
                    result.append(QString::number(month));
                    break;
                case 2:
                    result.append(zeroPad(month, repeat));
                    break;
                case 3:
                    result.append(q->monthName(month, QLocale::ShortName));
                    break;
                case 4:
                    result.append(q->monthName(month, QLocale::LongName));
                    break;
                case 5:
                    result.append(q->monthName(month, QLocale::NarrowName));
                    break;
                }
                break;
            }

            // Month Standalone 1..5
            case 'L': {
                used = true;
                repeat = qMin(repeat, 5);
                int month = q->calendar().month(*date);
                switch (repeat) {
                case 1:
                    result.append(QString::number(month));
                    break;
                case 2:
                    result.append(zeroPad(month, repeat));
                    break;
                case 3:
                    result.append(q->monthName(month, QLocale::ShortName, QLocale::StandaloneContext));
                    break;
                case 4:
                    result.append(q->monthName(month, QLocale::LongName, QLocale::StandaloneContext));
                    break;
                case 5:
                    result.append(q->monthName(month, QLocale::NarrowName, QLocale::StandaloneContext));
                    break;
                }
                break;
            }

            // Week 1..2
            case 'w':
                used = true;
                repeat = qMin(repeat, 2);
                result.append(zeroPad(q->calendar().weekNumber(*date, 0), repeat));
                break;

            // Day 1..2
            case 'd': {
                used = true;
#ifdef QT4_COMPAT
                repeat = qMin(repeat, 4);
#else
                repeat = qMin(repeat, 2);
#endif
                int day = q->calendar().day(*date);
                switch (repeat) {
                case 1:
                    result.append(QString::number(day));
                    break;
                case 2:
                    result.append(zeroPad(day, repeat));
                    break;
#ifdef QT4_COMPAT
                case 3:
                    result.append(q->dayName(day, QLocale::ShortName));
                    break;
                case 4:
                    result.append(q->dayName(day, QLocale::LongName));
                    break;
#endif
                }
                break;
            }

            // Day of Year 1..3
            case 'D':
                used = true;
                repeat = qMin(repeat, 3);
                result.append(zeroPad(q->calendar().dayOfYear(*date), repeat));
                break;

            // Day of Week 1..5
            case 'E': {
                used = true;
                repeat = qMin(repeat, 5);
                int dayOfWeek = q->calendar().dayOfWeek(*date);
                switch (repeat) {
                case 1:
                case 2:
                case 3:
                    result.append(q->dayName(dayOfWeek, QLocale::ShortName));
                    break;
                case 4:
                    result.append(q->dayName(dayOfWeek, QLocale::LongName));
                    break;
                case 5:
                    result.append(q->dayName(dayOfWeek, QLocale::NarrowName));
                    break;
                }
                break;
            }

            // Local Day of Week 1..5
            case 'e': {
                used = true;
                repeat = qMin(repeat, 5);
                int dayOfWeek = q->calendar().dayOfWeek(*date);
                switch (repeat) {
                case 1:
                    //TODO This may need localising based on firstDayOfWeek()???
                    result.append(QString::number(dayOfWeek));
                    break;
                case 2:
                    //TODO This may need localising based on firstDayOfWeek()???
                    result.append(zeroPad(dayOfWeek, repeat));
                    break;
                case 3:
                    result.append(q->dayName(dayOfWeek, QLocale::ShortName));
                    break;
                case 4:
                    result.append(q->dayName(dayOfWeek, QLocale::LongName));
                    break;
                case 5:
                    result.append(q->dayName(dayOfWeek, QLocale::NarrowName));
                    break;
                }
                break;
            }

            // Local Day of Week Standalone 1..5
            case 'c': {
                used = true;
                repeat = qMin(repeat, 5);
                int dayOfWeek = q->calendar().dayOfWeek(*date);
                switch (repeat) {
                case 1:
                    result.append(QString::number(dayOfWeek));
                    break;
                case 2:
                    //TODO This may need localising based on firstDayOfWeek()???
                    result.append(zeroPad(dayOfWeek, repeat));
                    break;
                case 3:
                    result.append(q->dayName(dayOfWeek, QLocale::ShortName, QLocale::StandaloneContext));
                    break;
                case 4:
                    result.append(q->dayName(dayOfWeek, QLocale::LongName, QLocale::StandaloneContext));
                    break;
                case 5:
                    result.append(q->dayName(dayOfWeek, QLocale::NarrowName, QLocale::StandaloneContext));
                    break;
                }
                break;
            }

            default:
                break;
            }
        }

        if (!used && time) {
            switch (c.unicode()) {
            // Hour 1 to 12 1..2
            case 'h': {
                used = true;
                repeat = qMin(repeat, 2);
#ifdef QT4_COMPAT
                int hour = format_am_pm ? hour12 : time->hour();
#else
                int hour = time->hour();
                if (hour > 12)
                    hour -= 12;
                else if (hour == 0)
                    hour = 12;
#endif
                switch (repeat) {
                case 1:
                    result.append(QString::number(hour));
                    break;
                case 2:
                    result.append(zeroPad(hour, 2));
                    break;
                }
                break;
            }

            // Hour 0 to 23 1..2
            case 'H': {
                used = true;
                repeat = qMin(repeat, 2);
                switch (repeat) {
                case 1:
                    result.append(QString::number(time->hour()));
                    break;
                case 2:
                    result.append(zeroPad(time->hour(), 2));
                    break;
                }
                break;
            }

            // Hour 0 to 11 1..2
            case 'K': {
                used = true;
                repeat = qMin(repeat, 2);
                int hour = time->hour();
                if (hour > 11)
                    hour = hour - 12;
                switch (repeat) {
                case 1:
                    result.append(QString::number(hour));
                    break;
                case 2:
                    result.append(zeroPad(hour, 2));
                    break;
                }
                break;
            }

            // Hour 1 to 24 1..2
            case 'k': {
                used = true;
                repeat = qMin(repeat, 2);
                int hour = time->hour() + 1;
                switch (repeat) {
                case 1:
                    result.append(QString::number(hour));
                    break;
                case 2:
                    result.append(zeroPad(hour, 2));
                    break;
                }
                break;
            }

            // Minute 1..2
            case 'm': {
                used = true;
                repeat = qMin(repeat, 2);
                switch (repeat) {
                case 1:
                    result.append(QString::number(time->minute()));
                    break;
                case 2:
                    result.append(zeroPad(time->minute(), 2));
                    break;
                }
                break;
            }

            // Second 1..2
            case 's': {
                used = true;
                repeat = qMin(repeat, 2);
                switch (repeat) {
                case 1:
                    result.append(QString::number(time->second()));
                    break;
                case 2:
                    result.append(zeroPad(time->second(), 2));
                    break;
                }
                break;
            }

            // Fractional seconds 1..n
            case 'S': {
                used = true;
                QString frac = zeroPad(time->msec(), 3);
                if (repeat > 3)
                    frac = frac.leftJustified(repeat, QLatin1Char('0'), true);
                else if (repeat < 3)
                    frac = frac.left(repeat);
                //TODO Find if leading 0's get stripped
                result.append(frac);
                break;
            }

#ifdef QT4_COMPAT
            // AM/PM Lowercase 1..2 a or ap
            case 'a':
                used = true;
                if (i + 1 < format.length() && format.at(i + 1).unicode() == 'p') {
                    repeat = 2;
                } else {
                    repeat = 1;
                }
                result.append(q->dayPeriodName(time).toLower());
                break;

            // AM/PM Uppercase 1..2 A or AP
            case 'A':
                used = true;
                if (i + 1 < format.length() && format.at(i + 1).unicode() == 'P') {
                    repeat = 2;
                } else {
                    repeat = 1;
                }
                result.append(q->dayPeriodName(time).toUpper());
                break;
#else
            // AM/PM 1
            case 'a':
                used = true;
                repeat = qMin(repeat, 1);
                //TODO Confirm is LongName not ShortName
                result.append(q->dayPeriodName(*time));
                break;

#endif

#ifdef QT4_COMPAT
            // Milliseconds 1..3, replaced by SSS
            case 'z': {
                used = true;
                if (repeat >= 3) {
                    repeat = 3;
                } else {
                    repeat = 1;
                }
                switch (repeat) {
                case 1:
                    result.append(QString::number(time->msec()));
                    break;
                case 3:
                    result.append(zeroPad(time->msec(), 3));
                    break;
                }
                break;
            }
#else
            // Time Zone 1..4
            //TODO Proper support to come with new QTimeZone
            case 'z':
                used = true;
                repeat = qMin(repeat, 4);
                result.append(timeZone());
                break;
#endif

            // Time Zone 1..4
            //TODO Proper support to come with new QTimeZone
            case 'Z':
                used = true;
                repeat = qMin(repeat, 4);
                result.append(timeZone());
                break;

            // Time Zone 1,2,4
            //TODO Proper support to come with new QTimeZone
            case 'v':
            case 'V':
                used = true;
                if (repeat < 4)
                    repeat = qMin(repeat, 1);
                else
                    repeat = 4;
                result.append(timeZone());
                break;

#ifdef QT4_COMPAT
            // Time Zone 1
            case 't':
                used = true;
                repeat = 1;
                result.append(timeZone());
                break;
#endif

            default:
                break;
            }
        }
        if (!used) {
            result.append(QString(repeat, c));
        }
        i += repeat;
    }

    return result;
}

#endif // QT_BOOTSTRAPPED

QT_END_NAMESPACE
