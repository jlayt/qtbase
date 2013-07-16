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

// ICU Utilities

ULocDataLocaleType qt_localeTypeToIcuType2(QLocale::LocaleCodeType type)
{
    switch (type) {
    case QLocale::RequestedLocaleCode :
        return ULOC_REQUESTED_LOCALE ;
    case QLocale::EffectiveLocaleCode :
        return ULOC_VALID_LOCALE;
    }
}

UDateFormatStyle qt_styleToIcuStyle(QDateTimeFormatter::DateTimeStyle style)
{
    switch (style) {
    case QDateTimeFormatter::DefaultDateTimeStyle :
        return UDAT_DEFAULT;  // == UDAT_MEDIUM
    case QDateTimeFormatter::FullStyle :
        return UDAT_FULL;
    case QDateTimeFormatter::LongStyle :
        return UDAT_LONG;
    case QDateTimeFormatter::MediumStyle :
        return UDAT_MEDIUM;
    case QDateTimeFormatter::ShortStyle :
        return UDAT_SHORT;
    case QDateTimeFormatter::FullRelativeStyle :
        return UDAT_FULL_RELATIVE;
    case QDateTimeFormatter::LongRelativeStyle :
        return UDAT_LONG_RELATIVE;
    case QDateTimeFormatter::MediumRelativeStyle :
        return UDAT_MEDIUM_RELATIVE;
    case QDateTimeFormatter::ShortRelativeStyle :
        return UDAT_SHORT_RELATIVE;
    case QDateTimeFormatter::IsoStyle :
    case QDateTimeFormatter::IsoWeekStyle :
    case QDateTimeFormatter::IsoOrdinalStyle :
    case QDateTimeFormatter::Rfc2822Style :
    case QDateTimeFormatter::Rfc3339Style :
        return UDAT_NONE;
    default :
        return UDAT_DEFAULT;  // == UDAT_MEDIUM
    }
}

UDate qt_dateToUDate(const QDate &date)
{
    return (date.toJulianDay() - 2440588) * U_MILLIS_PER_DAY;
}

UDate qt_timeToUDate(const QTime &time)
{
    return QTime(0, 0, 0, 0).msecsTo(time);
}

UDate qt_dateTimeToUDate(const QDateTime &dateTime)
{
    return dateTime.toMSecsSinceEpoch();
}

QDate qt_uDateToQDate(UDate date, UDateFormat *df)
{
    // TODO or can we just convert direct to JD without Cal?
    UErrorCode status = U_ZERO_ERROR;
    UCalendar *cal = ucal_clone(udat_getCalendar(df), &status);
    if (U_SUCCESS(status)) {
        status = U_ZERO_ERROR;
        ucal_setMillis(cal, date, &status);
        if (U_SUCCESS(status)) {
            status = U_ZERO_ERROR;
            return QDate::fromJulianDay(ucal_get(cal, UCAL_JULIAN_DAY, &status));
        }
    }
    return QDate();
}

QTime qt_uDateToQTime(UDate date)
{
    return QTime(0,0,0,0).addMSecs((int)date);
}

QDateTime qt_uDateToQDateTime(UDate date)
{
    // TODO Need to include time zone?
    QDateTime dt;
    dt.setMSecsSinceEpoch(date);
    return dt;
}

