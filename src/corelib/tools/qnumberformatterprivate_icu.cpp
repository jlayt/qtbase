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

#include "qnumberformatter.h"
#include "qnumberformatterprivate_p.h"

#include <qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    Private
*/

// ICU Utilities

static int qt_icu_version()
{
    return QByteArray(U_ICU_VERSION_SHORT).toInt();
}

/*
static ULocDataLocaleType qt_uloc_localeCodeTypeToIcu(QLocale::LocaleCodeType type)
{
    switch (type) {
    case QLocale::RequestedLocaleCode :
        return ULOC_REQUESTED_LOCALE;
    case QLocale::EffectiveLocaleCode :
        return ULOC_VALID_LOCALE;
    }
    return ULOC_VALID_LOCALE;
}
*/

static UNumberFormatStyle qt_unum_numberStyleToIcu(QLocale::NumberStyle style)
{
    switch (style) {
    case QLocale::DecimalStyle :
        return UNUM_DECIMAL;
    case QLocale::PercentageStyle :
        return UNUM_PERCENT;
    case QLocale::ScientificStyle :
        return UNUM_SCIENTIFIC;
    case QLocale::OrdinalStyle :
        return UNUM_ORDINAL;
    case QLocale::DurationStyle :
        return UNUM_DURATION;
    case QLocale::SpelloutStyle :
        return UNUM_SPELLOUT;
    }
    return UNUM_DECIMAL;
}

static UNumberFormatStyle qt_unum_currencyStyleToIcu(QLocale::CurrencyStyle style)
{
    switch (style) {
    case QLocale::CurrencySymbolStyle :
        return UNUM_CURRENCY;
    case QLocale::CurrencyCodeStyle :
        if (qt_icu_version() < 48)
            return UNUM_CURRENCY;
        return UNUM_CURRENCY_ISO;
    case QLocale::CurrencyNameStyle :
        if (qt_icu_version() < 48)
            return UNUM_CURRENCY;
        return UNUM_CURRENCY_PLURAL;
    case QLocale::CurrencyAccountingStyle :
        if (qt_icu_version() < 53)
            return UNUM_CURRENCY;
        return UNUM_CURRENCY_ACCOUNTING;
    case QLocale::CurrencyCashStyle :
        if (qt_icu_version() < 54)
            return UNUM_CURRENCY;
        return UNUM_CASH_CURRENCY;
    }
    return UNUM_CURRENCY;
}

static UNumberFormatSymbol qt_unum_numberSymbolToIcuSymbol(QLocale::NumberSymbol symbol)
{
    switch (symbol) {
    case QLocale::DecimalSeparatorSymbol :
        return UNUM_DECIMAL_SEPARATOR_SYMBOL;
    case QLocale::GroupingSeparatorSymbol :
        return UNUM_GROUPING_SEPARATOR_SYMBOL;
    case QLocale::PercentSymbol :
        return UNUM_PERCENT_SYMBOL;
    case QLocale::MinusSignSymbol :
        return UNUM_MINUS_SIGN_SYMBOL;
    case QLocale::PlusSignSymbol :
        return UNUM_PLUS_SIGN_SYMBOL;
    case QLocale::ExponentialSymbol :
        return UNUM_EXPONENTIAL_SYMBOL;
    case QLocale::PerMillSymbol :
        return UNUM_PERMILL_SYMBOL;
    case QLocale::InfinitySymbol :
        return UNUM_INFINITY_SYMBOL;
    case QLocale::NaNSymbol :
        return UNUM_NAN_SYMBOL;
    case QLocale::ZeroDigitSymbol :
        return UNUM_ZERO_DIGIT_SYMBOL;
    case QLocale::PositivePrefix :
    case QLocale::PositiveSuffix :
    case QLocale::NegativePrefix :
    case QLocale::NegativeSuffix :
        // All are UNumberFormatTextAttribute
        return UNumberFormatSymbol(-1);
    }
    return UNumberFormatSymbol(-1);
}

static UNumberFormatTextAttribute qt_unum_numberSymbolToIcuTextAttribute(QLocale::NumberSymbol symbol)
{
    switch (symbol) {
    case QLocale::DecimalSeparatorSymbol :
    case QLocale::GroupingSeparatorSymbol :
    case QLocale::PercentSymbol :
    case QLocale::MinusSignSymbol :
    case QLocale::PlusSignSymbol :
    case QLocale::ExponentialSymbol :
    case QLocale::PerMillSymbol :
    case QLocale::InfinitySymbol :
    case QLocale::NaNSymbol :
    case QLocale::ZeroDigitSymbol :
        // All are UNumberFormatSymbol
        return UNumberFormatTextAttribute(-1);
    case QLocale::PositivePrefix :
        return  UNUM_POSITIVE_PREFIX;
    case QLocale::PositiveSuffix :
        return  UNUM_POSITIVE_SUFFIX;
    case QLocale::NegativePrefix :
        return  UNUM_NEGATIVE_PREFIX;
    case QLocale::NegativeSuffix :
        return  UNUM_NEGATIVE_SUFFIX;
    }
    return UNumberFormatTextAttribute(-1);
}

