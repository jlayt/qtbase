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
    return ULOC_VALID_LOCALE;
}

UDateFormatStyle qt_styleToIcuStyle(QLocale::DateTimeStyle style)
{
    switch (style) {
    case QLocale::DefaultDateTimeStyle :
        return UDAT_DEFAULT;  // == UDAT_MEDIUM
    case QLocale::FullStyle :
        return UDAT_FULL;
    case QLocale::LongStyle :
        return UDAT_LONG;
    case QLocale::MediumStyle :
        return UDAT_MEDIUM;
    case QLocale::ShortStyle :
        return UDAT_SHORT;
    case QLocale::FullRelativeStyle :
        return UDAT_FULL_RELATIVE;
    case QLocale::LongRelativeStyle :
        return UDAT_LONG_RELATIVE;
    case QLocale::MediumRelativeStyle :
        return UDAT_MEDIUM_RELATIVE;
    case QLocale::ShortRelativeStyle :
        return UDAT_SHORT_RELATIVE;
    case QLocale::IsoDateStyle :
    case QLocale::IsoWeekStyle :
    case QLocale::IsoOrdinalStyle :
    case QLocale::Rfc2822Style :
    case QLocale::Rfc3339Style :
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

QString qt_customPattern(QMetaType::Type type, QLocale::DateTimeStyle style)
{
    if (type == QMetaType::QDate) {
        switch (style) {
        case QLocale::IsoDateStyle :
            return QStringLiteral("yyyy-MM-dd");
        case QLocale::IsoWeekStyle :
            return QStringLiteral("yyyy-'W'ww-E");
        case QLocale::IsoOrdinalStyle :
            return QStringLiteral("yyyy-DDD");
        case QLocale::Rfc2822Style :
            return QStringLiteral("EEE, dd MMM yyyy");
        case QLocale::Rfc3339Style :
            return QStringLiteral("yyyy-MM-dd");
        default :
            return QString();
        }
    } else if (type == QMetaType::QTime) {
        switch (style) {
        case QLocale::IsoDateStyle :
        case QLocale::IsoWeekStyle :
        case QLocale::IsoOrdinalStyle :
        case QLocale::Rfc2822Style :
        case QLocale::Rfc3339Style :
            return QStringLiteral("HH:mm:ss");
        default :
            return QString();
        }
    } else if (type == QMetaType::QDateTime) {
        switch (style) {
        case QLocale::IsoDateStyle :
            return QStringLiteral("yyyy-MM-dd'T'HH:mm:ss.SSSZZZZZ");
        case QLocale::IsoWeekStyle :
            return QStringLiteral("yyyy-'W'ww-E'T'HH:mm:ss.SSSZZZZZ");
        case QLocale::IsoOrdinalStyle :
            return QStringLiteral("yyyy-DDD'T'HH:mm:ss.SSSZZZZZ");
        case QLocale::Rfc2822Style :
            return QStringLiteral("EEE, dd MMM yyyy HH:mm:ss Z");
        case QLocale::Rfc3339Style :
            return QStringLiteral("yyyy-MM-dd'T'HH:mm:ss.SSSZZZZZZ");
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
        // udat_applyPattern(df, localized, value, valueSize, error, status)
        udat_applyPattern(df, FALSE, reinterpret_cast<const UChar *>(value.constData()),
                        value.size());
    }
}

UDateFormat *qt_udat_open(const QLocaleCode &locale, QMetaType::Type type, QLocale::DateTimeStyle style)
{
    QString icuLocale = locale.locale();
    UDateFormatStyle dateStyle = UDAT_NONE;
    UDateFormatStyle timeStyle = UDAT_NONE;

    if (type != QMetaType::QDate)
        timeStyle = qt_styleToIcuStyle(style);

    if (type != QMetaType::QTime)
        dateStyle = qt_styleToIcuStyle(style);

    // RFC formats require English symbols, don't use en_US as may have Americanised features
    if (style == QLocale::Rfc2822Style || style == QLocale::Rfc3339Style)
        icuLocale = QLatin1String("en_GB");

    UErrorCode status = U_ZERO_ERROR;
    UDateFormat *df = udat_open(timeStyle, dateStyle, icuLocale.toUtf8(), NULL, -1, NULL, -1, &status);

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
    return QLatin1String(udat_getLocaleByType(df, type , &status));
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

QDateTimeFormatter::ParsingMode qt_udat_parsingMode(UDateFormat *df)
{
    if (udat_isLenient(df))
        return QDateTimeFormatter::LenientParsing;
    return QDateTimeFormatter::StrictParsing;
}

UDateFormat *qt_parseFormatter(UDateFormat *mdf, const QString &pattern, QDateTimeFormatter::ParsingMode mode)
{
    // If same parsing mode and null pattern then use the default formatter
    if ((mode == QDateTimeFormatter::DefaultParsingMode || mode == qt_udat_parsingMode(mdf)) && pattern.isEmpty())
        return mdf;

    // If not same parsing mode or override pattern then clone the formatter
    UErrorCode status = U_ZERO_ERROR;
    UDateFormat *df = udat_clone(mdf, &status);
    if (!U_SUCCESS(status))
        return 0;
    if (mode != QDateTimeFormatter::DefaultParsingMode)
        udat_setLenient(df, (mode == QDateTimeFormatter::LenientParsing));
    qt_udat_applyPattern(df, pattern);
    return df;
}

QDate qt_udat_parseDate(UDateFormat *mdf, const QString &from,
                        const QString &pattern, QDateTimeFormatter::ParsingMode mode)
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
                        const QString &pattern, QDateTimeFormatter::ParsingMode mode)
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
                                const QString &pattern, QDateTimeFormatter::ParsingMode mode)
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
        out = out.append(qt_udat_getSymbols(df, type, i)).append(QStringLiteral(", "));
    qDebug() << text << out;
}

void qt_printDateTimeSettings(UDateFormat *df)
{
    qDebug() << "ULOC_REQUESTED_LOCALE           " << qt_udat_getLocaleByType(df, ULOC_REQUESTED_LOCALE);
    qDebug() << "ULOC_VALID_LOCALE               " << qt_udat_getLocaleByType(df, ULOC_VALID_LOCALE);
    qDebug() << "ULOC_ACTUAL_LOCALE              " << qt_udat_getLocaleByType(df, ULOC_ACTUAL_LOCALE);
    qDebug() << "Pattern                         " << qt_udat_toPattern(df);
    qDebug() << "Is Lenient                      " << udat_isLenient(df);
    qt_printSymbols(df, QStringLiteral("UDAT_ERAS                       "), UDAT_ERAS);
    qt_printSymbols(df, QStringLiteral("UDAT_ERA_NAMES                  "), UDAT_ERA_NAMES);
    qt_printSymbols(df, QStringLiteral("UDAT_MONTHS                     "), UDAT_MONTHS);
    qt_printSymbols(df, QStringLiteral("UDAT_SHORT_MONTHS               "), UDAT_SHORT_MONTHS);
    qt_printSymbols(df, QStringLiteral("UDAT_NARROW_MONTHS              "), UDAT_NARROW_MONTHS);
    qt_printSymbols(df, QStringLiteral("UDAT_STANDALONE_MONTHS          "), UDAT_STANDALONE_MONTHS);
    qt_printSymbols(df, QStringLiteral("UDAT_STANDALONE_SHORT_MONTHS    "), UDAT_STANDALONE_SHORT_MONTHS);
    qt_printSymbols(df, QStringLiteral("UDAT_STANDALONE_NARROW_MONTHS   "), UDAT_STANDALONE_NARROW_MONTHS);
    qt_printSymbols(df, QStringLiteral("UDAT_WEEKDAYS                   "), UDAT_WEEKDAYS);
    qt_printSymbols(df, QStringLiteral("UDAT_SHORT_WEEKDAYS             "), UDAT_SHORT_WEEKDAYS);
    qt_printSymbols(df, QStringLiteral("UDAT_NARROW_WEEKDAYS            "), UDAT_NARROW_WEEKDAYS);
    qt_printSymbols(df, QStringLiteral("UDAT_STANDALONE_WEEKDAYS        "), UDAT_STANDALONE_WEEKDAYS);
    qt_printSymbols(df, QStringLiteral("UDAT_STANDALONE_SHORT_WEEKDAYS  "), UDAT_STANDALONE_SHORT_WEEKDAYS);
    qt_printSymbols(df, QStringLiteral("UDAT_STANDALONE_NARROW_WEEKDAYS "), UDAT_STANDALONE_NARROW_WEEKDAYS);
    qt_printSymbols(df, QStringLiteral("UDAT_QUARTERS                   "), UDAT_QUARTERS);
    qt_printSymbols(df, QStringLiteral("UDAT_SHORT_QUARTERS             "), UDAT_SHORT_QUARTERS);
    qt_printSymbols(df, QStringLiteral("UDAT_STANDALONE_QUARTERS        "), UDAT_STANDALONE_QUARTERS);
    qt_printSymbols(df, QStringLiteral("UDAT_STANDALONE_SHORT_QUARTERS  "), UDAT_STANDALONE_SHORT_QUARTERS);
    qt_printSymbols(df, QStringLiteral("UDAT_AM_PMS                     "), UDAT_AM_PMS);
    qt_printSymbols(df, QStringLiteral("UDAT_LOCALIZED_CHARS            "), UDAT_LOCALIZED_CHARS);
}

QIcuDateTimeFormatterPrivate::QIcuDateTimeFormatterPrivate(QLocale::DateTimeStyle style, const QLocaleCode &locale)
    : QDateTimeFormatterPrivate(style, locale)
{
    // TODO Set date and time formatters to have no time zone, i.e.assume GMT???
    m_dateFormatter = qt_udat_open(locale, QMetaType::QDate, m_style);
    m_timeFormatter = qt_udat_open(locale, QMetaType::QTime, m_style);
    m_dateTimeFormatter = qt_udat_open(locale, QMetaType::QDateTime, m_style);

    // Debug code
    qt_printDateTimeSettings(m_dateTimeFormatter);
}

QIcuDateTimeFormatterPrivate::QIcuDateTimeFormatterPrivate(const QIcuDateTimeFormatterPrivate &other)
    : QDateTimeFormatterPrivate(other),
      m_dateFormatter(other.m_dateFormatter),
      m_timeFormatter(other.m_timeFormatter),
      m_dateTimeFormatter(other.m_dateTimeFormatter)
{
}

QIcuDateTimeFormatterPrivate::~QIcuDateTimeFormatterPrivate()
{
    if (m_dateFormatter)
        udat_close(m_dateFormatter);
    if (m_timeFormatter)
        udat_close(m_timeFormatter);
    if (m_dateTimeFormatter)
        udat_close(m_dateTimeFormatter);
}

QIcuDateTimeFormatterPrivate *QIcuDateTimeFormatterPrivate::clone()
{
    return new QIcuDateTimeFormatterPrivate(*this);
}

bool QIcuDateTimeFormatterPrivate::isValid(QMetaType::Type type) const
{
    switch (type) {
    case QMetaType::QDate :
        return (m_dateFormatter != Q_NULLPTR);
    case QMetaType::QTime :
        return (m_timeFormatter != Q_NULLPTR);
    default :
        return (m_dateTimeFormatter != Q_NULLPTR);
    }
}

/*
QString QIcuDateTimeFormatterPrivate::locale(QLocale::LocaleCodeType type) const
{
    return qt_udat_getLocaleByType(m_dateTimeFormatter, qt_localeTypeToIcuType2(type));
}
*/

QList<QLocale::DateTimeStyle> QIcuDateTimeFormatterPrivate::availableStyles()
{
    QList<QLocale::DateTimeStyle> list;
    list << QLocale::LongStyle << QLocale::MediumStyle;
    return list;
}

QString QIcuDateTimeFormatterPrivate::datePattern() const
{
    return qt_udat_toPattern(m_dateFormatter);
}

QString QIcuDateTimeFormatterPrivate::timePattern() const
{
    return qt_udat_toPattern(m_timeFormatter);
}

QString QIcuDateTimeFormatterPrivate::dateTimePattern() const
{
    return qt_udat_toPattern(m_dateTimeFormatter);
}

void QIcuDateTimeFormatterPrivate::setDatePattern(const QString &pattern)
{
    qt_udat_applyPattern(m_dateFormatter, pattern);
}

void QIcuDateTimeFormatterPrivate::setTimePattern(const QString &pattern)
{
    qt_udat_applyPattern(m_timeFormatter, pattern);
}

void QIcuDateTimeFormatterPrivate::setDateTimePattern(const QString &pattern)
{
    qt_udat_applyPattern(m_dateTimeFormatter, pattern);
}

qint32 QIcuDateTimeFormatterPrivate::shortYearWindowStart() const
{
    return 0; // TODO
}

void QIcuDateTimeFormatterPrivate::setShortYearWindowStart(qint32 year)
{
    // TODO
    Q_UNUSED(year)
}

QString QIcuDateTimeFormatterPrivate::toString(const QDate &date) const
{
    return qt_udat_format(m_dateFormatter, qt_dateToUDate(date));
}

QString QIcuDateTimeFormatterPrivate::toString(const QTime &time) const
{
    return qt_udat_format(m_timeFormatter, qt_timeToUDate(time));
}

QString QIcuDateTimeFormatterPrivate::toString(const QDateTime &dateTime) const
{
    return qt_udat_format(m_dateTimeFormatter, qt_dateTimeToUDate(dateTime));
}

QString QIcuDateTimeFormatterPrivate::toString(const QDate &date, const QString &pattern) const
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

QString QIcuDateTimeFormatterPrivate::toString(const QTime &time, const QString &pattern) const
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

QString QIcuDateTimeFormatterPrivate::toString(const QDateTime &dateTime, const QString &pattern) const
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

QDate QIcuDateTimeFormatterPrivate::toDate(const QString &from, QDateTimeFormatter::ParsingMode mode) const
{
    return qt_udat_parseDate(m_dateFormatter, from, QString(), mode);
}

QTime QIcuDateTimeFormatterPrivate::toTime(const QString &from, QDateTimeFormatter::ParsingMode mode) const
{
    return qt_udat_parseTime(m_timeFormatter, from, QString(), mode);
}

QDateTime QIcuDateTimeFormatterPrivate::toDateTime(const QString &from, QDateTimeFormatter::ParsingMode mode) const
{
    return qt_udat_parseDateTime(m_dateTimeFormatter, from, QString(), mode);
}

QDate QIcuDateTimeFormatterPrivate::toDate(const QString &from, const QString &pattern, QDateTimeFormatter::ParsingMode mode) const
{
    return qt_udat_parseDate(m_dateFormatter, from, pattern, mode);
}

QTime QIcuDateTimeFormatterPrivate::toTime(const QString &from, const QString &pattern, QDateTimeFormatter::ParsingMode mode) const
{
    return qt_udat_parseTime(m_timeFormatter, from, pattern, mode);
}

QDateTime QIcuDateTimeFormatterPrivate::toDateTime(const QString &from, const QString &pattern,
                                                   QDateTimeFormatter::ParsingMode mode) const
{
    return qt_udat_parseDateTime(m_dateTimeFormatter, from, pattern, mode);
}

QString QIcuDateTimeFormatterPrivate::eraName(int era, QLocale::ComponentStyle style,
                                              QLocale::ComponentContext context) const
{
    Q_UNUSED(context)
    UDateFormatSymbolType type;
    switch (style) {
    case QLocale::LongName :
        type = UDAT_ERA_NAMES;
            break;
    default :
        type = UDAT_ERAS;
    }
    return qt_udat_getSymbols(m_dateTimeFormatter, type, era - 1);
}

QString QIcuDateTimeFormatterPrivate::quarterName(int quarter, QLocale::ComponentStyle style,
                                                  QLocale::ComponentContext context) const
{
    UDateFormatSymbolType type;
    if (context == QLocale::StandaloneContext) {
        switch (style) {
        case QLocale::LongName :
            type = UDAT_STANDALONE_QUARTERS ;
            break;
        case QLocale::ShortName :
            type = UDAT_SHORT_QUARTERS;
            break;
        case QLocale::NarrowName :
            type = UDAT_SHORT_QUARTERS;
            break;
        default :
            type = UDAT_SHORT_QUARTERS;
        }
    } else {
        switch (style) {
        case QLocale::LongName :
            type = UDAT_QUARTERS;
            break;
        case QLocale::ShortName :
            type = UDAT_SHORT_QUARTERS;
            break;
        case QLocale::NarrowName :
            type = UDAT_SHORT_QUARTERS;
            break;
        default :
            type = UDAT_SHORT_QUARTERS;
        }
    }
    return qt_udat_getSymbols(m_dateTimeFormatter, type, quarter - 1);
}

QString QIcuDateTimeFormatterPrivate::monthName(int month, QLocale::ComponentStyle style,
                                                QLocale::ComponentContext context) const
{
    UDateFormatSymbolType type;
    if (context == QLocale::StandaloneContext) {
        switch (style) {
        case QLocale::LongName :
            type =  UDAT_STANDALONE_MONTHS ;
            break;
        case QLocale::ShortName :
            type =  UDAT_STANDALONE_SHORT_MONTHS;
            break;
        case QLocale::NarrowName :
            type =  UDAT_STANDALONE_NARROW_MONTHS;
            break;
        default :
            type =  UDAT_STANDALONE_SHORT_MONTHS;
        }
    } else {
        switch (style) {
        case QLocale::LongName :
            type =  UDAT_MONTHS;
            break;
        case QLocale::ShortName :
            type =  UDAT_SHORT_MONTHS;
            break;
        case QLocale::NarrowName :
            type =  UDAT_NARROW_MONTHS;
            break;
        default :
            type =  UDAT_SHORT_MONTHS;
        }
    }
    return qt_udat_getSymbols(m_dateTimeFormatter, type, month - 1);
}

QString QIcuDateTimeFormatterPrivate::weekdayName(int day, QLocale::ComponentStyle style,
                                                  QLocale::ComponentContext context) const
{
    UDateFormatSymbolType type;
    if (context == QLocale::StandaloneContext) {
        switch (style) {
        case QLocale::LongName :
            type =  UDAT_STANDALONE_WEEKDAYS ;
            break;
        case QLocale::ShortName :
            type =  UDAT_STANDALONE_SHORT_WEEKDAYS;
            break;
        case QLocale::NarrowName :
            type =  UDAT_STANDALONE_NARROW_WEEKDAYS;
            break;
        default :
            type =  UDAT_STANDALONE_SHORT_WEEKDAYS;
        }
    } else {
        switch (style) {
        case QLocale::LongName :
            type =  UDAT_WEEKDAYS;
            break;
        case QLocale::ShortName :
            type =  UDAT_SHORT_WEEKDAYS;
            break;
        case QLocale::NarrowName :
            type =  UDAT_NARROW_WEEKDAYS;
            break;
        default :
            type =  UDAT_SHORT_WEEKDAYS;
        }
    }
    return qt_udat_getSymbols(m_dateTimeFormatter, type, day);
}

QString QIcuDateTimeFormatterPrivate::dayPeriodName(const QTime &time, QLocale::ComponentStyle style,
                                                    QLocale::ComponentContext context) const
{
    Q_UNUSED(style)
    Q_UNUSED(context)
    int period = 0;
    if (time >= QTime(12, 0, 0))
        period = 1;
    return qt_udat_getSymbols(m_dateTimeFormatter, UDAT_AM_PMS, period);
}