QString qt_customPattern(QMetaType::Type type, QDateTimeFormatter::DateTimeStyle style)
{
    if (type == QMetaType::QDate) {
        switch (style) {
        case QDateTimeFormatter::IsoStyle :
            return "yyyy-MM-dd";
        case QDateTimeFormatter::IsoWeekStyle :
            return "yyyy-'W'ww-E";
        case QDateTimeFormatter::IsoOrdinalStyle :
            return "yyyy-DDD";
        case QDateTimeFormatter::Rfc2822Style :
            return "EEE, dd MMM yyyy";
        case QDateTimeFormatter::Rfc3339Style :
            return "yyyy-MM-dd";
        default :
            return QString();
        }
    } else if (type == QMetaType::QTime) {
        switch (style) {
        case QDateTimeFormatter::IsoStyle :
        case QDateTimeFormatter::IsoWeekStyle :
        case QDateTimeFormatter::IsoOrdinalStyle :
        case QDateTimeFormatter::Rfc2822Style :
        case QDateTimeFormatter::Rfc3339Style :
            return "HH:mm:ss";
        default :
            return QString();
        }
    } else if (type == QMetaType::QDateTime) {
        switch (style) {
        case QDateTimeFormatter::IsoStyle :
            return "yyyy-MM-dd'T'HH:mm:ss.SSSZZZZZ";
        case QDateTimeFormatter::IsoWeekStyle :
            return "yyyy-'W'ww-E'T'HH:mm:ss.SSSZZZZZ";
        case QDateTimeFormatter::IsoOrdinalStyle :
            return "yyyy-DDD'T'HH:mm:ss.SSSZZZZZ";
        case QDateTimeFormatter::Rfc2822Style :
            return "EEE, dd MMM yyyy HH:mm:ss Z";
        case QDateTimeFormatter::Rfc3339Style :
            return "yyyy-MM-dd'T'HH:mm:ss.SSSZZZZZZ";
        default :
            return QString();
        }
    }
    return QString();
}

QString qt_udat_toPattern(UDateFormat *df)
{
    if (df) {
        int32_t size = 50;
        QString result(size, Qt::Uninitialized);
        UErrorCode status = U_ZERO_ERROR;

        // udat_toPattern (df, localized, result, size, &status)
        udat_toPattern(df, false, reinterpret_cast<UChar *>(result.data()), size, &status);

        // If overflow, then resize and retry
        if (status == U_BUFFER_OVERFLOW_ERROR) {
            result.resize(size);
            status = U_ZERO_ERROR;
            udat_toPattern(df, false, reinterpret_cast<UChar *>(result.data()), size, &status);
        }

        // If successful on first or second go, resize and return
        if (U_SUCCESS(status)) {
            result.resize(size);
            return result;
        }
    }

    return QString();
}

void qt_udat_applyPattern(UDateFormat *df, const QString &value)
{
    if (df && !value.isEmpty()) {
        UErrorCode status = U_ZERO_ERROR;
        // udat_applyPattern(df, localized, value, valueSize, error, status)
        udat_applyPattern(df, FALSE, reinterpret_cast<const UChar *>(value.constData()),
                        value.size());
    }
}

UDateFormat *qt_udat_open(const QString &locale, QMetaType::Type type, QDateTimeFormatter::DateTimeStyle style)
{
    QString icuLocale = locale;
    UDateFormatStyle dateStyle = UDAT_NONE;
    UDateFormatStyle timeStyle = UDAT_NONE;

    if (type != QMetaType::QDate)
        timeStyle = qt_styleToIcuStyle(style);

    if (type != QMetaType::QTime)
        dateStyle = qt_styleToIcuStyle(style);

    // RFC formats require English symbols, don't use en_US as may have Americanised features
    if (style == QDateTimeFormatter::Rfc2822Style || style == QDateTimeFormatter::Rfc3339Style)
        icuLocale = "en_GB";

    UErrorCode status = U_ZERO_ERROR;
    UDateFormat *df = udat_open(timeStyle, dateStyle, icuLocale.toUtf8(),
                                 NULL, -1, NULL, -1, &status);

    if (!U_SUCCESS(status))
        return 0;

    // Apply standard style pattern, doesn't work if passed directly into udat_open?
    QString pattern = qt_customPattern(type, style);
    if (!pattern.isEmpty())
        qt_udat_applyPattern(df, pattern);

    // TODO set standard formats to Gregorian calendar and ISO weeks?

    return df;
}

QString qt_udat_getLocaleByType(UDateFormat *df, ULocDataLocaleType type)
{
    if (!df)
        return QString();

    UErrorCode status = U_ZERO_ERROR;
    // udat_getLocaleByType(df, type, error)
    QString locale = udat_getLocaleByType(df, type , &status);
    return locale;
}