static UNumberFormatSymbol qt_unum_currencySymbolTypeToIcu(QLocale::CurrencySymbolType symbol)
{
    switch (symbol) {
    case QLocale::CurrencyStandardSymbol :
        return UNUM_CURRENCY_SYMBOL;
    case QLocale::CurrencyInternationalSymbol :
        return UNUM_INTL_CURRENCY_SYMBOL;
    }
    return UNUM_CURRENCY_SYMBOL;
}

static UNumberFormatAttribute qt_unum_numberAttributeToIcu(QLocale::NumberAttribute attribute)
{
    switch (attribute) {
    case QLocale::MaximumIntegerDigits :
        return UNUM_MAX_INTEGER_DIGITS;
    case QLocale::MinimumIntegerDigits :
        return UNUM_MIN_INTEGER_DIGITS;
    case QLocale::MaximumFractionDigits :
        return UNUM_MAX_FRACTION_DIGITS;
    case QLocale::MinimumFractionDigits :
        return UNUM_MIN_FRACTION_DIGITS;
    case QLocale::MaximumSignificantDigits :
        return UNUM_MAX_SIGNIFICANT_DIGITS;
    case QLocale::MinimumSignificantDigits :
        return UNUM_MIN_SIGNIFICANT_DIGITS;
    case QLocale::Multiplier :
        return UNUM_MULTIPLIER;
    case QLocale::PrimaryGroupingSize :
        return UNUM_GROUPING_SIZE;
    case QLocale::SecondaryGroupingSize :
        return UNUM_SECONDARY_GROUPING_SIZE;
    }
    return UNumberFormatAttribute(-1);
}

static UNumberFormatAttribute qt_unum_numberFlagToIcu(QLocale::NumberFlag flag)
{
    switch (flag) {
    case QLocale::DefaultNumberFlags :
        return UNumberFormatAttribute(-1);
    case QLocale::ParseLenient :
        return UNUM_LENIENT_PARSE;
    case QLocale::ParseToDecimalSeparator :
        return UNUM_PARSE_INT_ONLY;
    case QLocale::FormatIntegerWithDecimalSeparator :
        return UNUM_DECIMAL_ALWAYS_SHOWN;
    case QLocale::FormatWithoutGroupingSeparator :
        return UNUM_GROUPING_USED;
    }
    return UNumberFormatAttribute(-1);
}

static UNumberFormatPadPosition qt_unum_padPositionToIcu(QLocale::PadPosition pos)
{
    switch (pos) {
    case QLocale::PadBeforePrefix :
        return UNUM_PAD_BEFORE_PREFIX;
    case QLocale::PadAfterPrefix :
        return UNUM_PAD_AFTER_PREFIX;
    case QLocale::PadBeforeSuffix :
        return UNUM_PAD_BEFORE_SUFFIX;
    case QLocale::PadAfterSuffix :
        return UNUM_PAD_AFTER_SUFFIX;
    case QLocale::DefaultPadPosition :
        // Should never be passed in
        return UNumberFormatPadPosition(-1);
    }
    return UNumberFormatPadPosition(-1);
}

static QLocale::PadPosition qt_unum_padPositionToQt(UNumberFormatPadPosition pos)
{
    switch (pos) {
    case UNUM_PAD_BEFORE_PREFIX :
        return QLocale::PadBeforePrefix;
    case UNUM_PAD_AFTER_PREFIX :
        return QLocale::PadAfterPrefix;
    case UNUM_PAD_BEFORE_SUFFIX :
        return QLocale::PadBeforeSuffix;
    case UNUM_PAD_AFTER_SUFFIX :
        return QLocale::PadAfterSuffix;
    }
    return QLocale::PadBeforePrefix;
}

static UNumberFormatRoundingMode qt_unum_roundingModeToIcu(QLocale::RoundingMode mode)
{
    switch (mode) {
    case QLocale::RoundCeiling :
        return UNUM_ROUND_CEILING;
    case QLocale::RoundFloor :
        return UNUM_ROUND_FLOOR;
    case QLocale::RoundDown :
        return UNUM_ROUND_DOWN;
    case QLocale::RoundUp :
        return UNUM_ROUND_UP;
    case QLocale::RoundHalfEven :
        return UNUM_ROUND_HALFEVEN;
    case QLocale::RoundHalfDown :
        return UNUM_ROUND_HALFDOWN;
    case QLocale::RoundHalfUp :
        return UNUM_ROUND_HALFUP;
    case QLocale::RoundUnnecessary :
        return UNUM_ROUND_UNNECESSARY;
    case QLocale::DefaultRoundingMode :
        // Should never be passed in
        return UNumberFormatRoundingMode(-1);
    }
    return UNumberFormatRoundingMode(-1);
}

static QLocale::RoundingMode qt_unum_roundingModeToQt(UNumberFormatRoundingMode mode)
{
    switch (mode) {
    case UNUM_ROUND_CEILING :
        return QLocale::RoundCeiling;
    case UNUM_ROUND_FLOOR :
        return QLocale::RoundFloor;
    case UNUM_ROUND_DOWN :
        return QLocale::RoundDown;
    case UNUM_ROUND_UP :
        return QLocale::RoundUp;
    case UNUM_ROUND_HALFEVEN :
        return QLocale::RoundHalfEven;
    case UNUM_ROUND_HALFDOWN :
        return QLocale::RoundHalfDown;
    case UNUM_ROUND_HALFUP :
        return QLocale::RoundHalfUp;
    case UNUM_ROUND_UNNECESSARY :
        return QLocale::RoundUnnecessary;
    }
    return QLocale::RoundHalfDown;
}

