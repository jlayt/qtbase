/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qdatetime.h"
#include "qdatetime_p.h"
#include "qdatetimeparser_p.h"

#include "qplatformdefs.h"
#include "qdatastream.h"
#include "qset.h"
#include "qlocale.h"
#include "qregexp.h"
#include "qstringlist.h"
#include "qdebug.h"

#if defined(Q_OS_WIN32) || defined(Q_OS_WINCE)
#include <qt_windows.h>
#endif
#ifndef Q_OS_WIN
#include <locale.h>
#endif

#include <time.h>
#if defined(Q_OS_WINCE)
#include "qfunctions_wince.h"
#endif

#if defined(Q_WS_MAC)
#include <private/qcore_mac_p.h>
#endif

#if defined(Q_OS_SYMBIAN)
#include <e32std.h>
#endif

QT_BEGIN_NAMESPACE

enum {
    SECS_PER_DAY = 86400,
    MSECS_PER_DAY = 86400000,
    SECS_PER_HOUR = 3600,
    MSECS_PER_HOUR = 3600000,
    SECS_PER_MIN = 60,
    MSECS_PER_MIN = 60000,
    JULIAN_DAY_FOR_EPOCH = 2440588 // result of julianDayFromGregorianDate(1970, 1, 1)
};

static inline qint64 nullJd()
{
    return std::numeric_limits<qint64>::min();
}

static inline QDate fixedDate(int y, int m, int d)
{
    QDate result(y, m, 1);
    result.setDate(y, m, qMin(d, result.daysInMonth()));
    return result;
}

static inline qint64 julianDayFromGregorianDate(int year, int month, int day)
{
    // Gregorian calendar starting from October 15, 1582
    // Algorithm from Henry F. Fliegel and Thomas C. Van Flandern
    return (1461 * (year + 4800 + (month - 14) / 12)) / 4
           + (367 * (month - 2 - 12 * ((month - 14) / 12))) / 12
           - (3 * ((year + 4900 + (month - 14) / 12) / 100)) / 4
           + day - 32075;
}

static qint64 julianDayFromDate(int year, int month, int day)
{
    if (year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day >= 15)))) {
        return julianDayFromGregorianDate(year, month, day);
    } else if (year < 1582 || (year == 1582 && (month < 10 || (month == 10 && day <= 4)))) {
        // Julian calendar until October 4, 1582
        // Algorithm from Frequently Asked Questions about Calendars by Claus Toendering
        if (year < 0)
            ++year;
        int a = (14 - month) / 12;
        return (153 * (month + (12 * a) - 3) + 2) / 5
               + (1461 * (year + 4800 - a)) / 4
               + day - 32083;
    } else {
        // the day following October 4, 1582 is October 15, 1582
        return nullJd();
    }
}

static void getDateFromJulianDay(qint64 julianDay, int *year, int *month, int *day)
{
    int y, m, d;

    if (julianDay >= 2299161) {
        // Gregorian calendar starting from October 15, 1582
        // This algorithm is from Henry F. Fliegel and Thomas C. Van Flandern
        qint64 ell, n, i, j;  //TODO These will need to be bigger to prevent overflow!!!
        ell = julianDay + 68569;
        n = (4 * ell) / 146097;
        ell = ell - (146097 * n + 3) / 4;
        i = (4000 * (ell + 1)) / 1461001;
        ell = ell - (1461 * i) / 4 + 31;
        j = (80 * ell) / 2447;
        d = ell - (2447 * j) / 80;
        ell = j / 11;
        m = j + 2 - (12 * ell);
        y = 100 * (n - 49) + i + ell;
    } else {
        // Julian calendar until October 4, 1582
        // Algorithm from Frequently Asked Questions about Calendars by Claus Toendering
        julianDay += 32082;
        qint64 dd = (4 * julianDay + 3) / 1461;  //TODO These may need to be bigger to prevent overflow!!!
        qint64 ee = julianDay - (1461 * dd) / 4;  //TODO These may need to be bigger to prevent overflow!!!
        qint64 mm = ((5 * ee) + 2) / 153;  //TODO These may need to be bigger to prevent overflow!!!
        d = ee - (153 * mm + 2) / 5 + 1;
        m = mm + 3 - 12 * (mm / 10);
        y = dd - 4800 + (mm / 10);
        if (y <= 0)
            --y;
    }
    if (year)
        *year = y;
    if (month)
        *month = m;
    if (day)
        *day = d;
}