QString qt_udat_getSymbols(UDateFormat *df, UDateFormatSymbolType type, qint32 index)
{
    if (df) {
        int32_t size = 20;
        QString result(size, Qt::Uninitialized);
        UErrorCode status = U_ZERO_ERROR;

        // size = udat_getSymbols(df, type, index, result, resultLength, status)
        size = udat_getSymbols(df, type, index, reinterpret_cast<UChar *>(result.data()), size, &status);

        // If overflow, then resize and retry
        if (status == U_BUFFER_OVERFLOW_ERROR) {
            result.resize(size);
            status = U_ZERO_ERROR;
            size = udat_getSymbols(df, type, index, reinterpret_cast<UChar *>(result.data()), size, &status);
        }

        // If successful on first or second go, resize and return
        if (U_SUCCESS(status)) {
            result.resize(size);
            return result;
        }
    }

    return QString();
}

QString qt_udat_format(UDateFormat *df, UDate date)
{
    if (df) {
        int32_t size = 30;
        QString result(size, Qt::Uninitialized);
        UErrorCode status = U_ZERO_ERROR;

        // size = udat_format(df, date, result, resultLength, pos, status)
        size = udat_format(df, date, reinterpret_cast<UChar *>(result.data()), size, NULL, &status);

        // If overflow, then resize and retry
        if (status == U_BUFFER_OVERFLOW_ERROR) {

            result.resize(size);
            status = U_ZERO_ERROR;
            size = udat_format(df, date, reinterpret_cast<UChar *>(result.data()), size, NULL, &status);
        }

        // If successful on first or second go, resize and return
        if (U_SUCCESS(status)) {
            result.resize(size);

            return result;
        }
    }

    return QString();
}

QLocale::ParsingMode qt_udat_parsingMode(UDateFormat *df)
{
    if (udat_isLenient(df))
        return QLocale::LenientParsing;
    return QLocale::StrictParsing;
}

UDateFormat *qt_parseFormatter(UDateFormat *mdf, const QString &pattern, QLocale::ParsingMode mode)
{
    // If same parsing mode and null pattern then use the default formatter
    if ((mode == QLocale::DefaultParsingMode || mode == qt_udat_parsingMode(mdf)) && pattern.isEmpty())
        return mdf;

    // If not same parsing mode or override pattern then clone the formatter
    UErrorCode status = U_ZERO_ERROR;
    UDateFormat *df = udat_clone(mdf, &status);
    if (!U_SUCCESS(status))
        return 0;
    if (mode != QLocale::DefaultParsingMode)
        udat_setLenient(df, (mode == QLocale::LenientParsing));
    qt_udat_applyPattern(df, pattern);
    return df;
}

QDate qt_udat_parseDate(UDateFormat *mdf, const QString &from,
                        const QString &pattern, QLocale::ParsingMode mode)
{
    // If not current parsing mode or pattern then clone the formatter
    UDateFormat *df = qt_parseFormatter(mdf, pattern, mode);

    if (df) {
        UErrorCode status = U_ZERO_ERROR;
        int pos = 0;

        // udat_parse(df, text, textLength, pos, status)
        UDate res = udat_parse(df, reinterpret_cast<const UChar *>(from.constData()),
                               from.size(), &pos, &status);

        if (U_SUCCESS(status))
            return qt_uDateToQDate(res, df);
    }

    return QDate();
}

QTime qt_udat_parseTime(UDateFormat *mdf, const QString &from,
                        const QString &pattern, QLocale::ParsingMode mode)
{
    // If not current parsing mode or pattern then clone the formatter
    UDateFormat *df = qt_parseFormatter(mdf, pattern, mode);

    if (df) {
        UErrorCode status = U_ZERO_ERROR;
        int pos = 0;

        // udat_parse(df, text, textLength, pos, status)
        UDate res = udat_parse(df, reinterpret_cast<const UChar *>(from.constData()),
                               from.size(), &pos, &status);

        if (U_SUCCESS(status))
            return qt_uDateToQTime(res);
    }

    return QTime();
}