static QString qt_unum_getLocaleByType(UNumberFormat *nf, ULocDataLocaleType type)
{
    UErrorCode status = U_ZERO_ERROR;
    // unum_getLocaleByType(nf, type, error)
    return QString::fromLatin1(unum_getLocaleByType(nf, type , &status));
}

static QString qt_unum_getTextAttribute(UNumberFormat *nf, UNumberFormatTextAttribute attribute)
{
    int32_t size = 10;
    QString result(size, Qt::Uninitialized);
    UErrorCode status = U_ZERO_ERROR;

    // size = unum_getTextAttribute(nf, attribute, result, resultLength, status)
    size = unum_getTextAttribute(nf, attribute, reinterpret_cast<UChar *>(result.data()), size, &status);

    // If overflow, then resize and retry
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        result.resize(size);
        status = U_ZERO_ERROR;
        size = unum_getTextAttribute(nf, attribute, reinterpret_cast<UChar *>(result.data()), size, &status);
    }

    // If successful on first or second go, resize and return
    if (U_SUCCESS(status)) {
        result.resize(size);
        return result;
    }

    return QString();
}

static void qt_unum_setTextAttribute(UNumberFormat *nf, UNumberFormatTextAttribute attribute, const QString &value)
{
    UErrorCode status = U_ZERO_ERROR;
    // unum_setAttribute(nf, attribute, value, valueSize, status)
    unum_setTextAttribute(nf, attribute, reinterpret_cast<const UChar *>(value.constData()), value.size(), &status);
}

static QString qt_unum_getSymbol(UNumberFormat *nf, UNumberFormatSymbol symbol)
{
    int32_t size = 10;
    QString result(size, Qt::Uninitialized);
    UErrorCode status = U_ZERO_ERROR;

    // size = unum_getSymbol(nf, symbol, result, resultLength, status)
    size = unum_getSymbol(nf, symbol, reinterpret_cast<UChar *>(result.data()), size, &status);

    // If overflow, then resize and retry
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        result.resize(size);
        status = U_ZERO_ERROR;
        size = unum_getSymbol(nf, symbol, reinterpret_cast<UChar *>(result.data()), size, &status);
    }

    // If successful on first or second go, resize and return
    if (U_SUCCESS(status)) {
        result.resize(size);
        return result;
    }

    return QString();
}

static void qt_unum_setSymbol(UNumberFormat *nf, UNumberFormatSymbol symbol, const QString &value)
{
    UErrorCode status = U_ZERO_ERROR;
    // unum_setAttribute(nf, symbol, value, valueSize, status)
    unum_setSymbol(nf, symbol, reinterpret_cast<const UChar *>(value.data()), value.size(), &status);
}

static QString qt_unum_pattern(UNumberFormat *nf)
{
    int32_t size = 50;
    QString result(size, Qt::Uninitialized);
    UErrorCode status = U_ZERO_ERROR;

    // unum_toPattern (nf, localized, reinterpret_cast<UChar *>(result.data()), size, &status)
    unum_toPattern(nf, false, reinterpret_cast<UChar *>(result.data()), size, &status);

    // If overflow, then resize and retry
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        result.resize(size);
        status = U_ZERO_ERROR;
        unum_toPattern(nf, false, reinterpret_cast<UChar *>(result.data()), size, &status);
    }

    // If successful on first or second go, resize and return
    if (U_SUCCESS(status)) {
        result.resize(size);
        return result;
    }

    return QString();
}

static QString qt_unum_formatDouble(UNumberFormat *nf, double from)
{
    int32_t size = 100;
    QString result(size, Qt::Uninitialized);
    UErrorCode status = U_ZERO_ERROR;

    // size = unum_formatDouble(nf, from, result, resultLength, pos, status)
    size = unum_formatDouble(nf, from, reinterpret_cast<UChar *>(result.data()), size, NULL, &status);

    // If overflow, then resize and retry
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        result.resize(size);
        status = U_ZERO_ERROR;
        size = unum_formatDouble(nf, from, reinterpret_cast<UChar *>(result.data()), size, NULL, &status);
    }

    // If successful on first or second go, resize and return
    if (U_SUCCESS(status)) {
        result.resize(size);
        return result;
    }

    return QString();
}

static double qt_unum_parseDouble(UNumberFormat *nf, const QString &from, bool *ok, qint32 *pos)
{
    double res = 0;
    bool success = false;
    int upos = 0;

    if (nf) {
        UErrorCode status = U_ZERO_ERROR;
        // unum_parseDouble(nf, text, textLength, pos, status)
        res = unum_parseDouble(nf, reinterpret_cast<const UChar *>(from.constData()), from.size(), &upos, &status);
        success = U_SUCCESS(status);
    }

    if (ok)
        *ok = success;
    if (pos)
        *pos = upos;

    return res;
}