static const char monthDays[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

#ifndef QT_NO_TEXTDATE
static const char * const qt_shortMonthNames[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
#endif
#ifndef QT_NO_DATESTRING
static QString fmtDateTime(const QString& f, const QTime* dt = 0, const QDate* dd = 0);
#endif

/*****************************************************************************
  QDate member functions
 *****************************************************************************/

/*!
    \enum QDate::MonthNameType

    This enum describes the types of the string representation used
    for the month name.

    \value DateFormat This type of name can be used for date-to-string formatting.
    \value StandaloneFormat This type is used when you need to enumerate months or weekdays.
           Usually standalone names are represented in singular forms with
           capitalized first letter.
*/

/*!
    \class QDate
    \reentrant
    \brief The QDate class provides date functions.


    A QDate object contains a calendar date, i.e. year, month, and day
    numbers, in the Gregorian calendar. (see \l{QDate G and J} {Use of
    Gregorian and Julian Calendars} for dates prior to 15 October
    1582). It can read the current date from the system clock. It
    provides functions for comparing dates, and for manipulating
    dates. For example, it is possible to add and subtract days,
    months, and years to dates.

    A QDate object is typically created either by giving the year,
    month, and day numbers explicitly. Note that QDate interprets two
    digit years as is, i.e., years 0 - 99. A QDate can also be
    constructed with the static function currentDate(), which creates
    a QDate object containing the system clock's date.  An explicit
    date can also be set using setDate(). The fromString() function
    returns a QDate given a string and a date format which is used to
    interpret the date within the string.

    The year(), month(), and day() functions provide access to the
    year, month, and day numbers. Also, dayOfWeek() and dayOfYear()
    functions are provided. The same information is provided in
    textual format by the toString(), shortDayName(), longDayName(),
    shortMonthName(), and longMonthName() functions.

    QDate provides a full set of operators to compare two QDate
    objects where smaller means earlier, and larger means later.

    You can increment (or decrement) a date by a given number of days
    using addDays(). Similarly you can use addMonths() and addYears().
    The daysTo() function returns the number of days between two
    dates.

    The daysInMonth() and daysInYear() functions return how many days
    there are in this date's month and year, respectively. The
    isLeapYear() function indicates whether a date is in a leap year.

    \section1

    \target QDate G and J
    \section2 Use of Gregorian and Julian Calendars

    QDate uses the Gregorian calendar in all locales, beginning
    on the date 15 October 1582. For dates up to and including 4
    October 1582, the Julian calendar is used.  This means there is a
    10-day gap in the internal calendar between the 4th and the 15th
    of October 1582. When you use QDateTime for dates in that epoch,
    the day after 4 October 1582 is 15 October 1582, and the dates in
    the gap are invalid.

    The Julian to Gregorian changeover date used here is the date when
    the Gregorian calendar was first introduced, by Pope Gregory
    XIII. That change was not universally accepted and some localities
    only executed it at a later date (if at all).  QDateTime
    doesn't take any of these historical facts into account. If an
    application must support a locale-specific dating system, it must
    do so on its own, remembering to convert the dates using the
    Julian day.

    \section2 No Year 0

    There is no year 0. Dates in that year are considered invalid. The
    year -1 is the year "1 before Christ" or "1 before current era."
    The day before 1 January 1 CE is 31 December 1 BCE.

    \section2 Range of Valid Dates

    Dates are stored internally as a Julian Day number, an interger count of
    every day in a contiguous range, with 1 January 4713 BCE being Julian Day 0.
    As well as being an efficient and accurate way of storing an absolute date,
    it is suitable for converting a Date into other calendar systems such as
    Hebrew, Islamic or Chinese. The Julian Day number can be obtained using
    QDate::toJulianDay() and can be set using QDate::fromJulianDay().

    The range of dates able to be stored by QDate as a Julian Day number is
    from around 5 million BCE to around 5 million CE. The range of Julian Days
    able to be accurately converted to and from valid YMD form Dates is
    restricted to 1 January 4800 BCE to 31 December 1400000 CE due to
    shortcomings in the available conversion formulas. This may change in the
    future.

    \sa QTime, QDateTime, QDateEdit, QDateTimeEdit, QCalendarWidget
*/

/*!
    \fn QDate::QDate()

    Constructs a null date. Null dates are invalid.

    \sa isNull(), isValid()
*/

/*!
    Constructs a date with year \a y, month \a m and day \a d.

    If the specified date is invalid, the date is not set and
    isValid() returns false. A date before 2 January 4713 B.C. is
    considered invalid.

    \sa isValid()
*/

QDate::QDate(int y, int m, int d)
{
    setDate(y, m, d);
}


/*!
    \fn bool QDate::isNull() const

    Returns true if the date is null; otherwise returns false. A null
    date is invalid.

    \note The behavior of this function is equivalent to !isValid().

    \sa isValid()
*/


/*!
    Returns true if this date is valid, i.e. not null; otherwise returns false.

    \note The behavior of this function is equivalent to !isNull().

    \sa isNull()
*/

bool QDate::isValid() const
{
    return !isNull();
}


/*!
    Returns the year of this date. Negative numbers indicate years
    before 1 CE, such that year -44 is 44 BCE.

    Returns 0 if the date is invalid.

    \sa month(), day()
*/

int QDate::year() const
{
    if (isValid()) {
        int y;
        getDateFromJulianDay(jd, &y, 0, 0);
        return y;
    }
    return 0;
}

/*!
    Returns the number corresponding to the month of this date, using
    the following convention:

    \list
    \i 1 = "January"
    \i 2 = "February"
    \i 3 = "March"
    \i 4 = "April"
    \i 5 = "May"
    \i 6 = "June"
    \i 7 = "July"
    \i 8 = "August"
    \i 9 = "September"
    \i 10 = "October"
    \i 11 = "November"
    \i 12 = "December"
    \endlist

    Returns 0 if the date is invalid.

    \sa year(), day()
*/

int QDate::month() const
{
    if (isValid()) {
        int m;
        getDateFromJulianDay(jd, 0, &m, 0);
        return m;
    }
    return 0;
}

/*!
    Returns the day of the month (1 to 31) of this date.

    Returns 0 if the date is invalid.

    \sa year(), month(), dayOfWeek()
*/

int QDate::day() const
{
    if (isValid()) {
        int dd;
        getDateFromJulianDay(jd, 0, 0, &dd);
        return dd;
    }
    return 0;
}

/*!
    Returns the weekday (1 to 7) for this date.

    Returns 0 if the date is invalid.

    \sa day(), dayOfYear(), Qt::DayOfWeek
*/

int QDate::dayOfWeek() const
{
    if (isValid()) {
        if (jd >= 0)
            return (jd % 7) + 1;
        else
            return (jd % 7) + 8;
    }
    return 0;
}

/*!
    Returns the day of the year (1 to 365 or 366 on leap years) for
    this date.

    Returns 0 if the date is invalid.

    \sa day(), dayOfWeek()
*/

int QDate::dayOfYear() const
{
    if (isValid()) {
        return jd - julianDayFromDate(year(), 1, 1) + 1;
    }
    return 0;
}

/*!
    Returns the number of days in the month (28 to 31) for this date.

    Returns 0 if the date is invalid.

    \sa day(), daysInYear()
*/

int QDate::daysInMonth() const
{
    if (isValid()) {
        int y, m;
        getDateFromJulianDay(jd, &y, &m, 0);
        if (m == 2 && isLeapYear(y))
            return 29;
        else if (m < 1 || m > 12)
            return 0;
        else
            return monthDays[m];
    }
    return 0;
}

/*!
    Returns the number of days in the year (365 or 366) for this date.

    Returns 0 if the date is invalid.

    \sa day(), daysInMonth()
*/

int QDate::daysInYear() const
{
    if (isValid()) {
        int y;
        getDateFromJulianDay(jd, &y, 0, 0);
        return isLeapYear(y) ? 366 : 365;
    }
    return 0;
}

/*!
    Returns the week number (1 to 53), and stores the year in
    *\a{yearNumber} unless \a yearNumber is null (the default).

    Returns 0 if the date is invalid.

    In accordance with ISO 8601, weeks start on Monday and the first
    Thursday of a year is always in week 1 of that year. Most years
    have 52 weeks, but some have 53.

    *\a{yearNumber} is not always the same as year(). For example, 1
    January 2000 has week number 52 in the year 1999, and 31 December
    2002 has week number 1 in the year 2003.

    \legalese
    Copyright (c) 1989 The Regents of the University of California.
    All rights reserved.

    Redistribution and use in source and binary forms are permitted
    provided that the above copyright notice and this paragraph are
    duplicated in all such forms and that any documentation,
    advertising materials, and other materials related to such
    distribution and use acknowledge that the software was developed
    by the University of California, Berkeley.  The name of the
    University may not be used to endorse or promote products derived
    from this software without specific prior written permission.
    THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

    \sa isValid()
*/

int QDate::weekNumber(int *yearNumber) const
{
    if (!isValid())
        return 0;

    int year = QDate::year();
    int yday = dayOfYear() - 1;
    int wday = dayOfWeek();
    if (wday == 7)
        wday = 0;
    int w;

    for (;;) {
        int len;
        int bot;
        int top;

        len = isLeapYear(year) ? 366 : 365;
        /*
        ** What yday (-3 ... 3) does
        ** the ISO year begin on?
        */
        bot = ((yday + 11 - wday) % 7) - 3;
        /*
        ** What yday does the NEXT
        ** ISO year begin on?
        */
        top = bot - (len % 7);
        if (top < -3)
            top += 7;
        top += len;
        if (yday >= top) {
            ++year;
            w = 1;
            break;
        }
        if (yday >= bot) {
            w = 1 + ((yday - bot) / 7);
            break;
        }
        --year;
        yday += isLeapYear(year) ? 366 : 365;
    }
    if (yearNumber != 0)
        *yearNumber = year;
    return w;
}

#ifndef QT_NO_TEXTDATE
/*!
    Returns the short name of the \a month for the representation specified
    by \a type.  By default returns the normal type for date formatting.

    The months are enumerated using the following convention:

    \list
    \i 1 = "Jan"
    \i 2 = "Feb"
    \i 3 = "Mar"
    \i 4 = "Apr"
    \i 5 = "May"
    \i 6 = "Jun"
    \i 7 = "Jul"
    \i 8 = "Aug"
    \i 9 = "Sep"
    \i 10 = "Oct"
    \i 11 = "Nov"
    \i 12 = "Dec"
    \endlist

    The month names will be localized according to the system's locale
    settings.

    Returns an empty string if the date is invalid.

    \sa toString(), longMonthName(), shortDayName(), longDayName()
*/

QString QDate::shortMonthName(int month, QDate::MonthNameType type)
{
    if (month < 1 || month > 12) {
        return QString();
    }
    switch (type) {
    case QDate::DateFormat:
        return QLocale::system().monthName(month, QLocale::ShortFormat);
    case QDate::StandaloneFormat:
        return QLocale::system().standaloneMonthName(month, QLocale::ShortFormat);
    default:
        break;
    }
    return QString();
}

/*!
    Returns the long name of the \a month for the representation specified
    by \a type.  By default returns the normal type for date formatting.

    The months are enumerated using the following convention:

    \list
    \i 1 = "January"
    \i 2 = "February"
    \i 3 = "March"
    \i 4 = "April"
    \i 5 = "May"
    \i 6 = "June"
    \i 7 = "July"
    \i 8 = "August"
    \i 9 = "September"
    \i 10 = "October"
    \i 11 = "November"
    \i 12 = "December"
    \endlist

    The month names will be localized according to the system's locale
    settings.

    Returns an empty string if the date is invalid.

    \sa toString(), shortMonthName(), shortDayName(), longDayName()
*/

QString QDate::longMonthName(int month, MonthNameType type)
{
    if (month < 1 || month > 12) {
        return QString();
    }
    switch (type) {
    case QDate::DateFormat:
        return QLocale::system().monthName(month, QLocale::LongFormat);
    case QDate::StandaloneFormat:
        return QLocale::system().standaloneMonthName(month, QLocale::LongFormat);
    default:
        break;
    }
    return QString();
}

/*!
    Returns the short name of the \a weekday for the representation specified
    by \a type.  By default returns the normal type for date formatting.

    The days are enumerated using the following convention:

    \list
    \i 1 = "Mon"
    \i 2 = "Tue"
    \i 3 = "Wed"
    \i 4 = "Thu"
    \i 5 = "Fri"
    \i 6 = "Sat"
    \i 7 = "Sun"
    \endlist

    The day names will be localized according to the system's locale
    settings.

    Returns an empty string if the date is invalid.

    \sa toString(), shortMonthName(), longMonthName(), longDayName()
*/

QString QDate::shortDayName(int weekday, MonthNameType type)
{
    if (weekday < 1 || weekday > 7) {
        return QString();
    }
    switch (type) {
    case QDate::DateFormat:
        return QLocale::system().dayName(weekday, QLocale::ShortFormat);
    case QDate::StandaloneFormat:
        return QLocale::system().standaloneDayName(weekday, QLocale::ShortFormat);
    default:
        break;
    }
    return QString();
}

/*!
    Returns the long name of the \a weekday for the representation specified
    by \a type.  By default returns the normal type for date formatting.

    The days are enumerated using the following convention:

    \list
    \i 1 = "Monday"
    \i 2 = "Tuesday"
    \i 3 = "Wednesday"
    \i 4 = "Thursday"
    \i 5 = "Friday"
    \i 6 = "Saturday"
    \i 7 = "Sunday"
    \endlist

    The day names will be localized according to the system's locale
    settings.

    Returns an empty string if the date is invalid.

    \sa toString(), shortDayName(), shortMonthName(), longMonthName()
*/

QString QDate::longDayName(int weekday, MonthNameType type)
{
    if (weekday < 1 || weekday > 7) {
        return QString();
    }
    switch (type) {
    case QDate::DateFormat:
        return QLocale::system().dayName(weekday, QLocale::LongFormat);
    case QDate::StandaloneFormat:
        return QLocale::system().standaloneDayName(weekday, QLocale::LongFormat);
    default:
        break;
    }
    return QLocale::system().dayName(weekday, QLocale::LongFormat);
}

#endif //QT_NO_TEXTDATE

#ifndef QT_NO_DATESTRING

/*!
    \fn QString QDate::toString(Qt::DateFormat format) const

    \overload

    Returns the date as a string. The \a format parameter determines
    the format of the string.

    If the \a format is Qt::TextDate, the string is formatted in
    the default way. QDate::shortDayName() and QDate::shortMonthName()
    are used to generate the string, so the day and month names will
    be localized names. An example of this formatting is
    "Sat May 20 1995".

    If the \a format is Qt::ISODate, the string format corresponds
    to the ISO 8601 extended specification for representations of
    dates and times, taking the form YYYY-MM-DD, where YYYY is the
    year, MM is the month of the year (between 01 and 12), and DD is
    the day of the month between 01 and 31.

    If the \a format is Qt::SystemLocaleShortDate or
    Qt::SystemLocaleLongDate, the string format depends on the locale
    settings of the system. Identical to calling
    QLocale::system().toString(date, QLocale::ShortFormat) or
    QLocale::system().toString(date, QLocale::LongFormat).

    If the \a format is Qt::DefaultLocaleShortDate or
    Qt::DefaultLocaleLongDate, the string format depends on the
    default application locale. This is the locale set with
    QLocale::setDefault(), or the system locale if no default locale
    has been set. Identical to calling QLocale().toString(date,
    QLocale::ShortFormat) or QLocale().toString(date,
    QLocale::LongFormat).

    If the date is invalid, an empty string will be returned.

    \warning The Qt::ISODate format is only valid for years in the
    range 0 to 9999. This restriction may apply to locale-aware
    formats as well, depending on the locale settings.

    \sa shortDayName(), shortMonthName()
*/
QString QDate::toString(Qt::DateFormat f) const
{
    if (!isValid())
        return QString();
    int y, m, d;
    getDateFromJulianDay(jd, &y, &m, &d);
    switch (f) {
    case Qt::SystemLocaleDate:
    case Qt::SystemLocaleShortDate:
    case Qt::SystemLocaleLongDate:
        return QLocale::system().toString(*this, f == Qt::SystemLocaleLongDate ? QLocale::LongFormat
                                                                               : QLocale::ShortFormat);
    case Qt::LocaleDate:
    case Qt::DefaultLocaleShortDate:
    case Qt::DefaultLocaleLongDate:
        return QLocale().toString(*this, f == Qt::DefaultLocaleLongDate ? QLocale::LongFormat
                                                                        : QLocale::ShortFormat);
    default:
#ifndef QT_NO_TEXTDATE
    case Qt::TextDate:
        {
            return QString::fromLatin1("%0 %1 %2 %3")
                .arg(shortDayName(dayOfWeek()))
                .arg(shortMonthName(m))
                .arg(d)
                .arg(y);
        }
#endif
    case Qt::ISODate:
        {
            if (year() < 0 || year() > 9999)
                return QString();
            QString month(QString::number(m).rightJustified(2, QLatin1Char('0')));
            QString day(QString::number(d).rightJustified(2, QLatin1Char('0')));
            return QString::number(y) + QLatin1Char('-') + month + QLatin1Char('-') + day;
        }
    }
}

/*!
    Returns the date as a string. The \a format parameter determines
    the format of the result string.

    These expressions may be used:

    \table
    \header \i Expression \i Output
    \row \i d \i the day as number without a leading zero (1 to 31)
    \row \i dd \i the day as number with a leading zero (01 to 31)
    \row \i ddd
         \i the abbreviated localized day name (e.g. 'Mon' to 'Sun').
            Uses QDate::shortDayName().
    \row \i dddd
         \i the long localized day name (e.g. 'Monday' to 'Sunday').
            Uses QDate::longDayName().
    \row \i M \i the month as number without a leading zero (1 to 12)
    \row \i MM \i the month as number with a leading zero (01 to 12)
    \row \i MMM
         \i the abbreviated localized month name (e.g. 'Jan' to 'Dec').
            Uses QDate::shortMonthName().
    \row \i MMMM
         \i the long localized month name (e.g. 'January' to 'December').
            Uses QDate::longMonthName().
    \row \i yy \i the year as two digit number (00 to 99)
    \row \i yyyy \i the year as four digit number. If the year is negative,
            a minus sign is prepended in addition.
    \endtable

    All other input characters will be ignored. Any sequence of characters that
    are enclosed in singlequotes will be treated as text and not be used as an
    expression. Two consecutive singlequotes ("''") are replaced by a singlequote
    in the output.

    Example format strings (assuming that the QDate is the 20 July
    1969):

    \table
    \header \o Format            \o Result
    \row    \o dd.MM.yyyy        \o 20.07.1969
    \row    \o ddd MMMM d yy     \o Sun July 20 69
    \row    \o 'The day is' dddd \o The day is Sunday
    \endtable

    If the datetime is invalid, an empty string will be returned.

    \warning The Qt::ISODate format is only valid for years in the
    range 0 to 9999. This restriction may apply to locale-aware
    formats as well, depending on the locale settings.

    \sa QDateTime::toString() QTime::toString()

*/
QString QDate::toString(const QString& format) const
{
    if (year() > 9999)
        return QString();
    return fmtDateTime(format, 0, this);
}
#endif //QT_NO_DATESTRING


/*!
    Sets the date's \a year, \a month, and \a day. Returns true if
    the date is valid; otherwise returns false.

    If the specified date is invalid, the QDate object is set to be
    invalid.

    Note that any date before 4800 BCE or after about 1.4 million CE
    may not be accurately stored.

    \sa isValid()
*/
bool QDate::setDate(int year, int month, int day)
{
    if (isValid(year, month, day)) {
        jd = julianDayFromDate(year, month, day);
    } else {
        jd = nullJd();
    }
    return isValid();
}

/*!
    Extracts the date's year, month, and day, and assigns them to
    *\a year, *\a month, and *\a day. The pointers may be null.

    Returns 0 if the date is invalid.

    Note that any date before 4800 BCE or after about 1.4 million CE
    may not be accurately stored.

    \sa year(), month(), day(), isValid()
*/
void QDate::getDate(int *year, int *month, int *day)
{
    if (isValid()) {
        getDateFromJulianDay(jd, year, month, day);
    } else {
        if (year)
            *year = 0;
        if (month)
            *month = 0;
        if (day)
            *day = 0;
    }
}

/*!
    Returns a QDate object containing a date \a ndays later than the
    date of this object (or earlier if \a ndays is negative).

    Returns a null date if the current date is invalid or the new date is
    out-of-range.

    \sa addMonths() addYears() daysTo()
*/

QDate QDate::addDays(int ndays) const
{
    if (isNull())
        return QDate();

    QDate d;
    // this is basically "d.jd = jd + ndays" with checks for integer overflow
    if (ndays >= 0)
        d.jd = (jd + ndays >= jd) ? jd + ndays : nullJd();
    else
        d.jd = (jd + ndays < jd) ? jd + ndays : nullJd();
    return d;
}

/*!
    Returns a QDate object containing a date \a nmonths later than the
    date of this object (or earlier if \a nmonths is negative).

    \note If the ending day/month combination does not exist in the
    resulting month/year, this function will return a date that is the
    latest valid date.

    \warning QDate has a date hole around the days introducing the
    Gregorian calendar (the days 5 to 14 October 1582, inclusive, do
    not exist). If the calculation ends in one of those days, QDate
    will return either October 4 or October 15.

    \sa addDays() addYears()
*/

QDate QDate::addMonths(int nmonths) const
{
    if (!isValid())
        return QDate();
    if (!nmonths)
        return *this;

    int old_y, y, m, d;
    getDateFromJulianDay(jd, &y, &m, &d);
    old_y = y;

    bool increasing = nmonths > 0;

    while (nmonths != 0) {
        if (nmonths < 0 && nmonths + 12 <= 0) {
            y--;
            nmonths+=12;
        } else if (nmonths < 0) {
            m+= nmonths;
            nmonths = 0;
            if (m <= 0) {
                --y;
                m += 12;
            }
        } else if (nmonths - 12 >= 0) {
            y++;
            nmonths -= 12;
        } else if (m == 12) {
            y++;
            m = 0;
        } else {
            m += nmonths;
            nmonths = 0;
            if (m > 12) {
                ++y;
                m -= 12;
            }
        }
    }

    // was there a sign change?
    if ((old_y > 0 && y <= 0) ||
        (old_y < 0 && y >= 0))
        // yes, adjust the date by +1 or -1 years
        y += increasing ? +1 : -1;

    // did we end up in the Gregorian/Julian conversion hole?
    if (y == 1582 && m == 10 && d > 4 && d < 15)
        d = increasing ? 15 : 4;

    return fixedDate(y, m, d);
}

/*!
    Returns a QDate object containing a date \a nyears later than the
    date of this object (or earlier if \a nyears is negative).

    \note If the ending day/month combination does not exist in the
    resulting year (i.e., if the date was Feb 29 and the final year is
    not a leap year), this function will return a date that is the
    latest valid date (that is, Feb 28).

    \sa addDays(), addMonths()
*/

QDate QDate::addYears(int nyears) const
{
    if (!isValid())
        return QDate();

    int y, m, d;
    getDateFromJulianDay(jd, &y, &m, &d);

    int old_y = y;
    y += nyears;

    // was there a sign change?
    if ((old_y > 0 && y <= 0) ||
        (old_y < 0 && y >= 0))
        // yes, adjust the date by +1 or -1 years
        y += nyears > 0 ? +1 : -1;

    return fixedDate(y, m, d);
}

/*!
    Returns the number of days from this date to \a d (which is
    negative if \a d is earlier than this date).

    Returns 0 if either date is invalid.

    Example:
    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 0

    \sa addDays()
*/

int QDate::daysTo(const QDate &d) const
{
    if (isNull() || d.isNull()) {
        return 0;
    } else {
        return d.jd - jd;
    }
}


/*!
    \fn bool QDate::operator==(const QDate &d) const

    Returns true if this date is equal to \a d; otherwise returns
    false.

*/

/*!
    \fn bool QDate::operator!=(const QDate &d) const

    Returns true if this date is different from \a d; otherwise
    returns false.
*/

/*!
    \fn bool QDate::operator<(const QDate &d) const

    Returns true if this date is earlier than \a d; otherwise returns
    false.
*/

/*!
    \fn bool QDate::operator<=(const QDate &d) const

    Returns true if this date is earlier than or equal to \a d;
    otherwise returns false.
*/

/*!
    \fn bool QDate::operator>(const QDate &d) const

    Returns true if this date is later than \a d; otherwise returns
    false.
*/

/*!
    \fn bool QDate::operator>=(const QDate &d) const

    Returns true if this date is later than or equal to \a d;
    otherwise returns false.
*/

/*!
    \fn QDate::currentDate()
    Returns the current date, as reported by the system clock.

    \sa QTime::currentTime(), QDateTime::currentDateTime()
*/

#ifndef QT_NO_DATESTRING
/*!
    \fn QDate QDate::fromString(const QString &string, Qt::DateFormat format)

    Returns the QDate represented by the \a string, using the
    \a format given, or an invalid date if the string cannot be
    parsed.

    Note for Qt::TextDate: It is recommended that you use the
    English short month names (e.g. "Jan"). Although localized month
    names can also be used, they depend on the user's locale settings.
*/
QDate QDate::fromString(const QString& s, Qt::DateFormat f)
{
    if (s.isEmpty())
        return QDate();

    switch (f) {
    case Qt::ISODate:
        {
            int year(s.mid(0, 4).toInt());
            int month(s.mid(5, 2).toInt());
            int day(s.mid(8, 2).toInt());
            if (year && month && day)
                return QDate(year, month, day);
        }
        break;
    case Qt::SystemLocaleDate:
    case Qt::SystemLocaleShortDate:
    case Qt::SystemLocaleLongDate:
        return fromString(s, QLocale::system().dateFormat(f == Qt::SystemLocaleLongDate ? QLocale::LongFormat
                                                                                        : QLocale::ShortFormat));
    case Qt::LocaleDate:
    case Qt::DefaultLocaleShortDate:
    case Qt::DefaultLocaleLongDate:
        return fromString(s, QLocale().dateFormat(f == Qt::DefaultLocaleLongDate ? QLocale::LongFormat
                                                                                 : QLocale::ShortFormat));
    default:
#ifndef QT_NO_TEXTDATE
    case Qt::TextDate: {
        QStringList parts = s.split(QLatin1Char(' '), QString::SkipEmptyParts);

        if (parts.count() != 4) {
            return QDate();
        }

        QString monthName = parts.at(1);
        int month = -1;
        // Assume that English monthnames are the default
        for (int i = 0; i < 12; ++i) {
            if (monthName == QLatin1String(qt_shortMonthNames[i])) {
                month = i + 1;
                break;
            }
        }
        // If English names can't be found, search the localized ones
        if (month == -1) {
            for (int i = 1; i <= 12; ++i) {
                if (monthName == QDate::shortMonthName(i)) {
                    month = i;
                    break;
                }
            }
        }
        if (month < 1 || month > 12) {
            return QDate();
        }

        bool ok;
        int day = parts.at(2).toInt(&ok);
        if (!ok) {
            return QDate();
        }

        int year = parts.at(3).toInt(&ok);
        if (!ok) {
            return QDate();
        }

        return QDate(year, month, day);
    }
#else
        break;
#endif
    }
    return QDate();
}

/*!
    \fn QDate::fromString(const QString &string, const QString &format)

    Returns the QDate represented by the \a string, using the \a
    format given, or an invalid date if the string cannot be parsed.

    These expressions may be used for the format:

    \table
    \header \i Expression \i Output
    \row \i d \i The day as a number without a leading zero (1 to 31)
    \row \i dd \i The day as a number with a leading zero (01 to 31)
    \row \i ddd
         \i The abbreviated localized day name (e.g. 'Mon' to 'Sun').
            Uses QDate::shortDayName().
    \row \i dddd
         \i The long localized day name (e.g. 'Monday' to 'Sunday').
            Uses QDate::longDayName().
    \row \i M \i The month as a number without a leading zero (1 to 12)
    \row \i MM \i The month as a number with a leading zero (01 to 12)
    \row \i MMM
         \i The abbreviated localized month name (e.g. 'Jan' to 'Dec').
            Uses QDate::shortMonthName().
    \row \i MMMM
         \i The long localized month name (e.g. 'January' to 'December').
            Uses QDate::longMonthName().
    \row \i yy \i The year as two digit number (00 to 99)
    \row \i yyyy \i The year as four digit number. If the year is negative,
            a minus sign is prepended in addition.
    \endtable

    All other input characters will be treated as text. Any sequence
    of characters that are enclosed in single quotes will also be
    treated as text and will not be used as an expression. For example:

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 1

    If the format is not satisfied, an invalid QDate is returned. The
    expressions that don't expect leading zeroes (d, M) will be
    greedy. This means that they will use two digits even if this
    will put them outside the accepted range of values and leaves too
    few digits for other sections. For example, the following format
    string could have meant January 30 but the M will grab two
    digits, resulting in an invalid date:

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 2

    For any field that is not represented in the format the following
    defaults are used:

    \table
    \header \i Field  \i Default value
    \row    \i Year   \i 1900
    \row    \i Month  \i 1
    \row    \i Day    \i 1
    \endtable

    The following examples demonstrate the default values:

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 3

    \sa QDateTime::fromString(), QTime::fromString(), QDate::toString(),
        QDateTime::toString(), QTime::toString()
*/

QDate QDate::fromString(const QString &string, const QString &format)
{
    QDate date;
#ifndef QT_BOOTSTRAPPED
    QDateTimeParser dt(QVariant::Date, QDateTimeParser::FromString);
    if (dt.parseFormat(format))
        dt.fromString(string, &date, 0);
#else
    Q_UNUSED(string);
    Q_UNUSED(format);
#endif
    return date;
}
#endif // QT_NO_DATESTRING

/*!
    \overload

    Returns true if the specified date (\a year, \a month, and \a
    day) is valid; otherwise returns false.

    Example:
    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 4

    \sa isNull(), setDate()
*/

bool QDate::isValid(int year, int month, int day)
{
    if (year == 0) // there is no year 0 in the Julian calendar
        return false;

    // passage from Julian to Gregorian calendar
    if (year == 1582 && month == 10 && day > 4 && day < 15)
        return false;

    return (day > 0 && month > 0 && month <= 12) &&
           (day <= monthDays[month] || (day == 29 && month == 2 && isLeapYear(year)));
}

/*!
    \fn bool QDate::isLeapYear(int year)

    Returns true if the specified \a year is a leap year; otherwise
    returns false.
*/

bool QDate::isLeapYear(int y)
{
    if (y < 1582) {
        if ( y < 1) {  // No year 0 in Julian calendar, so -1, -5, -9 etc are leap years
            ++y;
        }
        return y % 4 == 0;
    } else {
        return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
    }
}

/*! \fn static QDate QDate::fromJulianDay(int jd)

    Converts the Julian day \a jd to a QDate.

    \sa toJulianDay()
*/

/*! \fn int QDate::toJulianDay() const

    Converts the date to a Julian day.

    \sa fromJulianDay()
*/

/*****************************************************************************
  QTime member functions
 *****************************************************************************/

/*!
    \class QTime
    \reentrant

    \brief The QTime class provides clock time functions.


    A QTime object contains a clock time, i.e. the number of hours,
    minutes, seconds, and milliseconds since midnight. It can read the
    current time from the system clock and measure a span of elapsed
    time. It provides functions for comparing times and for
    manipulating a time by adding a number of milliseconds.

    QTime uses the 24-hour clock format; it has no concept of AM/PM.
    Unlike QDateTime, QTime knows nothing about time zones or
    daylight savings time (DST).

    A QTime object is typically created either by giving the number
    of hours, minutes, seconds, and milliseconds explicitly, or by
    using the static function currentTime(), which creates a QTime
    object that contains the system's local time. Note that the
    accuracy depends on the accuracy of the underlying operating
    system; not all systems provide 1-millisecond accuracy.

    The hour(), minute(), second(), and msec() functions provide
    access to the number of hours, minutes, seconds, and milliseconds
    of the time. The same information is provided in textual format by
    the toString() function.

    QTime provides a full set of operators to compare two QTime
    objects. One time is considered smaller than another if it is
    earlier than the other.

    The time a given number of seconds or milliseconds later than a
    given time can be found using the addSecs() or addMSecs()
    functions. Correspondingly, the number of seconds or milliseconds
    between two times can be found using secsTo() or msecsTo().

    QTime can be used to measure a span of elapsed time using the
    start(), restart(), and elapsed() functions.

    \sa QDate, QDateTime
*/

/*!
    \fn QTime::QTime()

    Constructs a null time object. A null time can be a QTime(0, 0, 0, 0)
    (i.e., midnight) object, except that isNull() returns true and isValid()
    returns false.

    \sa isNull(), isValid()
*/

/*!
    Constructs a time with hour \a h, minute \a m, seconds \a s and
    milliseconds \a ms.

    \a h must be in the range 0 to 23, \a m and \a s must be in the
    range 0 to 59, and \a ms must be in the range 0 to 999.

    \sa isValid()
*/

QTime::QTime(int h, int m, int s, int ms)
{
    setHMS(h, m, s, ms);
}


/*!
    \fn bool QTime::isNull() const

    Returns true if the time is null (i.e., the QTime object was
    constructed using the default constructor); otherwise returns
    false. A null time is also an invalid time.

    \sa isValid()
*/

/*!
    Returns true if the time is valid; otherwise returns false. For example,
    the time 23:30:55.746 is valid, but 24:12:30 is invalid.

    \sa isNull()
*/

bool QTime::isValid() const
{
    return mds > NullTime && mds < MSECS_PER_DAY;
}


/*!
    Returns the hour part (0 to 23) of the time.

    Returns -1 if the time is invalid.

    \sa minute(), second(), msec()
*/

int QTime::hour() const
{
    if (isValid())
        return ds() / MSECS_PER_HOUR;
    else
        return -1;
}

/*!
    Returns the minute part (0 to 59) of the time.

    Returns -1 if the time is invalid.

    \sa hour(), second(), msec()
*/

int QTime::minute() const
{
    if (isValid())
        return (ds() % MSECS_PER_HOUR) / MSECS_PER_MIN;
    else
        return -1;
}

/*!
    Returns the second part (0 to 59) of the time.

    Returns -1 if the time is invalid.

    \sa hour(), minute(), msec()
*/

int QTime::second() const
{
    if (isValid())
        return (ds() / 1000) % SECS_PER_MIN;
    else
        return -1;
}

/*!
    Returns the millisecond part (0 to 999) of the time.

    Returns -1 if the time is invalid.

    \sa hour(), minute(), second()
*/

int QTime::msec() const
{
    if (isValid())
        return ds() % 1000;
    else
        return -1;
}

#ifndef QT_NO_DATESTRING
/*!
    \overload

    Returns the time as a string. Milliseconds are not included. The
    \a format parameter determines the format of the string.

    If \a format is Qt::TextDate, the string format is HH:MM:SS; e.g. 1
    second before midnight would be "23:59:59".

    If \a format is Qt::ISODate, the string format corresponds to the
    ISO 8601 extended specification for representations of dates,
    which is also HH:MM:SS. (However, contrary to ISO 8601, dates
    before 15 October 1582 are handled as Julian dates, not Gregorian
    dates. See \l{QDate G and J} {Use of Gregorian and Julian
    Calendars}. This might change in a future version of Qt.)

    If the \a format is Qt::SystemLocaleShortDate or
    Qt::SystemLocaleLongDate, the string format depends on the locale
    settings of the system. Identical to calling
    QLocale::system().toString(time, QLocale::ShortFormat) or
    QLocale::system().toString(time, QLocale::LongFormat).

    If the \a format is Qt::DefaultLocaleShortDate or
    Qt::DefaultLocaleLongDate, the string format depends on the
    default application locale. This is the locale set with
    QLocale::setDefault(), or the system locale if no default locale
    has been set. Identical to calling QLocale().toString(time,
    QLocale::ShortFormat) or QLocale().toString(time,
    QLocale::LongFormat).

    If the time is invalid, an empty string will be returned.
*/

QString QTime::toString(Qt::DateFormat format) const
{
    if (!isValid())
        return QString();

    switch (format) {
    case Qt::SystemLocaleDate:
    case Qt::SystemLocaleShortDate:
    case Qt::SystemLocaleLongDate:
        return QLocale::system().toString(*this, format == Qt::SystemLocaleLongDate ? QLocale::LongFormat
                                          : QLocale::ShortFormat);
    case Qt::LocaleDate:
    case Qt::DefaultLocaleShortDate:
    case Qt::DefaultLocaleLongDate:
        return QLocale().toString(*this, format == Qt::DefaultLocaleLongDate ? QLocale::LongFormat
                                  : QLocale::ShortFormat);

    default:
    case Qt::ISODate:
    case Qt::TextDate:
        return QString::fromLatin1("%1:%2:%3")
            .arg(hour(), 2, 10, QLatin1Char('0'))
            .arg(minute(), 2, 10, QLatin1Char('0'))
            .arg(second(), 2, 10, QLatin1Char('0'));
    }
}

/*!
    Returns the time as a string. The \a format parameter determines
    the format of the result string.

    These expressions may be used:

    \table
    \header \i Expression \i Output
    \row \i h
         \i the hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display)
    \row \i hh
         \i the hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)
    \row \i H
         \i the hour without a leading zero (0 to 23, even with AM/PM display)
    \row \i HH
         \i the hour with a leading zero (00 to 23, even with AM/PM display)
    \row \i m \i the minute without a leading zero (0 to 59)
    \row \i mm \i the minute with a leading zero (00 to 59)
    \row \i s \i the second without a leading zero (0 to 59)
    \row \i ss \i the second with a leading zero (00 to 59)
    \row \i z \i the milliseconds without leading zeroes (0 to 999)
    \row \i zzz \i the milliseconds with leading zeroes (000 to 999)
    \row \i AP or A
         \i use AM/PM display. \e AP will be replaced by either "AM" or "PM".
    \row \i ap or a
         \i use am/pm display. \e ap will be replaced by either "am" or "pm".
    \row \i t \i the timezone (for example "CEST")
    \endtable

    All other input characters will be ignored. Any sequence of characters that
    are enclosed in singlequotes will be treated as text and not be used as an
    expression. Two consecutive singlequotes ("''") are replaced by a singlequote
    in the output.

    Example format strings (assuming that the QTime is 14:13:09.042)

    \table
    \header \i Format \i Result
    \row \i hh:mm:ss.zzz \i 14:13:09.042
    \row \i h:m:s ap     \i 2:13:9 pm
    \row \i H:m:s a      \i 14:13:9 pm
    \endtable

    If the datetime is invalid, an empty string will be returned.
    If \a format is empty, the default format "hh:mm:ss" is used.

    \sa QDate::toString() QDateTime::toString()
*/
QString QTime::toString(const QString& format) const
{
    return fmtDateTime(format, this, 0);
}
#endif //QT_NO_DATESTRING
/*!
    Sets the time to hour \a h, minute \a m, seconds \a s and
    milliseconds \a ms.

    \a h must be in the range 0 to 23, \a m and \a s must be in the
    range 0 to 59, and \a ms must be in the range 0 to 999.
    Returns true if the set time is valid; otherwise returns false.

    \sa isValid()
*/

bool QTime::setHMS(int h, int m, int s, int ms)
{
#if defined(Q_OS_WINCE)
    startTick = NullTime;
#endif
    if (!isValid(h,m,s,ms)) {
        mds = NullTime;                // make this invalid
        return false;
    }
    mds = (h*SECS_PER_HOUR + m*SECS_PER_MIN + s)*1000 + ms;
    return true;
}

/*!
    Returns a QTime object containing a time \a s seconds later
    than the time of this object (or earlier if \a s is negative).

    Note that the time will wrap if it passes midnight.

    Returns a null time if this time is invalid.

    Example:

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 5

    \sa addMSecs(), secsTo(), QDateTime::addSecs()
*/

QTime QTime::addSecs(int s) const
{
    return addMSecs(s * 1000);
}

/*!
    Returns the number of seconds from this time to \a t.
    If \a t is earlier than this time, the number of seconds returned
    is negative.

    Because QTime measures time within a day and there are 86400
    seconds in a day, the result is always between -86400 and 86400.

    secsTo() does not take into account any milliseconds.

    Returns 0 if either time is invalid.

    \sa addSecs(), QDateTime::secsTo()
*/

int QTime::secsTo(const QTime &t) const
{
    if (isValid() && t.isValid())
        return (t.ds() - ds()) / 1000;
    else
        return 0;
}

/*!
    Returns a QTime object containing a time \a ms milliseconds later
    than the time of this object (or earlier if \a ms is negative).

    Note that the time will wrap if it passes midnight. See addSecs()
    for an example.

    Returns a null time if this time is invalid.

    \sa addSecs(), msecsTo(), QDateTime::addMSecs()
*/

QTime QTime::addMSecs(int ms) const
{
    QTime t;
    if (isValid()) {
        if (ms < 0) {
            // % not well-defined for -ve, but / is.
            int negdays = (MSECS_PER_DAY - ms) / MSECS_PER_DAY;
            t.mds = (ds() + ms + negdays * MSECS_PER_DAY) % MSECS_PER_DAY;
        } else {
            t.mds = (ds() + ms) % MSECS_PER_DAY;
        }
    }
#if defined(Q_OS_WINCE)
    if (startTick > NullTime)
        t.startTick = (startTick + ms) % MSECS_PER_DAY;
#endif
    return t;
}

/*!
    Returns the number of milliseconds from this time to \a t.
    If \a t is earlier than this time, the number of milliseconds returned
    is negative.

    Because QTime measures time within a day and there are 86400
    seconds in a day, the result is always between -86400000 and
    86400000 ms.

    Returns 0 if either time is invalid.

    \sa secsTo(), addMSecs(), QDateTime::msecsTo()
*/

int QTime::msecsTo(const QTime &t) const
{
    if (isValid() && t.isValid()) {
#if defined(Q_OS_WINCE)
        // GetLocalTime() for Windows CE has no milliseconds resolution
        if (t.startTick > NullTime && startTick > NullTime)
            return t.startTick - startTick;
        else
#endif
            return t.ds() - ds();
    } else {
        return 0;
    }
}


/*!
    \fn bool QTime::operator==(const QTime &t) const

    Returns true if this time is equal to \a t; otherwise returns false.
*/

/*!
    \fn bool QTime::operator!=(const QTime &t) const

    Returns true if this time is different from \a t; otherwise returns false.
*/

/*!
    \fn bool QTime::operator<(const QTime &t) const

    Returns true if this time is earlier than \a t; otherwise returns false.
*/

/*!
    \fn bool QTime::operator<=(const QTime &t) const

    Returns true if this time is earlier than or equal to \a t;
    otherwise returns false.
*/

/*!
    \fn bool QTime::operator>(const QTime &t) const

    Returns true if this time is later than \a t; otherwise returns false.
*/

/*!
    \fn bool QTime::operator>=(const QTime &t) const

    Returns true if this time is later than or equal to \a t;
    otherwise returns false.
*/

/*!
    \fn QTime::currentTime()

    Returns the current time as reported by the system clock.

    Note that the accuracy depends on the accuracy of the underlying
    operating system; not all systems provide 1-millisecond accuracy.
*/

#ifndef QT_NO_DATESTRING
/*!
    \fn QTime QTime::fromString(const QString &string, Qt::DateFormat format)

    Returns the time represented in the \a string as a QTime using the
    \a format given, or an invalid time if this is not possible.

    Note that fromString() uses a "C" locale encoded string to convert
    milliseconds to a float value. If the default locale is not "C",
    this may result in two conversion attempts (if the conversion
    fails for the default locale). This should be considered an
    implementation detail.
*/
QTime QTime::fromString(const QString& s, Qt::DateFormat f)
{
    if (s.isEmpty()) {
        QTime t;
        t.mds = NullTime;
        return t;
    }

    switch (f) {
    case Qt::SystemLocaleDate:
    case Qt::SystemLocaleShortDate:
    case Qt::SystemLocaleLongDate:
        return fromString(s, QLocale::system().timeFormat(f == Qt::SystemLocaleLongDate ? QLocale::LongFormat
                                                                                        : QLocale::ShortFormat));
    case Qt::LocaleDate:
    case Qt::DefaultLocaleShortDate:
    case Qt::DefaultLocaleLongDate:
        return fromString(s, QLocale().timeFormat(f == Qt::DefaultLocaleLongDate ? QLocale::LongFormat
                                                                                 : QLocale::ShortFormat));
    default:
        {
            bool ok = true;
            const int hour(s.mid(0, 2).toInt(&ok));
            if (!ok)
                return QTime();
            const int minute(s.mid(3, 2).toInt(&ok));
            if (!ok)
                return QTime();
            const int second(s.mid(6, 2).toInt(&ok));
            if (!ok)
                return QTime();
            const QString msec_s(QLatin1String("0.") + s.mid(9, 4));
            const float msec(msec_s.toFloat(&ok));
            if (!ok)
                return QTime(hour, minute, second, 0);
            return QTime(hour, minute, second, qMin(qRound(msec * 1000.0), 999));
        }
    }
}

/*!
    \fn QTime::fromString(const QString &string, const QString &format)

    Returns the QTime represented by the \a string, using the \a
    format given, or an invalid time if the string cannot be parsed.

    These expressions may be used for the format:

    \table
    \header \i Expression \i Output
    \row \i h
         \i the hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display)
    \row \i hh
         \i the hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)
    \row \i m \i the minute without a leading zero (0 to 59)
    \row \i mm \i the minute with a leading zero (00 to 59)
    \row \i s \i the second without a leading zero (0 to 59)
    \row \i ss \i the second with a leading zero (00 to 59)
    \row \i z \i the milliseconds without leading zeroes (0 to 999)
    \row \i zzz \i the milliseconds with leading zeroes (000 to 999)
    \row \i AP
         \i interpret as an AM/PM time. \e AP must be either "AM" or "PM".
    \row \i ap
         \i Interpret as an AM/PM time. \e ap must be either "am" or "pm".
    \endtable

    All other input characters will be treated as text. Any sequence
    of characters that are enclosed in single quotes will also be
    treated as text and not be used as an expression.

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 6

    If the format is not satisfied an invalid QTime is returned.
    Expressions that do not expect leading zeroes to be given (h, m, s
    and z) are greedy. This means that they will use two digits even if
    this puts them outside the range of accepted values and leaves too
    few digits for other sections. For example, the following string
    could have meant 00:07:10, but the m will grab two digits, resulting
    in an invalid time:

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 7

    Any field that is not represented in the format will be set to zero.
    For example:

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 8

    \sa QDateTime::fromString() QDate::fromString() QDate::toString()
    QDateTime::toString() QTime::toString()
*/

QTime QTime::fromString(const QString &string, const QString &format)
{
    QTime time;
#ifndef QT_BOOTSTRAPPED
    QDateTimeParser dt(QVariant::Time, QDateTimeParser::FromString);
    if (dt.parseFormat(format))
        dt.fromString(string, 0, &time);
#else
    Q_UNUSED(string);
    Q_UNUSED(format);
#endif
    return time;
}

#endif // QT_NO_DATESTRING


/*!
    \overload

    Returns true if the specified time is valid; otherwise returns
    false.

    The time is valid if \a h is in the range 0 to 23, \a m and
    \a s are in the range 0 to 59, and \a ms is in the range 0 to 999.

    Example:

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 9
*/

bool QTime::isValid(int h, int m, int s, int ms)
{
    return (uint)h < 24 && (uint)m < 60 && (uint)s < 60 && (uint)ms < 1000;
}


/*!
    Sets this time to the current time. This is practical for timing:

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 10

    \sa restart(), elapsed(), currentTime()
*/

void QTime::start()
{
    *this = currentTime();
}

/*!
    Sets this time to the current time and returns the number of
    milliseconds that have elapsed since the last time start() or
    restart() was called.

    This function is guaranteed to be atomic and is thus very handy
    for repeated measurements. Call start() to start the first
    measurement, and restart() for each later measurement.

    Note that the counter wraps to zero 24 hours after the last call
    to start() or restart().

    \warning If the system's clock setting has been changed since the
    last time start() or restart() was called, the result is
    undefined. This can happen when daylight savings time is turned on
    or off.

    \sa start(), elapsed(), currentTime()
*/

int QTime::restart()
{
    QTime t = currentTime();
    int n = msecsTo(t);
    if (n < 0)                                // passed midnight
        n += 86400*1000;
    *this = t;
    return n;
}

/*!
    Returns the number of milliseconds that have elapsed since the
    last time start() or restart() was called.

    Note that the counter wraps to zero 24 hours after the last call
    to start() or restart.

    Note that the accuracy depends on the accuracy of the underlying
    operating system; not all systems provide 1-millisecond accuracy.

    \warning If the system's clock setting has been changed since the
    last time start() or restart() was called, the result is
    undefined. This can happen when daylight savings time is turned on
    or off.

    \sa start(), restart()
*/

int QTime::elapsed() const
{
    int n = msecsTo(currentTime());
    if (n < 0)                                // passed midnight
        n += 86400 * 1000;
    return n;
}


/*****************************************************************************
  QDateTime member functions
 *****************************************************************************/

/*!
    \class QDateTime
    \reentrant
    \brief The QDateTime class provides date and time functions.


    A QDateTime object contains a calendar date and a clock time (a
    "datetime"). It is a combination of the QDate and QTime classes.
    It can read the current datetime from the system clock. It
    provides functions for comparing datetimes and for manipulating a
    datetime by adding a number of seconds, days, months, or years.

    A QDateTime object is typically created either by giving a date
    and time explicitly in the constructor, or by using the static
    function currentDateTime() that returns a QDateTime object set
    to the system clock's time. The date and time can be changed with
    setDate() and setTime(). A datetime can also be set using the
    setTime_t() function that takes a POSIX-standard "number of
    seconds since 00:00:00 on January 1, 1970" value. The fromString()
    function returns a QDateTime, given a string and a date format
    used to interpret the date within the string.

    The date() and time() functions provide access to the date and
    time parts of the datetime. The same information is provided in
    textual format by the toString() function.

    QDateTime provides a full set of operators to compare two
    QDateTime objects where smaller means earlier and larger means
    later.

    You can increment (or decrement) a datetime by a given number of
    milliseconds using addMSecs(), seconds using addSecs(), or days
    using addDays(). Similarly you can use addMonths() and addYears().
    The daysTo() function returns the number of days between two datetimes,
    secsTo() returns the number of seconds between two datetimes, and
    msecsTo() returns the number of milliseconds between two datetimes.

    QDateTime can store datetimes as \l{Qt::LocalTime}{local time} or
    as \l{Qt::UTC}{UTC}. QDateTime::currentDateTime() returns a
    QDateTime expressed as local time; use toUTC() to convert it to
    UTC. You can also use timeSpec() to find out if a QDateTime
    object stores a UTC time or a local time. Operations such as
    addSecs() and secsTo() are aware of daylight saving time (DST).

    \note QDateTime does not account for leap seconds.

    \section1

    \target QDateTime G and J
    \section2 Use of Gregorian and Julian Calendars

    QDate uses the Gregorian calendar in all locales, beginning
    on the date 15 October 1582. For dates up to and including 4
    October 1582, the Julian calendar is used.  This means there is a
    10-day gap in the internal calendar between the 4th and the 15th
    of October 1582. When you use QDateTime for dates in that epoch,
    the day after 4 October 1582 is 15 October 1582, and the dates in
    the gap are invalid.

    The Julian to Gregorian changeover date used here is the date when
    the Gregorian calendar was first introduced, by Pope Gregory
    XIII. That change was not universally accepted and some localities
    only executed it at a later date (if at all).  QDateTime
    doesn't take any of these historical facts into account. If an
    application must support a locale-specific dating system, it must
    do so on its own, remembering to convert the dates using the
    Julian day.

    \section2 No Year 0

    There is no year 0. Dates in that year are considered invalid. The
    year -1 is the year "1 before Christ" or "1 before current era."
    The day before 1 January 1 CE is 31 December 1 BCE.

    \section2 Range of Valid Dates

    Dates are stored internally as a Julian Day number, an interger count of
    every day in a contiguous range, with 1 January 4713 BCE being Julian Day 0.
    As well as being an efficient and accurate way of storing an absolute date,
    it is suitable for converting a Date into other calendar systems such as
    Hebrew, Islamic or Chinese. The Julian Day number can be obtained using
    QDate::toJulianDay() and can be set using QDate::fromJulianDay().

    The range of dates able to be stored by QDate as a Julian Day number is
    from around 5 million BCE to around 5 million CE. The range of Julian Days
    able to be accurately converted to and from valid YMD form Dates is
    restricted to 1 January 4800 BCE to 31 December 1400000 CE due to
    shortcomings in the available conversion formulas. This may change in the
    future.

    The Gregorian calendar was introduced in different places around
    the world on different dates. QDateTime uses QDate to store the
    date, so it uses the Gregorian calendar for all locales, beginning
    on the date 15 October 1582. For dates up to and including 4
    October 1582, QDateTime uses the Julian calendar.  This means
    there is a 10-day gap in the QDateTime calendar between the 4th
    and the 15th of October 1582. When you use QDateTime for dates in
    that epoch, the day after 4 October 1582 is 15 October 1582, and
    the dates in the gap are invalid.

    \section2
    Use of System Timezone

    QDateTime uses the system's time zone information to determine the
    offset of local time from UTC. If the system is not configured
    correctly or not up-to-date, QDateTime will give wrong results as
    well.

    \section2 Daylight Savings Time (DST)

    QDateTime takes into account the system's time zone information
    when dealing with DST. On modern Unix systems, this means it
    applies the correct historical DST data whenever possible. On
    Windows and Windows CE, where the system doesn't support
    historical DST data, historical accuracy is not maintained with
    respect to DST.

    The range of valid dates taking DST into account is 1970-01-01 to
    the present, and rules are in place for handling DST correctly
    until 2037-12-31, but these could change. For dates falling
    outside that range, QDateTime makes a \e{best guess} using the
    rules for year 1970 or 2037, but we can't guarantee accuracy. This
    means QDateTime doesn't take into account changes in a locale's
    time zone before 1970, even if the system's time zone database
    supports that information.

    \sa QDate QTime QDateTimeEdit
*/

/*!
    Constructs a null datetime (i.e. null date and null time). A null
    datetime is invalid, since the date is invalid.

    \sa isValid()
*/
QDateTime::QDateTime()
    : d(new QDateTimePrivate)
{
}


/*!
    Constructs a datetime with the given \a date, a valid
    time(00:00:00.000), and sets the timeSpec() to Qt::LocalTime.
*/

QDateTime::QDateTime(const QDate &date)
    : d(new QDateTimePrivate)
{
    d->date = date;
    d->time = QTime(0, 0, 0);
}

/*!
    Constructs a datetime with the given \a date and \a time, using
    the time specification defined by \a spec.

    If \a date is valid and \a time is not, the time will be set to midnight.
*/

QDateTime::QDateTime(const QDate &date, const QTime &time, Qt::TimeSpec spec)
    : d(new QDateTimePrivate)
{
    d->date = date;
    d->time = date.isValid() && !time.isValid() ? QTime(0, 0, 0) : time;
    d->spec = (spec == Qt::UTC) ? QDateTimePrivate::UTC : QDateTimePrivate::LocalUnknown;
}

/*!
    Constructs a copy of the \a other datetime.
*/

QDateTime::QDateTime(const QDateTime &other)
    : d(other.d)
{
}

/*!
    Destroys the datetime.
*/
QDateTime::~QDateTime()
{
}

/*!
    Makes a copy of the \a other datetime and returns a reference to the
    copy.
*/

QDateTime &QDateTime::operator=(const QDateTime &other)
{
    d = other.d;
    return *this;
}

/*!
    Returns true if both the date and the time are null; otherwise
    returns false. A null datetime is invalid.

    \sa QDate::isNull(), QTime::isNull(), isValid()
*/

bool QDateTime::isNull() const
{
    return d->date.isNull() && d->time.isNull();
}

/*!
    Returns true if both the date and the time are valid; otherwise
    returns false.

    \sa QDate::isValid(), QTime::isValid()
*/

bool QDateTime::isValid() const
{
    return d->date.isValid() && d->time.isValid();
}

/*!
    Returns the date part of the datetime.

    \sa setDate(), time(), timeSpec()
*/

QDate QDateTime::date() const
{
    return d->date;
}

/*!
    Returns the time part of the datetime.

    \sa setTime(), date(), timeSpec()
*/

QTime QDateTime::time() const
{
    return d->time;
}

/*!
    Returns the time specification of the datetime.

    \sa setTimeSpec(), date(), time(), Qt::TimeSpec
*/

Qt::TimeSpec QDateTime::timeSpec() const
{
    switch(d->spec)
    {
        case QDateTimePrivate::UTC:
            return Qt::UTC;
        case QDateTimePrivate::OffsetFromUTC:
            return Qt::OffsetFromUTC;
        default:
            return Qt::LocalTime;
    }
}

/*!
    Sets the date part of this datetime to \a date.
    If no time is set, it is set to midnight.

    \sa date(), setTime(), setTimeSpec()
*/

void QDateTime::setDate(const QDate &date)
{
    detach();
    d->date = date;
    if (d->spec == QDateTimePrivate::LocalStandard
        || d->spec == QDateTimePrivate::LocalDST)
        d->spec = QDateTimePrivate::LocalUnknown;
    if (date.isValid() && !d->time.isValid())
        d->time = QTime(0, 0, 0);
}

/*!
    Sets the time part of this datetime to \a time.

    \sa time(), setDate(), setTimeSpec()
*/

void QDateTime::setTime(const QTime &time)
{
    detach();
    if (d->spec == QDateTimePrivate::LocalStandard
        || d->spec == QDateTimePrivate::LocalDST)
        d->spec = QDateTimePrivate::LocalUnknown;
    d->time = time;
}

/*!
    Sets the time specification used in this datetime to \a spec.

    \sa timeSpec(), setDate(), setTime(), Qt::TimeSpec
*/

void QDateTime::setTimeSpec(Qt::TimeSpec spec)
{
    detach();

    switch(spec)
    {
        case Qt::UTC:
            d->spec = QDateTimePrivate::UTC;
            break;
        case Qt::OffsetFromUTC:
            d->spec = QDateTimePrivate::OffsetFromUTC;
            break;
        default:
            d->spec = QDateTimePrivate::LocalUnknown;
            break;
    }
}

qint64 toMSecsSinceEpoch_helper(qint64 jd, int msecs)
{
    qint64 days = jd - JULIAN_DAY_FOR_EPOCH;
    qint64 retval = (days * MSECS_PER_DAY) + msecs;
    return retval;
}

/*!
    Returns the datetime as the number of milliseconds that have passed
    since 1970-01-01T00:00:00.000, Coordinated Universal Time (Qt::UTC).

    On systems that do not support time zones, this function will
    behave as if local time were Qt::UTC.

    The behavior for this function is undefined if the datetime stored in
    this object is not valid. However, for all valid dates, this function
    returns a unique value.

    \sa toTime_t(), setMSecsSinceEpoch()
*/
qint64 QDateTime::toMSecsSinceEpoch() const
{
    QDate utcDate;
    QTime utcTime;
    d->getUTC(utcDate, utcTime);

    return toMSecsSinceEpoch_helper(utcDate.toJulianDay(), QTime(0, 0, 0).msecsTo(utcTime));
}

/*!
    Returns the datetime as the number of seconds that have passed
    since 1970-01-01T00:00:00, Coordinated Universal Time (Qt::UTC).

    On systems that do not support time zones, this function will
    behave as if local time were Qt::UTC.

    \note This function returns a 32-bit unsigned integer, so it does not
    support dates before 1970, but it does support dates after
    2038-01-19T03:14:06, which may not be valid time_t values. Be careful
    when passing those time_t values to system functions, which could
    interpret them as negative dates.

    If the date is outside the range 1970-01-01T00:00:00 to
    2106-02-07T06:28:14, this function returns -1 cast to an unsigned integer
    (i.e., 0xFFFFFFFF).

    To get an extended range, use toMSecsSinceEpoch().

    \sa toMSecsSinceEpoch(), setTime_t()
*/

uint QDateTime::toTime_t() const
{
    qint64 retval = toMSecsSinceEpoch() / 1000;
    if (quint64(retval) >= Q_UINT64_C(0xFFFFFFFF))
        return uint(-1);
    return uint(retval);
}

/*!
    Sets the date and time given the number of milliseconds,\a msecs, that have
    passed since 1970-01-01T00:00:00.000, Coordinated Universal Time
    (Qt::UTC). On systems that do not support time zones this function
    will behave as if local time were Qt::UTC.

    Note that there are possible values for \a msecs that lie outside the
    valid range of QDateTime, both negative and positive. The behavior of
    this function is undefined for those values.

    \sa toMSecsSinceEpoch(), setTime_t()
*/
void QDateTime::setMSecsSinceEpoch(qint64 msecs)
{
    detach();

    QDateTimePrivate::Spec oldSpec = d->spec;

    int ddays = msecs / MSECS_PER_DAY;
    msecs %= MSECS_PER_DAY;
    if (msecs < 0) {
        // negative
        --ddays;
        msecs += MSECS_PER_DAY;
    }

    d->date = QDate(1970, 1, 1).addDays(ddays);
    d->time = QTime(0, 0, 0).addMSecs(msecs);
    d->spec = QDateTimePrivate::UTC;

    if (oldSpec != QDateTimePrivate::UTC)
        d->spec = d->getLocal(d->date, d->time);
}

/*!
    \fn void QDateTime::setTime_t(uint seconds)

    Sets the date and time given the number of \a seconds that have
    passed since 1970-01-01T00:00:00, Coordinated Universal Time
    (Qt::UTC). On systems that do not support time zones this function
    will behave as if local time were Qt::UTC.

    \sa toTime_t()
*/

void QDateTime::setTime_t(uint secsSince1Jan1970UTC)
{
    detach();

    QDateTimePrivate::Spec oldSpec = d->spec;

    d->date = QDate(1970, 1, 1).addDays(secsSince1Jan1970UTC / SECS_PER_DAY);
    d->time = QTime(0, 0, 0).addSecs(secsSince1Jan1970UTC % SECS_PER_DAY);
    d->spec = QDateTimePrivate::UTC;

    if (oldSpec != QDateTimePrivate::UTC)
        d->spec = d->getLocal(d->date, d->time);
}

#ifndef QT_NO_DATESTRING
/*!
    \fn QString QDateTime::toString(Qt::DateFormat format) const

    \overload

    Returns the datetime as a string in the \a format given.

    If the \a format is Qt::TextDate, the string is formatted in
    the default way. QDate::shortDayName(), QDate::shortMonthName(),
    and QTime::toString() are used to generate the string, so the
    day and month names will be localized names. An example of this
    formatting is "Wed May 20 03:40:13 1998".

    If the \a format is Qt::ISODate, the string format corresponds
    to the ISO 8601 extended specification for representations of
    dates and times, taking the form YYYY-MM-DDTHH:MM:SS[Z|[+|-]HH:MM],
    depending on the timeSpec() of the QDateTime. If the timeSpec()
    is Qt::UTC, Z will be appended to the string; if the timeSpec() is
    Qt::OffsetFromUTC the offset in hours and minutes from UTC will
    be appended to the string.

    If the \a format is Qt::SystemLocaleShortDate or
    Qt::SystemLocaleLongDate, the string format depends on the locale
    settings of the system. Identical to calling
    QLocale::system().toString(datetime, QLocale::ShortFormat) or
    QLocale::system().toString(datetime, QLocale::LongFormat).

    If the \a format is Qt::DefaultLocaleShortDate or
    Qt::DefaultLocaleLongDate, the string format depends on the
    default application locale. This is the locale set with
    QLocale::setDefault(), or the system locale if no default locale
    has been set. Identical to calling QLocale().toString(datetime,
    QLocale::ShortFormat) or QLocale().toString(datetime,
    QLocale::LongFormat).

    If the datetime is invalid, an empty string will be returned.

    \warning The Qt::ISODate format is only valid for years in the
    range 0 to 9999. This restriction may apply to locale-aware
    formats as well, depending on the locale settings.

    \sa QDate::toString() QTime::toString() Qt::DateFormat
*/

QString QDateTime::toString(Qt::DateFormat f) const
{
    QString buf;
    if (!isValid())
        return buf;

    if (f == Qt::ISODate) {
        buf = d->date.toString(Qt::ISODate);
        if (buf.isEmpty())
            return QString();   // failed to convert
        buf += QLatin1Char('T');
        buf += d->time.toString(Qt::ISODate);
        switch (d->spec) {
        case QDateTimePrivate::UTC:
            buf += QLatin1Char('Z');
            break;
        case QDateTimePrivate::OffsetFromUTC: {
            int sign = d->utcOffset >= 0 ? 1: -1;
            buf += QString::fromLatin1("%1%2:%3").
                arg(sign == 1 ? QLatin1Char('+') : QLatin1Char('-')).
                arg(d->utcOffset * sign / SECS_PER_HOUR, 2, 10, QLatin1Char('0')).
                arg((d->utcOffset / 60) % 60, 2, 10, QLatin1Char('0'));
            break;
        }
        default:
            break;
        }
    }
#ifndef QT_NO_TEXTDATE
    else if (f == Qt::TextDate) {
#ifndef Q_OS_WIN
        buf = d->date.shortDayName(d->date.dayOfWeek());
        buf += QLatin1Char(' ');
        buf += d->date.shortMonthName(d->date.month());
        buf += QLatin1Char(' ');
        buf += QString::number(d->date.day());
#else
        wchar_t out[255];
        GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ILDATE, out, 255);
        QString winstr = QString::fromWCharArray(out);
        switch (winstr.toInt()) {
        case 1:
            buf = d->date.shortDayName(d->date.dayOfWeek());
            buf += QLatin1Char(' ');
            buf += QString::number(d->date.day());
            buf += QLatin1String(". ");
            buf += d->date.shortMonthName(d->date.month());
            break;
        default:
            buf = d->date.shortDayName(d->date.dayOfWeek());
            buf += QLatin1Char(' ');
            buf += d->date.shortMonthName(d->date.month());
            buf += QLatin1Char(' ');
            buf += QString::number(d->date.day());
        }
#endif
        buf += QLatin1Char(' ');
        buf += d->time.toString();
        buf += QLatin1Char(' ');
        buf += QString::number(d->date.year());
    }
#endif
    else {
        buf = d->date.toString(f);
        if (buf.isEmpty())
            return QString();   // failed to convert
        buf += QLatin1Char(' ');
        buf += d->time.toString(f);
    }

    return buf;
}

/*!
    Returns the datetime as a string. The \a format parameter
    determines the format of the result string.

    These expressions may be used for the date:

    \table
    \header \i Expression \i Output
    \row \i d \i the day as number without a leading zero (1 to 31)
    \row \i dd \i the day as number with a leading zero (01 to 31)
    \row \i ddd
            \i the abbreviated localized day name (e.g. 'Mon' to 'Sun').
            Uses QDate::shortDayName().
    \row \i dddd
            \i the long localized day name (e.g. 'Monday' to 'Qt::Sunday').
            Uses QDate::longDayName().
    \row \i M \i the month as number without a leading zero (1-12)
    \row \i MM \i the month as number with a leading zero (01-12)
    \row \i MMM
            \i the abbreviated localized month name (e.g. 'Jan' to 'Dec').
            Uses QDate::shortMonthName().
    \row \i MMMM
            \i the long localized month name (e.g. 'January' to 'December').
            Uses QDate::longMonthName().
    \row \i yy \i the year as two digit number (00-99)
    \row \i yyyy \i the year as four digit number
    \endtable

    These expressions may be used for the time:

    \table
    \header \i Expression \i Output
    \row \i h
         \i the hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display)
    \row \i hh
         \i the hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)
    \row \i m \i the minute without a leading zero (0 to 59)
    \row \i mm \i the minute with a leading zero (00 to 59)
    \row \i s \i the second without a leading zero (0 to 59)
    \row \i ss \i the second with a leading zero (00 to 59)
    \row \i z \i the milliseconds without leading zeroes (0 to 999)
    \row \i zzz \i the milliseconds with leading zeroes (000 to 999)
    \row \i AP
            \i use AM/PM display. \e AP will be replaced by either "AM" or "PM".
    \row \i ap
            \i use am/pm display. \e ap will be replaced by either "am" or "pm".
    \endtable

    All other input characters will be ignored. Any sequence of characters that
    are enclosed in singlequotes will be treated as text and not be used as an
    expression. Two consecutive singlequotes ("''") are replaced by a singlequote
    in the output.

    Example format strings (assumed that the QDateTime is 21 May 2001
    14:13:09):

    \table
    \header \i Format       \i Result
    \row \i dd.MM.yyyy      \i 21.05.2001
    \row \i ddd MMMM d yy   \i Tue May 21 01
    \row \i hh:mm:ss.zzz    \i 14:13:09.042
    \row \i h:m:s ap        \i 2:13:9 pm
    \endtable

    If the datetime is invalid, an empty string will be returned.

    \sa QDate::toString() QTime::toString()
*/
QString QDateTime::toString(const QString& format) const
{
    return fmtDateTime(format, &d->time, &d->date);
}
#endif //QT_NO_DATESTRING

/*!
    Returns a QDateTime object containing a datetime \a ndays days
    later than the datetime of this object (or earlier if \a ndays is
    negative).

    \sa daysTo(), addMonths(), addYears(), addSecs()
*/

QDateTime QDateTime::addDays(int ndays) const
{
    return QDateTime(d->date.addDays(ndays), d->time, timeSpec());
}

/*!
    Returns a QDateTime object containing a datetime \a nmonths months
    later than the datetime of this object (or earlier if \a nmonths
    is negative).

    \sa daysTo(), addDays(), addYears(), addSecs()
*/

QDateTime QDateTime::addMonths(int nmonths) const
{
    return QDateTime(d->date.addMonths(nmonths), d->time, timeSpec());
}

/*!
    Returns a QDateTime object containing a datetime \a nyears years
    later than the datetime of this object (or earlier if \a nyears is
    negative).

    \sa daysTo(), addDays(), addMonths(), addSecs()
*/

QDateTime QDateTime::addYears(int nyears) const
{
    return QDateTime(d->date.addYears(nyears), d->time, timeSpec());
}

QDateTime QDateTimePrivate::addMSecs(const QDateTime &dt, qint64 msecs)
{
    QDate utcDate;
    QTime utcTime;
    dt.d->getUTC(utcDate, utcTime);

    addMSecs(utcDate, utcTime, msecs);

    return QDateTime(utcDate, utcTime, Qt::UTC).toTimeSpec(dt.timeSpec());
}

/*!
 Adds \a msecs to utcDate and \a utcTime as appropriate. It is assumed that
 utcDate and utcTime are adjusted to UTC.

 \internal
 */
void QDateTimePrivate::addMSecs(QDate &utcDate, QTime &utcTime, qint64 msecs)
{
    qint64 dd = utcDate.toJulianDay();
    int tt = QTime(0, 0, 0).msecsTo(utcTime);
    int sign = 1;
    if (msecs < 0) {
        msecs = -msecs;
        sign = -1;
    }
    if (msecs >= int(MSECS_PER_DAY)) {
        dd += sign * (msecs / MSECS_PER_DAY);
        msecs %= MSECS_PER_DAY;
    }

    tt += sign * msecs;
    if (tt < 0) {
        tt = MSECS_PER_DAY - tt - 1;
        dd -= tt / MSECS_PER_DAY;
        tt = tt % MSECS_PER_DAY;
        tt = MSECS_PER_DAY - tt - 1;
    } else if (tt >= int(MSECS_PER_DAY)) {
        dd += tt / MSECS_PER_DAY;
        tt = tt % MSECS_PER_DAY;
    }

    utcDate = QDate::fromJulianDay(dd);
    utcTime = QTime(0, 0, 0).addMSecs(tt);
}

/*!
    Returns a QDateTime object containing a datetime \a s seconds
    later than the datetime of this object (or earlier if \a s is
    negative).

    \sa addMSecs(), secsTo(), addDays(), addMonths(), addYears()
*/

QDateTime QDateTime::addSecs(int s) const
{
    return d->addMSecs(*this, qint64(s) * 1000);
}

/*!
    Returns a QDateTime object containing a datetime \a msecs miliseconds
    later than the datetime of this object (or earlier if \a msecs is
    negative).

    \sa addSecs(), msecsTo(), addDays(), addMonths(), addYears()
*/
QDateTime QDateTime::addMSecs(qint64 msecs) const
{
    return d->addMSecs(*this, msecs);
}

/*!
    Returns the number of days from this datetime to the \a other
    datetime. If the \a other datetime is earlier than this datetime,
    the value returned is negative.

    \sa addDays(), secsTo(), msecsTo()
*/

int QDateTime::daysTo(const QDateTime &other) const
{
    return d->date.daysTo(other.d->date);
}

/*!
    Returns the number of seconds from this datetime to the \a other
    datetime. If the \a other datetime is earlier than this datetime,
    the value returned is negative.

    Before performing the comparison, the two datetimes are converted
    to Qt::UTC to ensure that the result is correct if one of the two
    datetimes has daylight saving time (DST) and the other doesn't.

    Returns 0 if either time is invalid.

    Example:
    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 11

    \sa addSecs(), daysTo(), QTime::secsTo()
*/

int QDateTime::secsTo(const QDateTime &other) const
{
    if (isValid() && other.isValid()) {
        QDate date1, date2;
        QTime time1, time2;

        d->getUTC(date1, time1);
        other.d->getUTC(date2, time2);

        return (date1.daysTo(date2) * SECS_PER_DAY) + time1.secsTo(time2);
    } else {
        return 0;
    }
}

/*!
    Returns the number of milliseconds from this datetime to the \a other
    datetime. If the \a other datetime is earlier than this datetime,
    the value returned is negative.

    Before performing the comparison, the two datetimes are converted
    to Qt::UTC to ensure that the result is correct if one of the two
    datetimes has daylight saving time (DST) and the other doesn't.

    Returns 0 if either time is null.

    \sa addMSecs(), daysTo(), QTime::msecsTo()
*/

qint64 QDateTime::msecsTo(const QDateTime &other) const
{
    if (isValid() && other.isValid()) {
        QDate selfDate;
        QDate otherDate;
        QTime selfTime;
        QTime otherTime;

        d->getUTC(selfDate, selfTime);
        other.d->getUTC(otherDate, otherTime);

        return (static_cast<qint64>(selfDate.daysTo(otherDate)) * static_cast<qint64>(MSECS_PER_DAY))
               + static_cast<qint64>(selfTime.msecsTo(otherTime));
    } else {
        return 0;
    }
}


/*!
    \fn QDateTime QDateTime::toTimeSpec(Qt::TimeSpec specification) const

    Returns a copy of this datetime configured to use the given time
    \a specification.

    \sa timeSpec(), toUTC(), toLocalTime()
*/

QDateTime QDateTime::toTimeSpec(Qt::TimeSpec spec) const
{
    if ((d->spec == QDateTimePrivate::UTC) == (spec == Qt::UTC))
        return *this;

    QDateTime ret;
    if (spec == Qt::UTC) {
        d->getUTC(ret.d->date, ret.d->time);
        ret.d->spec = QDateTimePrivate::UTC;
    } else {
        ret.d->spec = d->getLocal(ret.d->date, ret.d->time);
    }
    return ret;
}

/*!
    Returns true if this datetime is equal to the \a other datetime;
    otherwise returns false.

    \sa operator!=()
*/

bool QDateTime::operator==(const QDateTime &other) const
{
    if (d->spec == other.d->spec && d->utcOffset == other.d->utcOffset)
        return d->time == other.d->time && d->date == other.d->date;
    else {
        QDate date1, date2;
        QTime time1, time2;

        d->getUTC(date1, time1);
        other.d->getUTC(date2, time2);
        return time1 == time2 && date1 == date2;
    }
}

/*!
    \fn bool QDateTime::operator!=(const QDateTime &other) const

    Returns true if this datetime is different from the \a other
    datetime; otherwise returns false.

    Two datetimes are different if either the date, the time, or the
    time zone components are different.

    \sa operator==()
*/

/*!
    Returns true if this datetime is earlier than the \a other
    datetime; otherwise returns false.
*/

bool QDateTime::operator<(const QDateTime &other) const
{
    if (d->spec == other.d->spec && d->spec != QDateTimePrivate::OffsetFromUTC) {
        if (d->date != other.d->date)
            return d->date < other.d->date;
        return d->time < other.d->time;
    } else {
        QDate date1, date2;
        QTime time1, time2;
        d->getUTC(date1, time1);
        other.d->getUTC(date2, time2);
        if (date1 != date2)
            return date1 < date2;
        return time1 < time2;
    }
}

/*!
    \fn bool QDateTime::operator<=(const QDateTime &other) const

    Returns true if this datetime is earlier than or equal to the
    \a other datetime; otherwise returns false.
*/

/*!
    \fn bool QDateTime::operator>(const QDateTime &other) const

    Returns true if this datetime is later than the \a other datetime;
    otherwise returns false.
*/

/*!
    \fn bool QDateTime::operator>=(const QDateTime &other) const

    Returns true if this datetime is later than or equal to the
    \a other datetime; otherwise returns false.
*/

/*!
    \fn QDateTime QDateTime::currentDateTime()
    Returns the current datetime, as reported by the system clock, in
    the local time zone.

    \sa currentDateTimeUtc(), QDate::currentDate(), QTime::currentTime(), toTimeSpec()
*/

/*!
    \fn QDateTime QDateTime::currentDateTimeUtc()
    Returns the current datetime, as reported by the system clock, in
    UTC.

    \sa currentDateTime(), QDate::currentDate(), QTime::currentTime(), toTimeSpec()
*/

/*!
    \fn qint64 QDateTime::currentMSecsSinceEpoch()

    Returns the number of milliseconds since 1970-01-01T00:00:00 Universal
    Coordinated Time. This number is like the POSIX time_t variable, but
    expressed in milliseconds instead.

    \sa currentDateTime(), currentDateTimeUtc(), toTime_t(), toTimeSpec()
*/

static inline uint msecsFromDecomposed(int hour, int minute, int sec, int msec = 0)
{
    return MSECS_PER_HOUR * hour + MSECS_PER_MIN * minute + 1000 * sec + msec;
}

#if defined(Q_OS_WIN)
QDate QDate::currentDate()
{
    QDate d;
    SYSTEMTIME st;
    memset(&st, 0, sizeof(SYSTEMTIME));
    GetLocalTime(&st);
    d.jd = julianDayFromDate(st.wYear, st.wMonth, st.wDay);
    return d;
}

QTime QTime::currentTime()
{
    QTime ct;
    SYSTEMTIME st;
    memset(&st, 0, sizeof(SYSTEMTIME));
    GetLocalTime(&st);
    ct.setHMS(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#if defined(Q_OS_WINCE)
    ct.startTick = GetTickCount() % MSECS_PER_DAY;
#endif
    return ct;
}

QDateTime QDateTime::currentDateTime()
{
    QDate d;
    QTime t;
    SYSTEMTIME st;
    memset(&st, 0, sizeof(SYSTEMTIME));
    GetLocalTime(&st);
    d.jd = julianDayFromDate(st.wYear, st.wMonth, st.wDay);
    t.mds = msecsFromDecomposed(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    return QDateTime(d, t);
}

QDateTime QDateTime::currentDateTimeUtc()
{
    QDate d;
    QTime t;
    SYSTEMTIME st;
    memset(&st, 0, sizeof(SYSTEMTIME));
    GetSystemTime(&st);
    d.jd = julianDayFromDate(st.wYear, st.wMonth, st.wDay);
    t.mds = msecsFromDecomposed(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    return QDateTime(d, t, Qt::UTC);
}

qint64 QDateTime::currentMSecsSinceEpoch()
{
    QDate d;
    QTime t;
    SYSTEMTIME st;
    memset(&st, 0, sizeof(SYSTEMTIME));
    GetSystemTime(&st);

    return msecsFromDecomposed(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds) +
            qint64(julianDayFromGregorianDate(st.wYear, st.wMonth, st.wDay)
                   - julianDayFromGregorianDate(1970, 1, 1)) * Q_INT64_C(86400000);
}

#elif defined(Q_OS_SYMBIAN)
QDate QDate::currentDate()
{
    QDate d;
    TTime localTime;
    localTime.HomeTime();
    TDateTime localDateTime = localTime.DateTime();
    // months and days are zero indexed
    d.jd = julianDayFromDate(localDateTime.Year(), localDateTime.Month() + 1, localDateTime.Day() + 1 );
    return d;
}

QTime QTime::currentTime()
{
    QTime ct;
    TTime localTime;
    localTime.HomeTime();
    TDateTime localDateTime = localTime.DateTime();
    ct.mds = msecsFromDecomposed(localDateTime.Hour(), localDateTime.Minute(),
                                 localDateTime.Second(), localDateTime.MicroSecond() / 1000);
    return ct;
}

QDateTime QDateTime::currentDateTime()
{
    QDate d;
    QTime ct;
    TTime localTime;
    localTime.HomeTime();
    TDateTime localDateTime = localTime.DateTime();
    // months and days are zero indexed
    d.jd = julianDayFromDate(localDateTime.Year(), localDateTime.Month() + 1, localDateTime.Day() + 1);
    ct.mds = msecsFromDecomposed(localDateTime.Hour(), localDateTime.Minute(),
                                 localDateTime.Second(), localDateTime.MicroSecond() / 1000);
    return QDateTime(d, ct);
}

QDateTime QDateTime::currentDateTimeUtc()
{
    QDate d;
    QTime ct;
    TTime gmTime;
    gmTime.UniversalTime();
    TDateTime gmtDateTime = gmTime.DateTime();
    // months and days are zero indexed
    d.jd = julianDayFromDate(gmtDateTime.Year(), gmtDateTime.Month() + 1, gmtDateTime.Day() + 1);
    ct.mds = msecsFromDecomposed(gmtDateTime.Hour(), gmtDateTime.Minute(),
                                 gmtDateTime.Second(), gmtDateTime.MicroSecond() / 1000);
    return QDateTime(d, ct, Qt::UTC);
}

qint64 QDateTime::currentMSecsSinceEpoch()
{
    QDate d;
    QTime ct;
    TTime gmTime;
    gmTime.UniversalTime();
    TDateTime gmtDateTime = gmTime.DateTime();

    // according to the documentation, the value is:
    // "a date and time as a number of microseconds since midnight, January 1st, 0 AD nominal Gregorian"
    qint64 value = gmTime.Int64();

    // whereas 1970-01-01T00:00:00 is (in the same representation):
    //   ((1970 * 365) + (1970 / 4) - (1970 / 100) + (1970 / 400) - 13) * 86400 * 1000000
    static const qint64 unixEpoch = Q_INT64_C(0xdcddb30f2f8000);

    return (value - unixEpoch) / 1000;
}

#elif defined(Q_OS_UNIX)
QDate QDate::currentDate()
{
    QDate d;
    // posix compliant system
    time_t ltime;
    time(&ltime);
    struct tm *t = 0;

#if !defined(QT_NO_THREAD) && defined(_POSIX_THREAD_SAFE_FUNCTIONS)
    // use the reentrant version of localtime() where available
    tzset();
    struct tm res;
    t = localtime_r(&ltime, &res);
#else
    t = localtime(&ltime);
#endif // !QT_NO_THREAD && _POSIX_THREAD_SAFE_FUNCTIONS

    d.jd = julianDayFromDate(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    return d;
}

QTime QTime::currentTime()
{
    QTime ct;
    // posix compliant system
    struct timeval tv;
    gettimeofday(&tv, 0);
    time_t ltime = tv.tv_sec;
    struct tm *t = 0;

#if !defined(QT_NO_THREAD) && defined(_POSIX_THREAD_SAFE_FUNCTIONS)
    // use the reentrant version of localtime() where available
    tzset();
    struct tm res;
    t = localtime_r(&ltime, &res);
#else
    t = localtime(&ltime);
#endif
    Q_CHECK_PTR(t);

    ct.mds = msecsFromDecomposed(t->tm_hour, t->tm_min, t->tm_sec, tv.tv_usec / 1000);
    return ct;
}

QDateTime QDateTime::currentDateTime()
{
    // posix compliant system
    // we have milliseconds
    struct timeval tv;
    gettimeofday(&tv, 0);
    time_t ltime = tv.tv_sec;
    struct tm *t = 0;

#if !defined(QT_NO_THREAD) && defined(_POSIX_THREAD_SAFE_FUNCTIONS)
    // use the reentrant version of localtime() where available
    tzset();
    struct tm res;
    t = localtime_r(&ltime, &res);
#else
    t = localtime(&ltime);
#endif

    QDateTime dt;
    dt.d->time.mds = msecsFromDecomposed(t->tm_hour, t->tm_min, t->tm_sec, tv.tv_usec / 1000);

    dt.d->date.jd = julianDayFromDate(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    dt.d->spec = t->tm_isdst > 0  ? QDateTimePrivate::LocalDST :
                 t->tm_isdst == 0 ? QDateTimePrivate::LocalStandard :
                 QDateTimePrivate::LocalUnknown;
    return dt;
}

QDateTime QDateTime::currentDateTimeUtc()
{
    // posix compliant system
    // we have milliseconds
    struct timeval tv;
    gettimeofday(&tv, 0);
    time_t ltime = tv.tv_sec;
    struct tm *t = 0;

#if !defined(QT_NO_THREAD) && defined(_POSIX_THREAD_SAFE_FUNCTIONS)
    // use the reentrant version of localtime() where available
    struct tm res;
    t = gmtime_r(&ltime, &res);
#else
    t = gmtime(&ltime);
#endif

    QDateTime dt;
    dt.d->time.mds = msecsFromDecomposed(t->tm_hour, t->tm_min, t->tm_sec, tv.tv_usec / 1000);

    dt.d->date.jd = julianDayFromDate(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    dt.d->spec = QDateTimePrivate::UTC;
    return dt;
}

qint64 QDateTime::currentMSecsSinceEpoch()
{
    // posix compliant system
    // we have milliseconds
    struct timeval tv;
    gettimeofday(&tv, 0);
    return qint64(tv.tv_sec) * Q_INT64_C(1000) + tv.tv_usec / 1000;
}

#else
#error "What system is this?"
#endif

/*!
  Returns a datetime whose date and time are the number of \a seconds
  that have passed since 1970-01-01T00:00:00, Coordinated Universal
  Time (Qt::UTC). On systems that do not support time zones, the time
  will be set as if local time were Qt::UTC.

  \sa toTime_t(), setTime_t()
*/
QDateTime QDateTime::fromTime_t(uint seconds)
{
    QDateTime d;
    d.setTime_t(seconds);
    return d;
}

/*!
  Returns a datetime whose date and time are the number of milliseconds, \a msecs,
  that have passed since 1970-01-01T00:00:00.000, Coordinated Universal
  Time (Qt::UTC). On systems that do not support time zones, the time
  will be set as if local time were Qt::UTC.

  Note that there are possible values for \a msecs that lie outside the valid
  range of QDateTime, both negative and positive. The behavior of this
  function is undefined for those values.

  \sa toTime_t(), setTime_t()
*/
QDateTime QDateTime::fromMSecsSinceEpoch(qint64 msecs)
{
    QDateTime d;
    d.setMSecsSinceEpoch(msecs);
    return d;
}

/*!
 \internal

 Sets the offset from UTC to \a seconds, and also sets timeSpec() to
 Qt::OffsetFromUTC.

 The maximum and minimum offset is 14 positive or negative hours.  If
 \a seconds is larger or smaller than that, the result is undefined.

 0 as offset is identical to UTC. Therefore, if \a seconds is 0, the
 timeSpec() will be set to Qt::UTC. Hence the UTC offset always
 relates to UTC, and can never relate to local time.

 \sa isValid(), utcOffset()
 */
void QDateTime::setUtcOffset(int seconds)
{
    detach();

    /* The motivation to also setting d->spec is to ensure that the QDateTime
     * instance stay in well-defined states all the time, instead of that
     * we instruct the user to ensure it. */
    if(seconds == 0)
        d->spec = QDateTimePrivate::UTC;
    else
        d->spec = QDateTimePrivate::OffsetFromUTC;

    /* Even if seconds is 0 we assign it to utcOffset. */
    d->utcOffset = seconds;
}

/*!
 \internal

 Returns the UTC offset in seconds. If the timeSpec() isn't
 Qt::OffsetFromUTC, 0 is returned. However, since 0 is a valid UTC
 offset the return value of this function cannot be used to determine
 whether a utcOffset() is used or is valid, timeSpec() must be
 checked.

 Likewise, if this QDateTime() is invalid or if timeSpec() isn't
 Qt::OffsetFromUTC, 0 is returned.

 The UTC offset only applies if the timeSpec() is Qt::OffsetFromUTC.

 \sa isValid(), setUtcOffset()
 */
int QDateTime::utcOffset() const
{
    if(isValid() && d->spec == QDateTimePrivate::OffsetFromUTC)
        return d->utcOffset;
    else
        return 0;
}

#ifndef QT_NO_DATESTRING

static int fromShortMonthName(const QString &monthName)
{
    // Assume that English monthnames are the default
    for (int i = 0; i < 12; ++i) {
        if (monthName == QLatin1String(qt_shortMonthNames[i]))
            return i + 1;
    }
    // If English names can't be found, search the localized ones
    for (int i = 1; i <= 12; ++i) {
        if (monthName == QDate::shortMonthName(i))
            return i;
    }
    return -1;
}

/*!
    \fn QDateTime QDateTime::fromString(const QString &string, Qt::DateFormat format)

    Returns the QDateTime represented by the \a string, using the
    \a format given, or an invalid datetime if this is not possible.

    Note for Qt::TextDate: It is recommended that you use the
    English short month names (e.g. "Jan"). Although localized month
    names can also be used, they depend on the user's locale settings.
*/
QDateTime QDateTime::fromString(const QString& s, Qt::DateFormat f)
{
    if (s.isEmpty()) {
        return QDateTime();
    }

    switch (f) {
    case Qt::ISODate: {
        QString tmp = s;
        Qt::TimeSpec ts = Qt::LocalTime;
        const QDate date = QDate::fromString(tmp.left(10), Qt::ISODate);
        if (tmp.size() == 10)
            return QDateTime(date);

        tmp = tmp.mid(11);

        // Recognize UTC specifications
        if (tmp.endsWith(QLatin1Char('Z'))) {
            ts = Qt::UTC;
            tmp.chop(1);
        }

        // Recognize timezone specifications
        QRegExp rx(QLatin1String("[+-]"));
        if (tmp.contains(rx)) {
            int idx = tmp.indexOf(rx);
            QString tmp2 = tmp.mid(idx);
            tmp = tmp.left(idx);
            bool ok = true;
            int ntzhour = 1;
            int ntzminute = 3;
            if ( tmp2.indexOf(QLatin1Char(':')) == 3 )
               ntzminute = 4;
            const int tzhour(tmp2.mid(ntzhour, 2).toInt(&ok));
            const int tzminute(tmp2.mid(ntzminute, 2).toInt(&ok));
            QTime tzt(tzhour, tzminute);
            int utcOffset = (tzt.hour() * 60 + tzt.minute()) * 60;
            if ( utcOffset != 0 ) {
                ts = Qt::OffsetFromUTC;
                QDateTime dt(date, QTime::fromString(tmp, Qt::ISODate), ts);
                dt.setUtcOffset( utcOffset * (tmp2.startsWith(QLatin1Char('-')) ? -1 : 1) );
                return dt;
            }
        }
        return QDateTime(date, QTime::fromString(tmp, Qt::ISODate), ts);
    }
    case Qt::SystemLocaleDate:
    case Qt::SystemLocaleShortDate:
    case Qt::SystemLocaleLongDate:
        return fromString(s, QLocale::system().dateTimeFormat(f == Qt::SystemLocaleLongDate ? QLocale::LongFormat
                                                                                            : QLocale::ShortFormat));
    case Qt::LocaleDate:
    case Qt::DefaultLocaleShortDate:
    case Qt::DefaultLocaleLongDate:
        return fromString(s, QLocale().dateTimeFormat(f == Qt::DefaultLocaleLongDate ? QLocale::LongFormat
                                                                                     : QLocale::ShortFormat));
#if !defined(QT_NO_TEXTDATE)
    case Qt::TextDate: {
        QStringList parts = s.split(QLatin1Char(' '), QString::SkipEmptyParts);

        if ((parts.count() < 5) || (parts.count() > 6)) {
            return QDateTime();
        }

        // Accept "Sun Dec 1 13:02:00 1974" and "Sun 1. Dec 13:02:00 1974"
        int month = -1, day = -1;
        bool ok;

        month = fromShortMonthName(parts.at(1));
        if (month != -1) {
            day = parts.at(2).toInt(&ok);
            if (!ok)
                day = -1;
        }

        if (month == -1 || day == -1) {
            // first variant failed, lets try the other
            month = fromShortMonthName(parts.at(2));
            if (month != -1) {
                QString dayStr = parts.at(1);
                if (dayStr.endsWith(QLatin1Char('.'))) {
                    dayStr.chop(1);
                    day = dayStr.toInt(&ok);
                    if (!ok)
                        day = -1;
                } else {
                    day = -1;
                }
            }
        }

        if (month == -1 || day == -1) {
            // both variants failed, give up
            return QDateTime();
        }

        int year;
        QStringList timeParts = parts.at(3).split(QLatin1Char(':'));
        if ((timeParts.count() == 3) || (timeParts.count() == 2)) {
            year = parts.at(4).toInt(&ok);
            if (!ok)
                return QDateTime();
        } else {
            timeParts = parts.at(4).split(QLatin1Char(':'));
            if ((timeParts.count() != 3) && (timeParts.count() != 2))
                return QDateTime();
            year = parts.at(3).toInt(&ok);
            if (!ok)
                return QDateTime();
        }

        int hour = timeParts.at(0).toInt(&ok);
        if (!ok) {
            return QDateTime();
        }

        int minute = timeParts.at(1).toInt(&ok);
        if (!ok) {
            return QDateTime();
        }

        int second = (timeParts.count() > 2) ? timeParts.at(2).toInt(&ok) : 0;
        if (!ok) {
            return QDateTime();
        }

        QDate date(year, month, day);
        QTime time(hour, minute, second);

        if (parts.count() == 5)
            return QDateTime(date, time, Qt::LocalTime);

        QString tz = parts.at(5);
        if (!tz.startsWith(QLatin1String("GMT"), Qt::CaseInsensitive))
            return QDateTime();
        QDateTime dt(date, time, Qt::UTC);
        if (tz.length() > 3) {
            int tzoffset = 0;
            QChar sign = tz.at(3);
            if ((sign != QLatin1Char('+'))
                && (sign != QLatin1Char('-'))) {
                return QDateTime();
            }
            int tzhour = tz.mid(4, 2).toInt(&ok);
            if (!ok)
                return QDateTime();
            int tzminute = tz.mid(6).toInt(&ok);
            if (!ok)
                return QDateTime();
            tzoffset = (tzhour*60 + tzminute) * 60;
            if (sign == QLatin1Char('-'))
                tzoffset = -tzoffset;
            dt.setUtcOffset(tzoffset);
        }
        return dt.toLocalTime();
    }
#endif //QT_NO_TEXTDATE
    }

    return QDateTime();
}

/*!
    \fn QDateTime::fromString(const QString &string, const QString &format)

    Returns the QDateTime represented by the \a string, using the \a
    format given, or an invalid datetime if the string cannot be parsed.

    These expressions may be used for the date part of the format string:

    \table
    \header \i Expression \i Output
    \row \i d \i the day as number without a leading zero (1 to 31)
    \row \i dd \i the day as number with a leading zero (01 to 31)
    \row \i ddd
            \i the abbreviated localized day name (e.g. 'Mon' to 'Sun').
            Uses QDate::shortDayName().
    \row \i dddd
            \i the long localized day name (e.g. 'Monday' to 'Sunday').
            Uses QDate::longDayName().
    \row \i M \i the month as number without a leading zero (1-12)
    \row \i MM \i the month as number with a leading zero (01-12)
    \row \i MMM
            \i the abbreviated localized month name (e.g. 'Jan' to 'Dec').
            Uses QDate::shortMonthName().
    \row \i MMMM
            \i the long localized month name (e.g. 'January' to 'December').
            Uses QDate::longMonthName().
    \row \i yy \i the year as two digit number (00-99)
    \row \i yyyy \i the year as four digit number
    \endtable

    \note Unlike the other version of this function, day and month names must
    be given in the user's local language. It is only possible to use the English
    names if the user's language is English.

    These expressions may be used for the time part of the format string:

    \table
    \header \i Expression \i Output
    \row \i h
            \i the hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display)
    \row \i hh
            \i the hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)
    \row \i H
            \i the hour without a leading zero (0 to 23, even with AM/PM display)
    \row \i HH
            \i the hour with a leading zero (00 to 23, even with AM/PM display)
    \row \i m \i the minute without a leading zero (0 to 59)
    \row \i mm \i the minute with a leading zero (00 to 59)
    \row \i s \i the second without a leading zero (0 to 59)
    \row \i ss \i the second with a leading zero (00 to 59)
    \row \i z \i the milliseconds without leading zeroes (0 to 999)
    \row \i zzz \i the milliseconds with leading zeroes (000 to 999)
    \row \i AP or A
         \i interpret as an AM/PM time. \e AP must be either "AM" or "PM".
    \row \i ap or a
         \i Interpret as an AM/PM time. \e ap must be either "am" or "pm".
    \endtable

    All other input characters will be treated as text. Any sequence
    of characters that are enclosed in singlequotes will also be
    treated as text and not be used as an expression.

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 12

    If the format is not satisfied an invalid QDateTime is returned.
    The expressions that don't have leading zeroes (d, M, h, m, s, z) will be
    greedy. This means that they will use two digits even if this will
    put them outside the range and/or leave too few digits for other
    sections.

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 13

    This could have meant 1 January 00:30.00 but the M will grab
    two digits.

    For any field that is not represented in the format the following
    defaults are used:

    \table
    \header \i Field  \i Default value
    \row    \i Year   \i 1900
    \row    \i Month  \i 1 (January)
    \row    \i Day    \i 1
    \row    \i Hour   \i 0
    \row    \i Minute \i 0
    \row    \i Second \i 0
    \endtable

    For example:

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 14

    \sa QDate::fromString() QTime::fromString() QDate::toString()
    QDateTime::toString() QTime::toString()
*/

QDateTime QDateTime::fromString(const QString &string, const QString &format)
{
#ifndef QT_BOOTSTRAPPED
    QTime time;
    QDate date;

    QDateTimeParser dt(QVariant::DateTime, QDateTimeParser::FromString);
    if (dt.parseFormat(format) && dt.fromString(string, &date, &time))
        return QDateTime(date, time);
#else
    Q_UNUSED(string);
    Q_UNUSED(format);
#endif
    return QDateTime(QDate(), QTime(-1, -1, -1));
}

#endif // QT_NO_DATESTRING
/*!
    \fn QDateTime QDateTime::toLocalTime() const

    Returns a datetime containing the date and time information in
    this datetime, but specified using the Qt::LocalTime definition.

    \sa toTimeSpec()
*/

/*!
    \fn QDateTime QDateTime::toUTC() const

    Returns a datetime containing the date and time information in
    this datetime, but specified using the Qt::UTC definition.

    \sa toTimeSpec()
*/

/*! \internal
 */
void QDateTime::detach()
{
    d.detach();
}

/*****************************************************************************
  Date/time stream functions
 *****************************************************************************/

#ifndef QT_NO_DATASTREAM
/*!
    \relates QDate

    Writes the \a date to stream \a out.

    \sa {Serializing Qt Data Types}
*/

QDataStream &operator<<(QDataStream &out, const QDate &date)
{
    return out << (qint64)(date.jd);
}

/*!
    \relates QDate

    Reads a date from stream \a in into the \a date.

    \sa {Serializing Qt Data Types}
*/

QDataStream &operator>>(QDataStream &in, QDate &date)
{
    qint64 jd;
    in >> jd;
    date.jd = jd;
    return in;
}

/*!
    \relates QTime

    Writes \a time to stream \a out.

    \sa {Serializing Qt Data Types}
*/

QDataStream &operator<<(QDataStream &out, const QTime &time)
{
    return out << quint32(time.mds);
}

/*!
    \relates QTime

    Reads a time from stream \a in into the given \a time.

    \sa {Serializing Qt Data Types}
*/

QDataStream &operator>>(QDataStream &in, QTime &time)
{
    quint32 ds;
    in >> ds;
    time.mds = int(ds);
    return in;
}

/*!
    \relates QDateTime

    Writes \a dateTime to the \a out stream.

    \sa {Serializing Qt Data Types}
*/
QDataStream &operator<<(QDataStream &out, const QDateTime &dateTime)
{
    out << dateTime.d->date << dateTime.d->time;
    if (out.version() >= 7)
        out << (qint8)dateTime.d->spec;
    return out;
}

/*!
    \relates QDateTime

    Reads a datetime from the stream \a in into \a dateTime.

    \sa {Serializing Qt Data Types}
*/

QDataStream &operator>>(QDataStream &in, QDateTime &dateTime)
{
    dateTime.detach();

    qint8 ts = (qint8)QDateTimePrivate::LocalUnknown;
    in >> dateTime.d->date >> dateTime.d->time;
    if (in.version() >= 7)
        in >> ts;
    dateTime.d->spec = (QDateTimePrivate::Spec)ts;
    return in;
}
#endif // QT_NO_DATASTREAM



// checks if there is an unqoted 'AP' or 'ap' in the string
static bool hasUnquotedAP(const QString &f)
{
    const QLatin1Char quote('\'');
    bool inquote = false;
    const int max = f.size();
    for (int i=0; i<max; ++i) {
        if (f.at(i) == quote) {
            inquote = !inquote;
        } else if (!inquote && f.at(i).toUpper() == QLatin1Char('A')) {
            return true;
        }
    }
    return false;
}

#ifndef QT_NO_DATESTRING
/*****************************************************************************
  Some static function used by QDate, QTime and QDateTime
*****************************************************************************/

// Replaces tokens by their value. See QDateTime::toString() for a list of valid tokens
static QString getFmtString(const QString& f, const QTime* dt = 0, const QDate* dd = 0, bool am_pm = false)
{
    if (f.isEmpty())
        return QString();

    QString buf = f;
    int removed = 0;

    if (dt) {
        if (f.startsWith(QLatin1String("hh")) || f.startsWith(QLatin1String("HH"))) {
            const bool hour12 = f.at(0) == QLatin1Char('h') && am_pm;
            if (hour12 && dt->hour() > 12)
                buf = QString::number(dt->hour() - 12).rightJustified(2, QLatin1Char('0'), true);
            else if (hour12 && dt->hour() == 0)
                buf = QLatin1String("12");
            else
                buf = QString::number(dt->hour()).rightJustified(2, QLatin1Char('0'), true);
            removed = 2;
        } else if (f.at(0) == QLatin1Char('h') || f.at(0) == QLatin1Char('H')) {
            const bool hour12 = f.at(0) == QLatin1Char('h') && am_pm;
            if (hour12 && dt->hour() > 12)
                buf = QString::number(dt->hour() - 12);
            else if (hour12 && dt->hour() == 0)
                buf = QLatin1String("12");
            else
                buf = QString::number(dt->hour());
            removed = 1;
        } else if (f.startsWith(QLatin1String("mm"))) {
            buf = QString::number(dt->minute()).rightJustified(2, QLatin1Char('0'), true);
            removed = 2;
        } else if (f.at(0) == (QLatin1Char('m'))) {
            buf = QString::number(dt->minute());
            removed = 1;
        } else if (f.startsWith(QLatin1String("ss"))) {
            buf = QString::number(dt->second()).rightJustified(2, QLatin1Char('0'), true);
            removed = 2;
        } else if (f.at(0) == QLatin1Char('s')) {
            buf = QString::number(dt->second());
        } else if (f.startsWith(QLatin1String("zzz"))) {
            buf = QString::number(dt->msec()).rightJustified(3, QLatin1Char('0'), true);
            removed = 3;
        } else if (f.at(0) == QLatin1Char('z')) {
            buf = QString::number(dt->msec());
            removed = 1;
        } else if (f.at(0).toUpper() == QLatin1Char('A')) {
            const bool upper = f.at(0) == QLatin1Char('A');
            buf = dt->hour() < 12 ? QLatin1String("am") : QLatin1String("pm");
            if (upper)
                buf = buf.toUpper();
            if (f.size() > 1 && f.at(1).toUpper() == QLatin1Char('P') &&
                f.at(0).isUpper() == f.at(1).isUpper()) {
                removed = 2;
            } else {
                removed = 1;
            }
        }
    }

    if (dd) {
        if (f.startsWith(QLatin1String("dddd"))) {
            buf = dd->longDayName(dd->dayOfWeek());
            removed = 4;
        } else if (f.startsWith(QLatin1String("ddd"))) {
            buf = dd->shortDayName(dd->dayOfWeek());
            removed = 3;
        } else if (f.startsWith(QLatin1String("dd"))) {
            buf = QString::number(dd->day()).rightJustified(2, QLatin1Char('0'), true);
            removed = 2;
        } else if (f.at(0) == QLatin1Char('d')) {
            buf = QString::number(dd->day());
            removed = 1;
        } else if (f.startsWith(QLatin1String("MMMM"))) {
            buf = dd->longMonthName(dd->month());
            removed = 4;
        } else if (f.startsWith(QLatin1String("MMM"))) {
            buf = dd->shortMonthName(dd->month());
            removed = 3;
        } else if (f.startsWith(QLatin1String("MM"))) {
            buf = QString::number(dd->month()).rightJustified(2, QLatin1Char('0'), true);
            removed = 2;
        } else if (f.at(0) == QLatin1Char('M')) {
            buf = QString::number(dd->month());
            removed = 1;
        } else if (f.startsWith(QLatin1String("yyyy"))) {
            const int year = dd->year();
            buf = QString::number(qAbs(year)).rightJustified(4, QLatin1Char('0'));
            if(year > 0)
                removed = 4;
            else
            {
                buf.prepend(QLatin1Char('-'));
                removed = 5;
            }

        } else if (f.startsWith(QLatin1String("yy"))) {
            buf = QString::number(dd->year()).right(2).rightJustified(2, QLatin1Char('0'));
            removed = 2;
        }
    }
    if (removed == 0 || removed >= f.size()) {
        return buf;
    }

    return buf + getFmtString(f.mid(removed), dt, dd, am_pm);
}

// Parses the format string and uses getFmtString to get the values for the tokens. Ret
static QString fmtDateTime(const QString& f, const QTime* dt, const QDate* dd)
{
    const QLatin1Char quote('\'');
    if (f.isEmpty())
        return QString();
    if (dt && !dt->isValid())
        return QString();
    if (dd && !dd->isValid())
        return QString();

    const bool ap = hasUnquotedAP(f);

    QString buf;
    QString frm;
    QChar status(QLatin1Char('0'));

    for (int i = 0; i < (int)f.length(); ++i) {
        if (f.at(i) == quote) {
            if (status == quote) {
                if (i > 0 && f.at(i - 1) == quote)
                    buf += QLatin1Char('\'');
                status = QLatin1Char('0');
            } else {
                if (!frm.isEmpty()) {
                    buf += getFmtString(frm, dt, dd, ap);
                    frm.clear();
                }
                status = quote;
            }
        } else if (status == quote) {
            buf += f.at(i);
        } else if (f.at(i) == status) {
            if ((ap) && ((f.at(i) == QLatin1Char('P')) || (f.at(i) == QLatin1Char('p'))))
                status = QLatin1Char('0');
            frm += f.at(i);
        } else {
            buf += getFmtString(frm, dt, dd, ap);
            frm.clear();
            if ((f.at(i) == QLatin1Char('h')) || (f.at(i) == QLatin1Char('m'))
                || (f.at(i) == QLatin1Char('H'))
                || (f.at(i) == QLatin1Char('s')) || (f.at(i) == QLatin1Char('z'))) {
                status = f.at(i);
                frm += f.at(i);
            } else if ((f.at(i) == QLatin1Char('d')) || (f.at(i) == QLatin1Char('M')) || (f.at(i) == QLatin1Char('y'))) {
                status = f.at(i);
                frm += f.at(i);
            } else if ((ap) && (f.at(i) == QLatin1Char('A'))) {
                status = QLatin1Char('P');
                frm += f.at(i);
            } else  if((ap) && (f.at(i) == QLatin1Char('a'))) {
                status = QLatin1Char('p');
                frm += f.at(i);
            } else {
                buf += f.at(i);
                status = QLatin1Char('0');
            }
        }
    }

    buf += getFmtString(frm, dt, dd, ap);

    return buf;
}
#endif // QT_NO_DATESTRING

#ifdef Q_OS_WIN
static const int LowerYear = 1980;
#else
static const int LowerYear = 1970;
#endif
static const int UpperYear = 2037;

static QDate adjustDate(QDate date)
{
    QDate lowerLimit(LowerYear, 1, 2);
    QDate upperLimit(UpperYear, 12, 30);

    if (date > lowerLimit && date < upperLimit)
        return date;

    int month = date.month();
    int day = date.day();

    // neither 1970 nor 2037 are leap years, so make sure date isn't Feb 29
    if (month == 2 && day == 29)
        --day;

    if (date < lowerLimit)
        date.setDate(LowerYear, month, day);
    else
        date.setDate(UpperYear, month, day);

    return date;
}

static QDateTimePrivate::Spec utcToLocal(QDate &date, QTime &time)
{
    QDate fakeDate = adjustDate(date);

    // won't overflow because of fakeDate
    time_t secsSince1Jan1970UTC = toMSecsSinceEpoch_helper(fakeDate.toJulianDay(), QTime(0, 0, 0).msecsTo(time)) / 1000;
    tm *brokenDown = 0;

#if defined(Q_OS_WINCE)
    tm res;
    FILETIME utcTime = time_tToFt(secsSince1Jan1970UTC);
    FILETIME resultTime;
    FileTimeToLocalFileTime(&utcTime , &resultTime);
    SYSTEMTIME sysTime;
    FileTimeToSystemTime(&resultTime , &sysTime);

    res.tm_sec = sysTime.wSecond;
    res.tm_min = sysTime.wMinute;
    res.tm_hour = sysTime.wHour;
    res.tm_mday = sysTime.wDay;
    res.tm_mon = sysTime.wMonth - 1;
    res.tm_year = sysTime.wYear - 1900;
    brokenDown = &res;
#elif defined(Q_OS_SYMBIAN)
    // months and days are zero index based
    _LIT(KUnixEpoch, "19700000:000000.000000");
    TTimeIntervalSeconds utcOffset = User::UTCOffset();
    TTimeIntervalSeconds tTimeIntervalSecsSince1Jan1970UTC(secsSince1Jan1970UTC);
    TTime epochTTime;
    TInt err = epochTTime.Set(KUnixEpoch);
    tm res;
    if(err == KErrNone) {
        TTime utcTTime = epochTTime + tTimeIntervalSecsSince1Jan1970UTC;
        utcTTime = utcTTime + utcOffset;
        TDateTime utcDateTime = utcTTime.DateTime();
        res.tm_sec = utcDateTime.Second();
        res.tm_min = utcDateTime.Minute();
        res.tm_hour = utcDateTime.Hour();
        res.tm_mday = utcDateTime.Day() + 1; // non-zero based index for tm struct
        res.tm_mon = utcDateTime.Month();
        res.tm_year = utcDateTime.Year() - 1900;
        res.tm_isdst = 0;
        brokenDown = &res;
    }
#elif !defined(QT_NO_THREAD) && defined(_POSIX_THREAD_SAFE_FUNCTIONS)
    // use the reentrant version of localtime() where available
    tzset();
    tm res;
    brokenDown = localtime_r(&secsSince1Jan1970UTC, &res);
#elif defined(_MSC_VER) && _MSC_VER >= 1400
    tm res;
    if (!_localtime64_s(&res, &secsSince1Jan1970UTC))
        brokenDown = &res;
#else
    brokenDown = localtime(&secsSince1Jan1970UTC);
#endif
    if (!brokenDown) {
        date = QDate(1970, 1, 1);
        time = QTime();
        return QDateTimePrivate::LocalUnknown;
    } else {
        int deltaDays = fakeDate.daysTo(date);
        date = QDate(brokenDown->tm_year + 1900, brokenDown->tm_mon + 1, brokenDown->tm_mday);
        time = QTime(brokenDown->tm_hour, brokenDown->tm_min, brokenDown->tm_sec, time.msec());
        date = date.addDays(deltaDays);
        if (brokenDown->tm_isdst > 0)
            return QDateTimePrivate::LocalDST;
        else if (brokenDown->tm_isdst < 0)
            return QDateTimePrivate::LocalUnknown;
        else
            return QDateTimePrivate::LocalStandard;
    }
}

static void localToUtc(QDate &date, QTime &time, int isdst)
{
    if (!date.isValid())
        return;

    QDate fakeDate = adjustDate(date);

    tm localTM;
    localTM.tm_sec = time.second();
    localTM.tm_min = time.minute();
    localTM.tm_hour = time.hour();
    localTM.tm_mday = fakeDate.day();
    localTM.tm_mon = fakeDate.month() - 1;
    localTM.tm_year = fakeDate.year() - 1900;
    localTM.tm_isdst = (int)isdst;
#if defined(Q_OS_WINCE) || defined(Q_OS_SYMBIAN)
    time_t secsSince1Jan1970UTC = (toMSecsSinceEpoch_helper(fakeDate.toJulianDay(), QTime().msecsTo(time)) / 1000);
#else
#if defined(Q_OS_WIN)
    _tzset();
#endif
    time_t secsSince1Jan1970UTC = mktime(&localTM);
#endif
    tm *brokenDown = 0;
#if defined(Q_OS_WINCE)
    tm res;
    FILETIME localTime = time_tToFt(secsSince1Jan1970UTC);
    SYSTEMTIME sysTime;
    FileTimeToSystemTime(&localTime, &sysTime);
    FILETIME resultTime;
    LocalFileTimeToFileTime(&localTime , &resultTime);
    FileTimeToSystemTime(&resultTime , &sysTime);
    res.tm_sec = sysTime.wSecond;
    res.tm_min = sysTime.wMinute;
    res.tm_hour = sysTime.wHour;
    res.tm_mday = sysTime.wDay;
    res.tm_mon = sysTime.wMonth - 1;
    res.tm_year = sysTime.wYear - 1900;
    res.tm_isdst = (int)isdst;
    brokenDown = &res;
#elif defined(Q_OS_SYMBIAN)
    // months and days are zero index based
    _LIT(KUnixEpoch, "19700000:000000.000000");
    TTimeIntervalSeconds utcOffset = TTimeIntervalSeconds(0 - User::UTCOffset().Int());
    TTimeIntervalSeconds tTimeIntervalSecsSince1Jan1970UTC(secsSince1Jan1970UTC);
    TTime epochTTime;
    TInt err = epochTTime.Set(KUnixEpoch);
    tm res;
    if(err == KErrNone) {
        TTime utcTTime = epochTTime + tTimeIntervalSecsSince1Jan1970UTC;
        utcTTime = utcTTime + utcOffset;
        TDateTime utcDateTime = utcTTime.DateTime();
        res.tm_sec = utcDateTime.Second();
        res.tm_min = utcDateTime.Minute();
        res.tm_hour = utcDateTime.Hour();
        res.tm_mday = utcDateTime.Day() + 1; // non-zero based index for tm struct
        res.tm_mon = utcDateTime.Month();
        res.tm_year = utcDateTime.Year() - 1900;
        res.tm_isdst = (int)isdst;
        brokenDown = &res;
    }
#elif !defined(QT_NO_THREAD) && defined(_POSIX_THREAD_SAFE_FUNCTIONS)
    // use the reentrant version of gmtime() where available
    tm res;
    brokenDown = gmtime_r(&secsSince1Jan1970UTC, &res);
#elif defined(_MSC_VER) && _MSC_VER >= 1400
    tm res;
    if (!_gmtime64_s(&res, &secsSince1Jan1970UTC))
        brokenDown = &res;
#else
    brokenDown = gmtime(&secsSince1Jan1970UTC);
#endif // !QT_NO_THREAD && _POSIX_THREAD_SAFE_FUNCTIONS
    if (!brokenDown) {
        date = QDate(1970, 1, 1);
        time = QTime();
    } else {
        int deltaDays = fakeDate.daysTo(date);
        date = QDate(brokenDown->tm_year + 1900, brokenDown->tm_mon + 1, brokenDown->tm_mday);
        time = QTime(brokenDown->tm_hour, brokenDown->tm_min, brokenDown->tm_sec, time.msec());
        date = date.addDays(deltaDays);
    }
}

QDateTimePrivate::Spec QDateTimePrivate::getLocal(QDate &outDate, QTime &outTime) const
{
    outDate = date;
    outTime = time;
    if (spec == QDateTimePrivate::UTC)
        return utcToLocal(outDate, outTime);
    return spec;
}

void QDateTimePrivate::getUTC(QDate &outDate, QTime &outTime) const
{
    outDate = date;
    outTime = time;
    const bool isOffset = spec == QDateTimePrivate::OffsetFromUTC;

    if (spec != QDateTimePrivate::UTC && !isOffset)
        localToUtc(outDate, outTime, (int)spec);

    if (isOffset)
        addMSecs(outDate, outTime, -(qint64(utcOffset) * 1000));
}

#if !defined(QT_NO_DEBUG_STREAM) && !defined(QT_NO_DATESTRING)
QDebug operator<<(QDebug dbg, const QDate &date)
{
    dbg.nospace() << "QDate(" << date.toString() << ')';
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const QTime &time)
{
    dbg.nospace() << "QTime(" << time.toString() << ')';
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const QDateTime &date)
{
    dbg.nospace() << "QDateTime(" << date.toString() << ')';
    return dbg.space();
}
#endif

#ifndef QT_BOOTSTRAPPED

#ifdef Q_OS_SYMBIAN
const static TTime UnixEpochOffset(I64LIT(0xdcddb30f2f8000));
const static TInt64 MinimumMillisecondTime(KMinTInt64 / 1000);
const static TInt64 MaximumMillisecondTime(KMaxTInt64 / 1000);
QDateTime qt_symbian_TTime_To_QDateTime(const TTime& time)
{
    TTimeIntervalMicroSeconds absolute = time.MicroSecondsFrom(UnixEpochOffset);

    return QDateTime::fromMSecsSinceEpoch(absolute.Int64() / 1000);
}

TTime qt_symbian_QDateTime_To_TTime(const QDateTime& datetime)
{
    qint64 absolute = datetime.toMSecsSinceEpoch();
    if(absolute > MaximumMillisecondTime)
        return TTime(KMaxTInt64);
    if(absolute < MinimumMillisecondTime)
        return TTime(KMinTInt64);
    return TTime(absolute * 1000);
}

time_t qt_symbian_TTime_To_time_t(const TTime& time)
{
    TTimeIntervalSeconds interval;
    TInt err = time.SecondsFrom(UnixEpochOffset, interval);
    if (err || interval.Int() < 0)
        return (time_t) 0;
    return (time_t) interval.Int();
}

TTime qt_symbian_time_t_To_TTime(time_t time)
{
    return UnixEpochOffset + TTimeIntervalSeconds(time);
}
#endif //Q_OS_SYMBIAN

#endif // QT_BOOTSTRAPPED

QT_END_NAMESPACE