QDateTime qt_udat_parseDateTime(UDateFormat *mdf, const QString &from,
                                const QString &pattern, QLocale::ParsingMode mode)
{
    // If not current parsing mode or pattern then clone the formatter
    UDateFormat *df = qt_parseFormatter(mdf, pattern, mode);

    if (df) {
        UErrorCode status = U_ZERO_ERROR;
        int pos = 0;

        // udat_parse(df, text, textLength, pos, status)
        UDate res = udat_parse(df, reinterpret_cast<const UChar *>(from.constData()),
                            from.size(), &pos, &status);

        if (U_SUCCESS(status))
            return qt_uDateToQDateTime(res);
    }

    return QDateTime();
}

void qt_printSymbols(UDateFormat *df, const QString &text, UDateFormatSymbolType type)
{
    QString out;
    for (int i = 0; i < udat_countSymbols(df, type); ++i)
        out = out.append(qt_udat_getSymbols(df, type, i)).append(", ");
    qDebug() << text << out;
}

void qt_printDateTimeSettings(UDateFormat *df)
{
    qDebug() << "ULOC_REQUESTED_LOCALE           "
             << qt_udat_getLocaleByType(df, ULOC_REQUESTED_LOCALE);
    qDebug() << "ULOC_VALID_LOCALE               "
             << qt_udat_getLocaleByType(df, ULOC_VALID_LOCALE);
    qDebug() << "ULOC_ACTUAL_LOCALE              "
             << qt_udat_getLocaleByType(df, ULOC_ACTUAL_LOCALE);
    qDebug() << "Pattern                         "
             << qt_udat_toPattern(df);
    qDebug() << "Is Lenient                      "
             << udat_isLenient(df);
    qt_printSymbols(df, "UDAT_ERAS                       ", UDAT_ERAS);
    qt_printSymbols(df, "UDAT_ERA_NAMES                  ", UDAT_ERA_NAMES);
    qt_printSymbols(df, "UDAT_MONTHS                     ", UDAT_MONTHS);
    qt_printSymbols(df, "UDAT_SHORT_MONTHS               ", UDAT_SHORT_MONTHS);
    qt_printSymbols(df, "UDAT_NARROW_MONTHS              ", UDAT_NARROW_MONTHS);
    qt_printSymbols(df, "UDAT_STANDALONE_MONTHS          ", UDAT_STANDALONE_MONTHS);
    qt_printSymbols(df, "UDAT_STANDALONE_SHORT_MONTHS    ", UDAT_STANDALONE_SHORT_MONTHS);
    qt_printSymbols(df, "UDAT_STANDALONE_NARROW_MONTHS   ", UDAT_STANDALONE_NARROW_MONTHS);
    qt_printSymbols(df, "UDAT_WEEKDAYS                   ", UDAT_WEEKDAYS);
    qt_printSymbols(df, "UDAT_SHORT_WEEKDAYS             ", UDAT_SHORT_WEEKDAYS);
    qt_printSymbols(df, "UDAT_NARROW_WEEKDAYS            ", UDAT_NARROW_WEEKDAYS);
    qt_printSymbols(df, "UDAT_STANDALONE_WEEKDAYS        ", UDAT_STANDALONE_WEEKDAYS);
    qt_printSymbols(df, "UDAT_STANDALONE_SHORT_WEEKDAYS  ", UDAT_STANDALONE_SHORT_WEEKDAYS);
    qt_printSymbols(df, "UDAT_STANDALONE_NARROW_WEEKDAYS ", UDAT_STANDALONE_NARROW_WEEKDAYS);
    qt_printSymbols(df, "UDAT_QUARTERS                   ", UDAT_QUARTERS);
    qt_printSymbols(df, "UDAT_SHORT_QUARTERS             ", UDAT_SHORT_QUARTERS);
    qt_printSymbols(df, "UDAT_STANDALONE_QUARTERS        ", UDAT_STANDALONE_QUARTERS);
    qt_printSymbols(df, "UDAT_STANDALONE_SHORT_QUARTERS  ", UDAT_STANDALONE_SHORT_QUARTERS);
    qt_printSymbols(df, "UDAT_AM_PMS                     ", UDAT_AM_PMS);
    qt_printSymbols(df, "UDAT_LOCALIZED_CHARS            ", UDAT_LOCALIZED_CHARS);
}

