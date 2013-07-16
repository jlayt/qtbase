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

#ifndef QDATETIMEFORMATTER_H_P
#define QDATETIMEFORMATTER_H_P

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of internal files.  This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.
//

#include "qlocale.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QDateTimeFormatterPrivate;

// Internal for now, make public later
// Implement full ICU support, parts may not be made public later

class Q_CORE_EXPORT QDateTimeFormatter
{
public:
    // TODO Move these enums to QLocale or Qt

    // CLDR format length attribute for date/time
    // ICU UDateDateTimeStyle plus custom formats
    enum DateTimeStyle {
        DefaultDateTimeStyle = 0,
        FullStyle,
        LongStyle,
        MediumStyle,
        ShortStyle,
        FullRelativeStyle,
        LongRelativeStyle,
        MediumRelativeStyle,
        ShortRelativeStyle,
        IsoStyle,
        IsoWeekStyle,
        IsoOrdinalStyle,
        Rfc2822Style,
        Rfc3339Style,
        LastDateTimeStyle = Rfc3339Style
    };

    enum DateTimeComponent {
        Year          = 0x1,
        YearName      = 0x2,
        Month         = 0x4,
        MonthName     = 0x8,
        Day           = 0x10,
        DayName       = 0x20,
        JulianDay     = 0x40,
        EraName       = 0x80,
        EraYear       = 0x100,
        YearInEra     = 0x200,
        DayOfYear     = 0x400,
        DayOfYearName = 0x800,
        DayOfWeek     = 0x1000,
        DayOfWeekName = 0x2000,
        Week          = 0x4000,
        WeekYear      = 0x8000,
        MonthsInYear  = 0x10000,
        WeeksInYear   = 0x20000,
        DaysInYear    = 0x40000,
        DaysInMonth   = 0x80000,
        DaysInWeek    = 0x100000,
        Hour          = 0x200000,
        Minute        = 0x400000,
        Second        = 0x800000,
        Millisecond   = 0x1000000,
        DayPeriod     = 0x2000000,
        DayPeriodHour = 0x4000000,
        Timezone      = 0x8000000,
        TimezoneName  = 0x10000000,
        UnixTime      = 0x20000000 
    };
    Q_DECLARE_FLAGS(DateTimeComponents, DateTimeComponent)

    // CLDR field width attribute
    // ICU ???
    enum ComponentStyle {
        DefaultComponentStyle = 0,
        LongName,           // e.g. January
        ShortName,          // e.g. Jan
        NarrowName,         // e.g. J
        LongNumber,         // e.g. 01
        ShortNumber         // e.g. 1
    };

    // CLDR context attribute
    // ICU ???
    enum ComponentContext {
        DefaultContext = 0,
        StyleContext,         // Use in a style
        StandaloneContext     // Use standalone
    };

    // Have QCalendarSystem API to return yearType() ???
    enum YearType {
        StandardYear,
        LeapYear
    };

    enum Calendar {
        DefaultCalendar   = 0,
        GregorianCalendar = 1,
        LastCalendar      = GregorianCalendar
    };

    // Create default style and locale
    QDateTimeFormatter();
    // Create named style and locale
    QDateTimeFormatter(QDateTimeFormatter::DateTimeStyle style, const QString &localeCode = QString());
    ~QDateTimeFormatter();

    QDateTimeFormatter(const QDateTimeFormatter &other);
    QDateTimeFormatter &operator=(const QDateTimeFormatter &other);

    bool isValid() const;

    QString localeCode(QLocale::LocaleCodeType type = QLocale::RequestedLocaleCode) const;

    QDateTimeFormatter::DateTimeStyle style() const;

    QString datePattern() const;
    QString timePattern() const;
    QString dateTimePattern() const;

    qint32 shortYearWindowStart() const;

    QLocale::ParsingMode parsingMode() const;

    // Format with default
    QString toString(const QDate &date) const;
    QString toString(const QTime &time) const;
    QString toString(const QDateTime &dateTime) const;

    // Format with override pattern
    QString toString(const QDate &date, const QString &pattern) const;
    QString toString(const QTime &time, const QString &pattern) const;
    QString toString(const QDateTime &dateTime, const QString &pattern) const;

    // Parse with default
    QDate toDate(const QString &string,
                 QLocale::ParsingMode mode = QLocale::DefaultParsingMode) const;
    QTime toTime(const QString &string,
                 QLocale::ParsingMode mode = QLocale::DefaultParsingMode) const;
    QDateTime toDateTime(const QString &string,
                         QLocale::ParsingMode mode = QLocale::DefaultParsingMode) const;

    // Parse with override pattern
    QDate toDate(const QString &string, const QString &pattern,
                 QLocale::ParsingMode mode = QLocale::DefaultParsingMode) const;
    QTime toTime(const QString &string, const QString &pattern,
                 QLocale::ParsingMode mode = QLocale::DefaultParsingMode) const;
    QDateTime toDateTime(const QString &string, const QString &pattern,
                         QLocale::ParsingMode mode = QLocale::DefaultParsingMode) const;

    QString eraName(int era,
                    QDateTimeFormatter::ComponentStyle style = QDateTimeFormatter::DefaultComponentStyle,
                    QDateTimeFormatter::ComponentContext context = QDateTimeFormatter::DefaultContext) const;

    QString quarterName(int quarter,
                        QDateTimeFormatter::ComponentStyle style = QDateTimeFormatter::DefaultComponentStyle,
                        QDateTimeFormatter::ComponentContext context = QDateTimeFormatter::DefaultContext) const;

    QString monthName(int month,
                      QDateTimeFormatter::ComponentStyle style = QDateTimeFormatter::DefaultComponentStyle,
                      QDateTimeFormatter::ComponentContext context = QDateTimeFormatter::DefaultContext) const;

    QString weekdayName(int day,
                        QDateTimeFormatter::ComponentStyle style = QDateTimeFormatter::DefaultComponentStyle,
                        QDateTimeFormatter::ComponentContext context = QDateTimeFormatter::DefaultContext) const;

    QString dayPeriodName(const QTime &time,
                          QDateTimeFormatter::ComponentStyle style = QDateTimeFormatter::DefaultComponentStyle,
                          QDateTimeFormatter::ComponentContext context = QDateTimeFormatter::DefaultContext) const;

private:
    // Create locale with backend
    QDateTimeFormatter(QDateTimeFormatterPrivate &dd);
    friend class QDateTimeFormatterPrivate;
    friend class QCustomDateTimeFormatter;
    QSharedDataPointer<QDateTimeFormatterPrivate> d;
};

class Q_CORE_EXPORT QCustomDateTimeFormatter : public QDateTimeFormatter
{
public:
    // Create named date style with named locale
    QCustomDateTimeFormatter(QDateTimeFormatter::DateTimeStyle style,
                             const QString &localeCode = QString());
    ~QCustomDateTimeFormatter();

    QCustomDateTimeFormatter(const QCustomDateTimeFormatter &other);
    QCustomDateTimeFormatter &operator=(const QCustomDateTimeFormatter &other);

    void setDatePattern(const QString & pattern);
    void setTimePattern(const QString & pattern);
    void setDateTimePattern(const QString & pattern);
    void setShortYearWindowStart(qint32 year);
    void setParsingMode(QLocale::ParsingMode mode);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDATETIMEFORMATTER_H_P