// Temporary debug code
static void qt_printSettings(UNumberFormat *nf)
{
    qDebug() << "ULOC_REQUESTED_LOCALE                   " << qt_unum_getLocaleByType(nf, ULOC_REQUESTED_LOCALE);
    qDebug() << "ULOC_VALID_LOCALE                       " << qt_unum_getLocaleByType(nf, ULOC_VALID_LOCALE);
    qDebug() << "ULOC_ACTUAL_LOCALE                      " << qt_unum_getLocaleByType(nf, ULOC_ACTUAL_LOCALE);
    qDebug() << "UNUM_PARSE_INT_ONLY                     " << unum_getAttribute(nf, UNUM_PARSE_INT_ONLY);
    qDebug() << "UNUM_GROUPING_USED                      " << unum_getAttribute(nf, UNUM_GROUPING_USED);
    qDebug() << "UNUM_DECIMAL_ALWAYS_SHOWN               " << unum_getAttribute(nf, UNUM_DECIMAL_ALWAYS_SHOWN);
    qDebug() << "UNUM_MAX_INTEGER_DIGITS                 " << unum_getAttribute(nf, UNUM_MAX_INTEGER_DIGITS);
    qDebug() << "UNUM_MIN_INTEGER_DIGITS                 " << unum_getAttribute(nf, UNUM_MIN_INTEGER_DIGITS);
    qDebug() << "UNUM_INTEGER_DIGITS                     " << unum_getAttribute(nf, UNUM_INTEGER_DIGITS);
    qDebug() << "UNUM_MAX_FRACTION_DIGITS                " << unum_getAttribute(nf, UNUM_MAX_FRACTION_DIGITS);
    qDebug() << "UNUM_MIN_FRACTION_DIGITS                " << unum_getAttribute(nf, UNUM_MIN_FRACTION_DIGITS);
    qDebug() << "UNUM_FRACTION_DIGITS                    " << unum_getAttribute(nf, UNUM_FRACTION_DIGITS);
    qDebug() << "UNUM_MULTIPLIER                         " << unum_getAttribute(nf, UNUM_MULTIPLIER);
    qDebug() << "UNUM_GROUPING_SIZE                      " << unum_getAttribute(nf, UNUM_GROUPING_SIZE);
    qDebug() << "UNUM_ROUNDING_MODE                      " << unum_getAttribute(nf, UNUM_ROUNDING_MODE);
    qDebug() << "UNUM_ROUNDING_INCREMENT                 " << unum_getDoubleAttribute(nf, UNUM_ROUNDING_INCREMENT);
    qDebug() << "UNUM_FORMAT_WIDTH                       " << unum_getAttribute(nf, UNUM_FORMAT_WIDTH);
    qDebug() << "UNUM_PADDING_POSITION                   " << unum_getAttribute(nf, UNUM_PADDING_POSITION);
    qDebug() << "UNUM_SECONDARY_GROUPING_SIZE            " << unum_getAttribute(nf, UNUM_SECONDARY_GROUPING_SIZE);
    qDebug() << "UNUM_SIGNIFICANT_DIGITS_USED            " << unum_getAttribute(nf, UNUM_SIGNIFICANT_DIGITS_USED);
    qDebug() << "UNUM_MIN_SIGNIFICANT_DIGITS             " << unum_getAttribute(nf, UNUM_MIN_SIGNIFICANT_DIGITS);
    qDebug() << "UNUM_MAX_SIGNIFICANT_DIGITS             " << unum_getAttribute(nf, UNUM_MAX_SIGNIFICANT_DIGITS);
    qDebug() << "UNUM_LENIENT_PARSE                      " << unum_getAttribute(nf, UNUM_LENIENT_PARSE);
    qDebug() << "UNUM_PADDING_CHARACTER                  " << qt_unum_getTextAttribute(nf, UNUM_PADDING_CHARACTER);
    qDebug() << "UNUM_POSITIVE_PREFIX                    " << qt_unum_getTextAttribute(nf, UNUM_POSITIVE_PREFIX);
    qDebug() << "UNUM_POSITIVE_SUFFIX                    " << qt_unum_getTextAttribute(nf, UNUM_POSITIVE_SUFFIX);
    qDebug() << "UNUM_NEGATIVE_PREFIX                    " << qt_unum_getTextAttribute(nf, UNUM_NEGATIVE_PREFIX);
    qDebug() << "UNUM_NEGATIVE_SUFFIX                    " << qt_unum_getTextAttribute(nf, UNUM_NEGATIVE_SUFFIX);
    qDebug() << "UNUM_CURRENCY_CODE                      " << qt_unum_getTextAttribute(nf, UNUM_CURRENCY_CODE);
    qDebug() << "UNUM_DEFAULT_RULESET                    " << qt_unum_getTextAttribute(nf, UNUM_DEFAULT_RULESET);
    qDebug() << "UNUM_PUBLIC_RULESETS                    " << qt_unum_getTextAttribute(nf, UNUM_PUBLIC_RULESETS);
    qDebug() << "UNUM_DECIMAL_SEPARATOR_SYMBOL           " << qt_unum_getSymbol(nf, UNUM_DECIMAL_SEPARATOR_SYMBOL);
    qDebug() << "UNUM_GROUPING_SEPARATOR_SYMBOL          " << qt_unum_getSymbol(nf, UNUM_GROUPING_SEPARATOR_SYMBOL);
    qDebug() << "UNUM_PATTERN_SEPARATOR_SYMBOL           " << qt_unum_getSymbol(nf, UNUM_PATTERN_SEPARATOR_SYMBOL);
    qDebug() << "UNUM_PERCENT_SYMBOL                     " << qt_unum_getSymbol(nf, UNUM_PERCENT_SYMBOL);
    qDebug() << "UNUM_DIGIT_SYMBOL                       " << qt_unum_getSymbol(nf, UNUM_DIGIT_SYMBOL);
    qDebug() << "UNUM_MINUS_SIGN_SYMBOL                  " << qt_unum_getSymbol(nf, UNUM_MINUS_SIGN_SYMBOL);
    qDebug() << "UNUM_PLUS_SIGN_SYMBOL                   " << qt_unum_getSymbol(nf, UNUM_PLUS_SIGN_SYMBOL);
    qDebug() << "UNUM_CURRENCY_SYMBOL                    " << qt_unum_getSymbol(nf, UNUM_CURRENCY_SYMBOL);
    qDebug() << "UNUM_INTL_CURRENCY_SYMBOL               " << qt_unum_getSymbol(nf, UNUM_INTL_CURRENCY_SYMBOL);
    qDebug() << "UNUM_MONETARY_SEPARATOR_SYMBOL          " << qt_unum_getSymbol(nf, UNUM_MONETARY_SEPARATOR_SYMBOL);
    qDebug() << "UNUM_EXPONENTIAL_SYMBOL                 " << qt_unum_getSymbol(nf, UNUM_EXPONENTIAL_SYMBOL);
    qDebug() << "UNUM_PERMILL_SYMBOL                     " << qt_unum_getSymbol(nf, UNUM_PERMILL_SYMBOL);
    qDebug() << "UNUM_PAD_ESCAPE_SYMBOL                  " << qt_unum_getSymbol(nf, UNUM_PAD_ESCAPE_SYMBOL);
    qDebug() << "UNUM_INFINITY_SYMBOL                    " << qt_unum_getSymbol(nf, UNUM_INFINITY_SYMBOL);
    qDebug() << "UNUM_NAN_SYMBOL                         " << qt_unum_getSymbol(nf, UNUM_NAN_SYMBOL);
    qDebug() << "UNUM_SIGNIFICANT_DIGIT_SYMBOL           " << qt_unum_getSymbol(nf, UNUM_SIGNIFICANT_DIGIT_SYMBOL);
    qDebug() << "UNUM_MONETARY_GROUPING_SEPARATOR_SYMBOL " << qt_unum_getSymbol(nf, UNUM_MONETARY_GROUPING_SEPARATOR_SYMBOL);
    qDebug() << "UNUM_ZERO_DIGIT_SYMBOL                  " << qt_unum_getSymbol(nf, UNUM_ZERO_DIGIT_SYMBOL);
    qDebug() << "UNUM_ONE_DIGIT_SYMBOL                   " << qt_unum_getSymbol(nf, UNUM_ONE_DIGIT_SYMBOL);
    qDebug() << "UNUM_TWO_DIGIT_SYMBOL                   " << qt_unum_getSymbol(nf, UNUM_TWO_DIGIT_SYMBOL);
    qDebug() << "UNUM_THREE_DIGIT_SYMBOL                 " << qt_unum_getSymbol(nf, UNUM_THREE_DIGIT_SYMBOL);
    qDebug() << "UNUM_FIVE_DIGIT_SYMBOL                  " << qt_unum_getSymbol(nf, UNUM_FIVE_DIGIT_SYMBOL);
    qDebug() << "UNUM_SIX_DIGIT_SYMBOL                   " << qt_unum_getSymbol(nf, UNUM_SIX_DIGIT_SYMBOL);
    qDebug() << "UNUM_SEVEN_DIGIT_SYMBOL                 " << qt_unum_getSymbol(nf, UNUM_SEVEN_DIGIT_SYMBOL);
    qDebug() << "UNUM_EIGHT_DIGIT_SYMBOL                 " << qt_unum_getSymbol(nf, UNUM_EIGHT_DIGIT_SYMBOL);
    qDebug() << "UNUM_NINE_DIGIT_SYMBOL                  " << qt_unum_getSymbol(nf, UNUM_NINE_DIGIT_SYMBOL);
    qDebug() << "UNUM_FORMAT_SYMBOL_COUNT                " << qt_unum_getSymbol(nf, UNUM_FORMAT_SYMBOL_COUNT);
}