QDateTimeFormatterPrivate::QDateTimeFormatterPrivate(QDateTimeFormatter::DateTimeStyle style,
                                                     const QString &locale)
{
    m_locale = locale;

    // ICU defines UDAT_DEFAULT == UDAT_MEDIUM
    if (style == QDateTimeFormatter::DefaultDateTimeStyle)
        m_style = QDateTimeFormatter::MediumStyle;
    else
        m_style = style;

    // TODO Set date and time formatters to have no time zone, i.e.assume GMT???
    m_dateFormatter = qt_udat_open(locale, QMetaType::QDate, m_style);
    m_timeFormatter = qt_udat_open(locale, QMetaType::QTime, m_style);
    m_dateTimeFormatter = qt_udat_open(locale, QMetaType::QDateTime, m_style);

    // Debug code
    //qt_printDateTimeSettings(m_dateTimeFormatter);
}

QDateTimeFormatterPrivate::~QDateTimeFormatterPrivate()
{
    if (m_dateFormatter)
        udat_close(m_dateFormatter);
    if (m_timeFormatter)
        udat_close(m_timeFormatter);
    if (m_dateTimeFormatter)
        udat_close(m_dateTimeFormatter);
}

bool QDateTimeFormatterPrivate::isValid(QMetaType::Type type) const
{
    switch (type) {
    case QMetaType::QDate :
        return (m_dateFormatter > 0);
    case QMetaType::QTime :
        return (m_timeFormatter > 0);
    default :
        return (m_dateTimeFormatter > 0);
    }
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
    return qt_udat_toPattern(m_dateFormatter);
}

QString QDateTimeFormatterPrivate::timePattern() const
{
    return qt_udat_toPattern(m_timeFormatter);
}

QString QDateTimeFormatterPrivate::dateTimePattern() const
{
    return qt_udat_toPattern(m_dateTimeFormatter);
}

void QDateTimeFormatterPrivate::setDatePattern(const QString &pattern)
{
    qt_udat_applyPattern(m_dateFormatter, pattern);
}

void QDateTimeFormatterPrivate::setTimePattern(const QString &pattern)
{
    qt_udat_applyPattern(m_timeFormatter, pattern);
}

void QDateTimeFormatterPrivate::setDateTimePattern(const QString &pattern)
{
    qt_udat_applyPattern(m_dateTimeFormatter, pattern);
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
    if (udat_isLenient(m_dateTimeFormatter))
        return QLocale::LenientParsing;
    return QLocale::StrictParsing;
}

void QDateTimeFormatterPrivate::setParsingMode(QLocale::ParsingMode mode)
{
    udat_setLenient(m_dateFormatter, (mode != QLocale::StrictParsing));
    udat_setLenient(m_timeFormatter, (mode != QLocale::StrictParsing));
    udat_setLenient(m_dateTimeFormatter, (mode != QLocale::StrictParsing));
}

QString QDateTimeFormatterPrivate::toString(const QDate &date) const
{
    return qt_udat_format(m_dateFormatter, qt_dateToUDate(date));
}

QString QDateTimeFormatterPrivate::toString(const QTime &time) const
{
    return qt_udat_format(m_timeFormatter, qt_timeToUDate(time));
}

QString QDateTimeFormatterPrivate::toString(const QDateTime &dateTime) const
{
    return qt_udat_format(m_dateTimeFormatter, qt_dateTimeToUDate(dateTime));
}

QString QDateTimeFormatterPrivate::toString(const QDate &date, const QString &pattern) const
{
    // Clone the formatter so we don't change the shared object
    UErrorCode status = U_ZERO_ERROR;
    UDateFormat *df = 0;
    df = udat_clone(m_dateTimeFormatter, &status);
    if (!U_SUCCESS(status))
        return QString();

    // Set the required pattern
    qt_udat_applyPattern(df, pattern);

    QString result = qt_udat_format(df, qt_dateToUDate(date));
    udat_close(df);
    return result;
}

QString QDateTimeFormatterPrivate::toString(const QTime &time, const QString &pattern) const
{
    // Clone the df so we don't change the shared object
    UErrorCode status = U_ZERO_ERROR;
    UDateFormat *df = 0;
    df = udat_clone(m_dateTimeFormatter, &status);
    if (!U_SUCCESS(status))
        return QString();

    // Set the required pattern
    qt_udat_applyPattern(df, pattern);

    QString result = qt_udat_format(df, qt_timeToUDate(time));
    udat_close(df);
    return result;
}

QString QDateTimeFormatterPrivate::toString(const QDateTime &dateTime, const QString &pattern) const
{
    // Clone the df so we don't change the shared object
    UErrorCode status = U_ZERO_ERROR;
    UDateFormat *df = 0;
    df = udat_clone(m_dateTimeFormatter, &status);
    if (!U_SUCCESS(status))
        return QString();

    // Set the required pattern
    status = U_ZERO_ERROR;
    qt_udat_applyPattern(df, pattern);

    QString result = qt_udat_format(df, qt_dateTimeToUDate(dateTime));
    udat_close(df);
    return result;
}

QDate QDateTimeFormatterPrivate::toDate(const QString &from, QLocale::ParsingMode mode) const
{
    return qt_udat_parseDate(m_dateFormatter, from, QString(), mode);
}

QTime QDateTimeFormatterPrivate::toTime(const QString &from, QLocale::ParsingMode mode) const
{
    return qt_udat_parseTime(m_timeFormatter, from, QString(), mode);
}

QDateTime QDateTimeFormatterPrivate::toDateTime(const QString &from, QLocale::ParsingMode mode) const
{
    return qt_udat_parseDateTime(m_dateTimeFormatter, from, QString(), mode);
}

QDate QDateTimeFormatterPrivate::toDate(const QString &from, const QString &pattern,
                                        QLocale::ParsingMode mode) const
{
    return qt_udat_parseDate(m_dateFormatter, from, pattern, mode);
}

QTime QDateTimeFormatterPrivate::toTime(const QString &from, const QString &pattern,
                                        QLocale::ParsingMode mode) const
{
    return qt_udat_parseTime(m_timeFormatter, from, pattern, mode);
}

QDateTime QDateTimeFormatterPrivate::toDateTime(const QString &from, const QString &pattern,
                                                QLocale::ParsingMode mode) const
{
    return qt_udat_parseDateTime(m_dateTimeFormatter, from, pattern, mode);
}

QString QDateTimeFormatterPrivate::eraName(int era, QDateTimeFormatter::ComponentStyle style,
                                           QDateTimeFormatter::ComponentContext context) const
{
    Q_UNUSED(context)
    UDateFormatSymbolType type;
    switch (style) {
    case QDateTimeFormatter::LongName :
        type = UDAT_ERA_NAMES;
            break;
    default :
        type = UDAT_ERAS;
    }
    return qt_udat_getSymbols(m_dateTimeFormatter, type, era - 1);
}