QIcuNumberFormatterPrivate::QIcuNumberFormatterPrivate(QLocale::NumberStyle style, const QLocaleCode &locale)
    : QNumberFormatterPrivate(style, locale),
      m_status(U_ZERO_ERROR)
{
    //unum_open(style, pattern, patternLength, locale, parseErr, status)
    m_unum = unum_open(qt_unum_numberStyleToIcu(style), NULL, -1, locale.locale().toUtf8(), NULL, &m_status);

    // Match QLocale default behaviour
    // TODO move setting to QLocale or default only?
    if (isValid())
        unum_setAttribute(m_unum, UNUM_MAX_FRACTION_DIGITS, 6);

    // Debug code
    if (isValid())
        qt_printSettings(m_unum);
}

QIcuNumberFormatterPrivate::QIcuNumberFormatterPrivate(QLocale::CurrencyStyle style, const QLocaleCode &locale)
    : QNumberFormatterPrivate(style, locale),
      m_status(U_ZERO_ERROR)
{
    //unum_open(style, pattern, patternLength, locale, parseErr, status)
    m_unum = unum_open(qt_unum_currencyStyleToIcu(style), NULL, -1, locale.locale().toUtf8(), NULL, &m_status);

    // Debug code
    if (isValid())
        qt_printSettings(m_unum);
}

QIcuNumberFormatterPrivate::QIcuNumberFormatterPrivate(const QIcuNumberFormatterPrivate &other)
    : QNumberFormatterPrivate(other),
      m_unum(other.m_unum),
      m_status(other.m_status)
{
}

QIcuNumberFormatterPrivate::~QIcuNumberFormatterPrivate()
{
    unum_close(m_unum);
}

QIcuNumberFormatterPrivate *QIcuNumberFormatterPrivate::clone()
{
    return new QIcuNumberFormatterPrivate(*this);
}

bool QIcuNumberFormatterPrivate::isValid() const
{
    return (U_SUCCESS(m_status));
}

QList<QLocale::NumberStyle> QIcuNumberFormatterPrivate::availableNumberStyles()
{
    QList<QLocale::NumberStyle> list;

    list << QLocale::DecimalStyle
         << QLocale::ScientificStyle
         << QLocale::PercentageStyle
         << QLocale::SpelloutStyle
         << QLocale::OrdinalStyle
         << QLocale::DurationStyle;

    return list;
}

QList<QLocale::CurrencyStyle> QIcuNumberFormatterPrivate::availableCurrencyStyles()
{
    QList<QLocale::CurrencyStyle> list;

    list << QLocale::CurrencySymbolStyle;
    if (qt_icu_version() >= 48)
        list << QLocale::CurrencyCodeStyle << QLocale::CurrencyNameStyle;
    if (qt_icu_version() >= 53)
        list << QLocale::CurrencyAccountingStyle;
    if (qt_icu_version() >= 54)
        list << QLocale::CurrencyCashStyle;

    return list;
}

QString QIcuNumberFormatterPrivate::positivePattern() const
{
    return qt_unum_pattern(m_unum).split(QLatin1Char(';')).at(0);
}

QString QIcuNumberFormatterPrivate::negativePattern() const
{
    return qt_unum_pattern(m_unum).split(QLatin1Char(';')).at(1);
}

void QIcuNumberFormatterPrivate::setPattern(const QString & positive, const QString & negative)
{
    QString pattern = positive + QStringLiteral(";") + negative;
    UErrorCode status = U_ZERO_ERROR;
    // unum_applyPattern(nf, localized, value, valueSize, error, status)
    unum_applyPattern(m_unum, FALSE, reinterpret_cast<const UChar *>(pattern.constData()), pattern.size(), NULL, &status);
}

QString QIcuNumberFormatterPrivate::currencyCode() const
{
    return qt_unum_getTextAttribute(m_unum, UNUM_CURRENCY_CODE);
}

void QIcuNumberFormatterPrivate::setCurrencyCode(const QString &value)
{
    qt_unum_setTextAttribute(m_unum, UNUM_CURRENCY_CODE, value);
}

QString QIcuNumberFormatterPrivate::currencySymbol(QLocale::CurrencySymbolType symbol) const
{
    return qt_unum_getSymbol(m_unum, qt_unum_currencySymbolTypeToIcu(symbol));
}

void QIcuNumberFormatterPrivate::setCurrencySymbol(QLocale::CurrencySymbolType symbol, const QString &value)
{
    qt_unum_setSymbol(m_unum, qt_unum_currencySymbolTypeToIcu(symbol), value);
}

QString QIcuNumberFormatterPrivate::symbol(QLocale::NumberSymbol symbol) const
{
    switch (symbol) {
    case QLocale::PositivePrefix :
    case QLocale::PositiveSuffix :
    case QLocale::NegativePrefix :
    case QLocale::NegativeSuffix :
        return qt_unum_getTextAttribute(m_unum, qt_unum_numberSymbolToIcuTextAttribute(symbol));
    default :
        return qt_unum_getSymbol(m_unum, qt_unum_numberSymbolToIcuSymbol(symbol));
    }
}

void QIcuNumberFormatterPrivate::setSymbol(QLocale::NumberSymbol symbol, const QString &value)
{
    switch (symbol) {
    case QLocale::PositivePrefix :
    case QLocale::PositiveSuffix :
    case QLocale::NegativePrefix :
    case QLocale::NegativeSuffix :
        qt_unum_setTextAttribute(m_unum, qt_unum_numberSymbolToIcuTextAttribute(symbol), value);
    default :
        qt_unum_setSymbol(m_unum, qt_unum_numberSymbolToIcuSymbol(symbol), value);
    }
}