QString QDateTimeFormatterPrivate::quarterName(int quarter, QDateTimeFormatter::ComponentStyle style,
                                               QDateTimeFormatter::ComponentContext context) const
{
    UDateFormatSymbolType type;
    if (context == QDateTimeFormatter::StandaloneContext) {
        switch (style) {
        case QDateTimeFormatter::LongName :
            type = UDAT_STANDALONE_QUARTERS ;
            break;
        case QDateTimeFormatter::ShortName :
            type = UDAT_SHORT_QUARTERS;
            break;
        case QDateTimeFormatter::NarrowName :
            type = UDAT_SHORT_QUARTERS;
            break;
        default :
            type = UDAT_SHORT_QUARTERS;
        }
    } else {
        switch (style) {
        case QDateTimeFormatter::LongName :
            type = UDAT_QUARTERS;
            break;
        case QDateTimeFormatter::ShortName :
            type = UDAT_SHORT_QUARTERS;
            break;
        case QDateTimeFormatter::NarrowName :
            type = UDAT_SHORT_QUARTERS;
            break;
        default :
            type = UDAT_SHORT_QUARTERS;
        }
    }
    return qt_udat_getSymbols(m_dateTimeFormatter, type, quarter - 1);
}

QString QDateTimeFormatterPrivate::monthName(int month, QDateTimeFormatter::ComponentStyle style,
                                             QDateTimeFormatter::ComponentContext context) const
{
    UDateFormatSymbolType type;
    if (context == QDateTimeFormatter::StandaloneContext) {
        switch (style) {
        case QDateTimeFormatter::LongName :
            type =  UDAT_STANDALONE_MONTHS ;
            break;
        case QDateTimeFormatter::ShortName :
            type =  UDAT_STANDALONE_SHORT_MONTHS;
            break;
        case QDateTimeFormatter::NarrowName :
            type =  UDAT_STANDALONE_NARROW_MONTHS;
            break;
        default :
            type =  UDAT_STANDALONE_SHORT_MONTHS;
        }
    } else {
        switch (style) {
        case QDateTimeFormatter::LongName :
            type =  UDAT_MONTHS;
            break;
        case QDateTimeFormatter::ShortName :
            type =  UDAT_SHORT_MONTHS;
            break;
        case QDateTimeFormatter::NarrowName :
            type =  UDAT_NARROW_MONTHS;
            break;
        default :
            type =  UDAT_SHORT_MONTHS;
        }
    }
    return qt_udat_getSymbols(m_dateTimeFormatter, type, month - 1);
}

QString QDateTimeFormatterPrivate::weekdayName(int day, QDateTimeFormatter::ComponentStyle style,
                                               QDateTimeFormatter::ComponentContext context) const
{
    UDateFormatSymbolType type;
    if (context == QDateTimeFormatter::StandaloneContext) {
        switch (style) {
        case QDateTimeFormatter::LongName :
            type =  UDAT_STANDALONE_WEEKDAYS ;
            break;
        case QDateTimeFormatter::ShortName :
            type =  UDAT_STANDALONE_SHORT_WEEKDAYS;
            break;
        case QDateTimeFormatter::NarrowName :
            type =  UDAT_STANDALONE_NARROW_WEEKDAYS;
            break;
        default :
            type =  UDAT_STANDALONE_SHORT_WEEKDAYS;
        }
    } else {
        switch (style) {
        case QDateTimeFormatter::LongName :
            type =  UDAT_WEEKDAYS;
            break;
        case QDateTimeFormatter::ShortName :
            type =  UDAT_SHORT_WEEKDAYS;
            break;
        case QDateTimeFormatter::NarrowName :
            type =  UDAT_NARROW_WEEKDAYS;
            break;
        default :
            type =  UDAT_SHORT_WEEKDAYS;
        }
    }
    return qt_udat_getSymbols(m_dateTimeFormatter, type, day);
}

QString QDateTimeFormatterPrivate::dayPeriodName(const QTime &time, QDateTimeFormatter::ComponentStyle style,
                                                 QDateTimeFormatter::ComponentContext context) const
{
    Q_UNUSED(style)
    Q_UNUSED(context)
    int period = 0;
    if (time >= QTime(12, 0, 0))
        period = 1;
    return qt_udat_getSymbols(m_dateTimeFormatter, UDAT_AM_PMS, period);
}