qint32 QIcuNumberFormatterPrivate::attribute(QLocale::NumberAttribute attribute) const
{
    return unum_getAttribute(m_unum, qt_unum_numberAttributeToIcu(attribute));
}

void QIcuNumberFormatterPrivate::setAttribute(QLocale::NumberAttribute attribute, qint32 value)
{
    unum_setAttribute(m_unum, qt_unum_numberAttributeToIcu(attribute), value);
}

bool QIcuNumberFormatterPrivate::flag(QLocale::NumberFlag flag) const
{
    if (flag == QLocale::DefaultNumberFlags)
        return (flags() == QLocale::DefaultNumberFlags);
    return unum_getAttribute(m_unum, qt_unum_numberFlagToIcu(flag));
}

void QIcuNumberFormatterPrivate::setFlag(QLocale::NumberFlag flag, bool value)
{
    if (flag == QLocale::DefaultNumberFlags)
        setFlags(flag);
    else
        unum_setAttribute(m_unum, qt_unum_numberFlagToIcu(flag), value);
}

QLocale::NumberFlags QIcuNumberFormatterPrivate::flags() const
{
    QLocale::NumberFlags fl = QLocale::DefaultNumberFlags;
    if (flag(QLocale::ParseLenient))
        fl = fl | QLocale::ParseLenient;
    if (flag(QLocale::ParseToDecimalSeparator))
        fl = fl | QLocale::ParseToDecimalSeparator;
    if (flag(QLocale::FormatIntegerWithDecimalSeparator))
        fl = fl | QLocale::FormatIntegerWithDecimalSeparator;
    if (flag(QLocale::FormatWithoutGroupingSeparator))
        fl = fl | QLocale::FormatWithoutGroupingSeparator;
    return fl;
}

void QIcuNumberFormatterPrivate::setFlags(QLocale::NumberFlags flags)
{
    setFlag(QLocale::ParseLenient, (flags & QLocale::ParseLenient));
    setFlag(QLocale::ParseToDecimalSeparator, (flags & QLocale::ParseToDecimalSeparator));
    setFlag(QLocale::FormatIntegerWithDecimalSeparator, (flags & QLocale::FormatIntegerWithDecimalSeparator));
    setFlag(QLocale::FormatWithoutGroupingSeparator, (flags & QLocale::FormatWithoutGroupingSeparator));
}

QLocale::DecimalFormatMode QIcuNumberFormatterPrivate::decimalFormatMode() const
{
    if (unum_getAttribute(m_unum, UNUM_SIGNIFICANT_DIGITS_USED))
        return QLocale::SignificantDigitsMode;
    else
        return QLocale::IntegerFractionDigitsMode;
}

void QIcuNumberFormatterPrivate::setDecimalFormatMode(QLocale::DecimalFormatMode mode)
{
    unum_setAttribute(m_unum, UNUM_SIGNIFICANT_DIGITS_USED, (mode == QLocale::SignificantDigitsMode));
}

QLocale::RoundingMode QIcuNumberFormatterPrivate::roundingMode() const
{
    return qt_unum_roundingModeToQt((UNumberFormatRoundingMode)unum_getAttribute(m_unum, UNUM_ROUNDING_MODE));
}

double QIcuNumberFormatterPrivate::roundingIncrement() const
{
    return unum_getDoubleAttribute(m_unum, UNUM_ROUNDING_INCREMENT);
}

void QIcuNumberFormatterPrivate::setRounding(QLocale::RoundingMode mode, double increment)
{
    if (mode != QLocale::DefaultRoundingMode)
        unum_setAttribute(m_unum, UNUM_ROUNDING_MODE , qt_unum_roundingModeToIcu(mode));
    if (increment >= 0.0)
        unum_setDoubleAttribute(m_unum, UNUM_ROUNDING_INCREMENT, increment);
}

qint32 QIcuNumberFormatterPrivate::paddingWidth() const
{
    return unum_getAttribute(m_unum, UNUM_FORMAT_WIDTH);
}

QString QIcuNumberFormatterPrivate::padding() const
{
    return qt_unum_getTextAttribute(m_unum, UNUM_PADDING_CHARACTER);
}

QLocale::PadPosition QIcuNumberFormatterPrivate::paddingPosition() const
{
    return qt_unum_padPositionToQt((UNumberFormatPadPosition)unum_getAttribute(m_unum, UNUM_PADDING_POSITION));
}

void QIcuNumberFormatterPrivate::setPadding(qint32 width, const QString &padding, QLocale::PadPosition position)
{
    // Set padding if not default values
    if (width >= 0)
        unum_setAttribute(m_unum,  UNUM_FORMAT_WIDTH, width);
    if (!padding.isNull())
        qt_unum_setTextAttribute(m_unum, UNUM_PADDING_CHARACTER, padding);
    if (position != QLocale::DefaultPadPosition)
        unum_setAttribute(m_unum,  UNUM_PADDING_POSITION, qt_unum_padPositionToIcu(position));
}

QString QIcuNumberFormatterPrivate::toString(qint64 from) const
{
    int32_t size = 60;
    QString result(size, Qt::Uninitialized);
    UErrorCode status = U_ZERO_ERROR;

    // size = unum_format(nf, from, result, resultLength, pos, status)
    size = unum_formatInt64(m_unum, from, reinterpret_cast<UChar *>(result.data()), size, NULL, &status);

    // If overflow, then resize and retry
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        result.resize(size);
        status = U_ZERO_ERROR;
        size = unum_formatInt64(m_unum, from, reinterpret_cast<UChar *>(result.data()), size, NULL, &status);
    }

    // If successful on first or second go, resize and return
    if (U_SUCCESS(status)) {
        result.resize(size);
        return result;
    }

    return QString();
}

//TODO Fix for ICU >= 52
QString QIcuNumberFormatterPrivate::toString(quint64 from) const
{
    //TODO Handle too large properly!
    return toString(qint64(from));
}

QString QIcuNumberFormatterPrivate::toString(double from) const
{
    return qt_unum_formatDouble(m_unum, from);
}

qint64 QIcuNumberFormatterPrivate::toInt64(const QString &from, bool *ok, qint32 *pos) const
{
    qint64 res = 0;
    bool success = false;
    int upos = 0;

    if (m_unum) {
        UErrorCode status = U_ZERO_ERROR;
        // unum_parse(nf, text, textLength, pos, status)
        res = unum_parseInt64(m_unum, reinterpret_cast<const UChar *>(from.constData()),
                              from.size(), &upos, &status);
        success = U_SUCCESS(status);
    }

    if (ok)
        *ok = success;
    if (pos)
        *pos = upos;

    return res;
}

//TODO Fix for ICU >= 52
quint64 QIcuNumberFormatterPrivate::toUInt64(const QString &from, bool *ok, qint32 *pos) const
{
    qint64 res = 0;
    bool success = false;
    int upos = 0;

    if (m_unum) {
        UErrorCode status = U_ZERO_ERROR;
        // unum_parse(nf, text, textLength, pos, status)
        res = unum_parseInt64(m_unum, reinterpret_cast<const UChar *>(from.constData()),
                              from.size(), &upos, &status);
        success = U_SUCCESS(status);
    }
    // If we got a negative number back then it's invalid!
    if (success && res < 0)
        success = false;
        res = 0;
        upos = from.size() - 1;

    if (ok)
        *ok = success;
    if (pos)
        *pos = upos;

    return quint64(res);
}

double QIcuNumberFormatterPrivate::toDouble(const QString &from, bool *ok, qint32 *pos) const
{
    double res = 0;
    bool success = false;
        int upos = 0;

    if (m_unum) {
        UErrorCode status = U_ZERO_ERROR;
        // unum_parseDouble(nf, text, textLength, pos, status)
        res = unum_parseDouble(m_unum, reinterpret_cast<const UChar *>(from.constData()),
                         from.size(), &upos, &status);
        success = U_SUCCESS(status);
    }

    if (ok)
        *ok = success;
    if (pos)
        *pos = upos;

    return res;
}

QString QIcuNumberFormatterPrivate::toCurrencyString(double from) const
{
    return qt_unum_formatDouble(m_unum, from);
}

double QIcuNumberFormatterPrivate::toCurrencyDouble(const QString &from, bool *ok, qint32 *pos) const
{
    return qt_unum_parseDouble(m_unum, from, ok, pos);
}

QString QIcuNumberFormatterPrivate::toCurrencyString(double from, const QString &currencyCode) const
{
    int32_t size = 100;
    QString result(size, Qt::Uninitialized);
    UErrorCode status = U_ZERO_ERROR;
    QString code = currencyCode;

    // size = unum_formatDoubleCurrency(nf, from, currencyCode, result, resultLength, pos, status)
    size = unum_formatDoubleCurrency(m_unum, from, reinterpret_cast<UChar *>(code.data()),
                                     reinterpret_cast<UChar *>(result.data()), size, NULL, &status);

    // If overflow, then resize and retry
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        result.resize(size);
        status = U_ZERO_ERROR;
        size = unum_formatDoubleCurrency(m_unum, from, reinterpret_cast<UChar *>(code.data()),
                                         reinterpret_cast<UChar *>(result.data()), size, NULL, &status);
    }

    // If successful on first or second go, resize and return
    if (U_SUCCESS(status)) {
        result.resize(size);
        return result;
    }

    return QString();
}

double QIcuNumberFormatterPrivate::toCurrencyDouble(const QString &from, QString *currencyCode, bool *ok, int *pos) const
{
    UErrorCode status = U_ZERO_ERROR;
    int p = 0;
    QString code(4, Qt::Uninitialized);
    UChar uc[4];

    // unum_parseDoubleCurrency(nf, text, textLength, pos, currencyCode, status)
    double amt = unum_parseDoubleCurrency(m_unum, reinterpret_cast<const UChar *>(from.constData()),
                                          from.size(), &p,
                                          uc, &status);
                                          //reinterpret_cast<UChar *>(code.data()), &status);
    code = code.trimmed();
    code.squeeze();
    if (currencyCode)
        *currencyCode = code;
    if (ok)
        *ok = U_SUCCESS(status);
    if (pos)
        *pos = p;
//qDebug() << from << U_SUCCESS(status) << status << u_errorName(status) << amt << code;
    return amt;
}
