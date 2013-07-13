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

#include "qnumberformatter_p.h"
#include "qnumberformatterprivate_p.h"

#include <qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    Private
*/

// ICU Utilities

ULocDataLocaleType qt_localeTypeToIcuType(QLocale::LocaleCodeType type)
{
    switch (type) {
    case QLocale::RequestedLocaleCode :
        return ULOC_REQUESTED_LOCALE ;
    case QLocale::EffectiveLocaleCode :
        return ULOC_VALID_LOCALE;
    }
}

UNumberFormatStyle qt_numberFormatToIcuStyle(QNumberFormatter::NumberStyle style)
{
    switch (style) {
    case QNumberFormatter::DecimalStyle :
        return UNUM_DECIMAL;
    case QNumberFormatter::CurrencyStyle :
        return UNUM_CURRENCY;
    case QNumberFormatter::CurrencyCodeStyle :
        if (U_ICU_VERSION_SHORT < "48")
            return UNUM_CURRENCY;
        return UNUM_CURRENCY_ISO;
    case QNumberFormatter::CurrencyNameStyle :
        if (U_ICU_VERSION_SHORT < "48")
            return UNUM_CURRENCY;
        return UNUM_CURRENCY_PLURAL;
    case QNumberFormatter::PercentageStyle :
        return UNUM_PERCENT;
    case QNumberFormatter::ScientificStyle :
        return UNUM_SCIENTIFIC;
    case QNumberFormatter::OrdinalStyle :
        return UNUM_ORDINAL;
    case QNumberFormatter::DurationStyle :
        return UNUM_DURATION;
    case QNumberFormatter::SpelloutStyle :
        return UNUM_SPELLOUT;
    default :
        return UNUM_DECIMAL;
    }
}

UNumberFormatSymbol qt_numberSymbolToIcuSymbol(QNumberFormatter::NumberSymbol symbol)
{
    switch (symbol) {
    case QNumberFormatter::DecimalSeparatorSymbol :
        return UNUM_DECIMAL_SEPARATOR_SYMBOL;
    case QNumberFormatter::GroupingSeparatorSymbol :
        return UNUM_GROUPING_SEPARATOR_SYMBOL;
    case QNumberFormatter::PercentSymbol :
        return UNUM_PERCENT_SYMBOL;
    case QNumberFormatter::MinusSignSymbol :
        return UNUM_MINUS_SIGN_SYMBOL;
    case QNumberFormatter::PlusSignSymbol :
        return UNUM_PLUS_SIGN_SYMBOL;
    case QNumberFormatter::ExponentialSymbol :
        return UNUM_EXPONENTIAL_SYMBOL;
    case QNumberFormatter::PerMillSymbol :
        return UNUM_PERMILL_SYMBOL;
    case QNumberFormatter::InfinitySymbol :
        return UNUM_INFINITY_SYMBOL;
    case QNumberFormatter::NaNSymbol :
        return UNUM_NAN_SYMBOL;
    case QNumberFormatter::CurrencySymbol :
        return UNUM_CURRENCY_SYMBOL;
    case QNumberFormatter::CurrencyInternationalSymbol :
        return UNUM_INTL_CURRENCY_SYMBOL;
    case QNumberFormatter::MonetarySeparatorSymbol :
        return UNUM_MONETARY_SEPARATOR_SYMBOL;
    case QNumberFormatter::MonetaryGroupingSeparatorSymbol :
        return UNUM_MONETARY_GROUPING_SEPARATOR_SYMBOL;
    case QNumberFormatter::PatternSeparatorSymbol :
        return UNUM_PATTERN_SEPARATOR_SYMBOL;
    case QNumberFormatter::PatternDigitSymbol :
        return UNUM_DIGIT_SYMBOL;
    case QNumberFormatter::PatternSignificantDigitSymbol :
        return UNUM_SIGNIFICANT_DIGIT_SYMBOL ;
    case QNumberFormatter::PatternPadEscapeSymbol :
        return UNUM_PAD_ESCAPE_SYMBOL ;
    case QNumberFormatter::PatternFormatSymbolCount :
        return UNUM_FORMAT_SYMBOL_COUNT ;
    case QNumberFormatter::ZeroDigitSymbol :
        return UNUM_ZERO_DIGIT_SYMBOL;
    case QNumberFormatter::OneDigitSymbol :
        return UNUM_ONE_DIGIT_SYMBOL;
    case QNumberFormatter::TwoDigitSymbol :
        return UNUM_TWO_DIGIT_SYMBOL;
    case QNumberFormatter::ThreeDigitSymbol :
        return UNUM_THREE_DIGIT_SYMBOL;
    case QNumberFormatter::FourDigitSymbol :
        return UNUM_FOUR_DIGIT_SYMBOL;
    case QNumberFormatter::FiveDigitSymbol :
        return UNUM_FIVE_DIGIT_SYMBOL;
    case QNumberFormatter::SixDigitSymbol :
        return UNUM_SIX_DIGIT_SYMBOL;
    case QNumberFormatter::SevenDigitSymbol :
        return UNUM_SEVEN_DIGIT_SYMBOL;
    case QNumberFormatter::EightDigitSymbol :
        return UNUM_EIGHT_DIGIT_SYMBOL;
    case QNumberFormatter::NineDigitSymbol :
        return UNUM_NINE_DIGIT_SYMBOL;
    }
}

UNumberFormatAttribute qt_numberAttributeToIcuAttribute(QNumberFormatter::NumberAttribute attribute)
{
    switch (attribute) {
    case QNumberFormatter::MaximumIntegerDigits :
        return UNUM_MAX_INTEGER_DIGITS;
    case QNumberFormatter::MinimumIntegerDigits :
        return UNUM_MIN_INTEGER_DIGITS;
    case QNumberFormatter::MaximumFractionDigits :
        return UNUM_MAX_FRACTION_DIGITS;
    case QNumberFormatter::MinimumFractionDigits :
        return UNUM_MIN_FRACTION_DIGITS;
    case QNumberFormatter::MaximumSignificantDigits :
        return UNUM_MAX_SIGNIFICANT_DIGITS;
    case QNumberFormatter::MinimumSignificantDigits :
        return UNUM_MIN_SIGNIFICANT_DIGITS;
    case QNumberFormatter::Multiplier :
        return UNUM_MULTIPLIER;
    case QNumberFormatter::PrimaryGroupingSize :
        return UNUM_GROUPING_SIZE;
    case QNumberFormatter::SecondaryGroupingSize :
        return UNUM_SECONDARY_GROUPING_SIZE;
    }
}

UNumberFormatPadPosition qt_numberAttributeToIcuAttribute(QNumberFormatter::PadPosition pos)
{
    switch (pos) {
    case QNumberFormatter::PadBeforePrefix :
        return UNUM_PAD_BEFORE_PREFIX;
    case QNumberFormatter::PadAfterPrefix :
        return UNUM_PAD_AFTER_PREFIX;
    case QNumberFormatter::PadBeforeSuffix :
        return UNUM_PAD_BEFORE_SUFFIX;
    case QNumberFormatter::PadAfterSuffix :
        return UNUM_PAD_AFTER_SUFFIX;
    }
}

QNumberFormatter::PadPosition qt_icuPositionToPadPosition(UNumberFormatPadPosition pos)
{
    switch (pos) {
    case UNUM_PAD_BEFORE_PREFIX :
        return QNumberFormatter::PadBeforePrefix;
    case UNUM_PAD_AFTER_PREFIX :
        return QNumberFormatter::PadAfterPrefix;
    case UNUM_PAD_BEFORE_SUFFIX :
        return QNumberFormatter::PadBeforeSuffix;
    case UNUM_PAD_AFTER_SUFFIX :
        return QNumberFormatter::PadAfterSuffix;
    }
}

UNumberFormatRoundingMode qt_roundingModeToIcuMode(QNumberFormatter::RoundingMode mode)
{
    switch (mode) {
    case QNumberFormatter::RoundCeiling :
        return UNUM_ROUND_CEILING;
    case QNumberFormatter::RoundFloor :
        return UNUM_ROUND_FLOOR;
    case QNumberFormatter::RoundDown :
        return UNUM_ROUND_DOWN;
    case QNumberFormatter::RoundUp :
        return UNUM_ROUND_UP;
    case QNumberFormatter::RoundHalfEven :
        return UNUM_ROUND_HALFEVEN;
    case QNumberFormatter::RoundHalfDown :
        return UNUM_ROUND_HALFDOWN;
    case QNumberFormatter::RoundHalfUp :
        return UNUM_ROUND_HALFUP;
    case QNumberFormatter::RoundUnnecessary :
        return UNUM_ROUND_UNNECESSARY;
    }
}

QNumberFormatter::RoundingMode qt_roundingModeToIcuMode(UNumberFormatRoundingMode mode)
{
    switch (mode) {
    case UNUM_ROUND_CEILING :
        return QNumberFormatter::RoundCeiling;
    case UNUM_ROUND_FLOOR :
        return QNumberFormatter::RoundFloor;
    case UNUM_ROUND_DOWN :
        return QNumberFormatter::RoundDown;
    case UNUM_ROUND_UP :
        return QNumberFormatter::RoundUp;
    case UNUM_ROUND_HALFEVEN :
        return QNumberFormatter::RoundHalfEven;
    case UNUM_ROUND_HALFDOWN :
        return QNumberFormatter::RoundHalfDown;
    case UNUM_ROUND_HALFUP :
        return QNumberFormatter::RoundHalfUp;
    case UNUM_ROUND_UNNECESSARY :
        return QNumberFormatter::RoundUnnecessary;
    }
}

QLocale::ParsingMode qt_unum_parsingMode(UNumberFormat *nf)
{
    if (unum_getAttribute(nf, UNUM_LENIENT_PARSE))
        return QLocale::LenientParsing;
    return QLocale::StrictParsing;
}

QString qt_unum_getLocaleByType(UNumberFormat *nf, ULocDataLocaleType type)
{
    UErrorCode status = U_ZERO_ERROR;
    // unum_getLocaleByType(nf, type, error)
    QString locale = unum_getLocaleByType(nf, type , &status);
    return locale;
}

QString qt_unum_getTextAttribute(UNumberFormat *nf, UNumberFormatTextAttribute attribute)
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

void qt_unum_setTextAttribute(UNumberFormat *nf, UNumberFormatTextAttribute attribute, const QString &value)
{
    UErrorCode status = U_ZERO_ERROR;
    // unum_setAttribute(nf, attribute, value, valueSize, status)
    unum_setTextAttribute(nf, attribute, reinterpret_cast<const UChar *>(value.constData()),
                          value.size(), &status);
}

QString qt_unum_getSymbol(UNumberFormat *nf, UNumberFormatSymbol symbol)
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

void qt_unum_setSymbol(UNumberFormat *nf, UNumberFormatSymbol symbol, const QString &value)
{
    UErrorCode status = U_ZERO_ERROR;
    // unum_setAttribute(nf, symbol, value, valueSize, status)
    unum_setSymbol(nf, symbol, reinterpret_cast<const UChar *>(value.constData()),
                   value.size(), &status);
}

QString qt_unum_toPattern(UNumberFormat *nf)
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

void qt_unum_applyPattern(UNumberFormat *nf, const QString &value)
{
    UErrorCode status = U_ZERO_ERROR;
    // unum_applyPattern(nf, localized, value, valueSize, error, status)
    unum_applyPattern(nf, FALSE, reinterpret_cast<const UChar *>(value.constData()),
                      value.size(), NULL, &status);
}

void qt_setPadding(UNumberFormat *nf, qint32 width, const QString &padding,
                   QNumberFormatter::PadPosition position)
{
    // Set padding if not default values
    if (width >= 0)
        unum_setAttribute(nf,  UNUM_FORMAT_WIDTH, width);
    if (!padding.isNull())
        qt_unum_setTextAttribute(nf, UNUM_PADDING_CHARACTER, padding);
    if (position != QNumberFormatter::DefaultPadPosition)
        unum_setAttribute(nf,  UNUM_PADDING_POSITION, qt_numberAttributeToIcuAttribute(position));
}

void qt_setRounding(UNumberFormat *nf, QNumberFormatter::RoundingMode mode, double increment)
{
    if (mode != QNumberFormatter::DefaultRoundingMode)
        unum_setAttribute(nf, UNUM_ROUNDING_MODE , qt_roundingModeToIcuMode(mode));
    if (increment >= 0.0)
        unum_setDoubleAttribute(nf, UNUM_ROUNDING_INCREMENT, increment);
}

QString qt_unum_formatInt32(UNumberFormat *nf, qint32 from)
{
    int32_t size = 30;
    QString result(size, Qt::Uninitialized);
    UErrorCode status = U_ZERO_ERROR;

    // size = unum_format(nf, from, result, resultLength, pos, status)
    size = unum_format(nf, from, reinterpret_cast<UChar *>(result.data()), size, NULL, &status);

    // If overflow, then resize and retry
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        result.resize(size);
        status = U_ZERO_ERROR;
        size = unum_format(nf, from, reinterpret_cast<UChar *>(result.data()), size, NULL, &status);
    }

    // If successful on first or second go, resize and return
    if (U_SUCCESS(status)) {
        result.resize(size);
        return result;
    }

    return QString();
}

QString qt_unum_formatInt64(UNumberFormat *nf, qint64 from)
{
    int32_t size = 60;
    QString result(size, Qt::Uninitialized);
    UErrorCode status = U_ZERO_ERROR;

    // size = unum_format(nf, from, result, resultLength, pos, status)
    size = unum_formatInt64(nf, from, reinterpret_cast<UChar *>(result.data()), size, NULL, &status);

    // If overflow, then resize and retry
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        result.resize(size);
        status = U_ZERO_ERROR;
        size = unum_formatInt64(nf, from, reinterpret_cast<UChar *>(result.data()), size, NULL, &status);
    }

    // If successful on first or second go, resize and return
    if (U_SUCCESS(status)) {
        result.resize(size);
        return result;
    }

    return QString();
}

QString qt_unum_formatDouble(UNumberFormat *nf, double from)
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

QString qt_unum_formatDoubleCurrency(UNumberFormat *nf, double from, QString currencyCode)
{
    int32_t size = 100;
    QString result(size, Qt::Uninitialized);
    UErrorCode status = U_ZERO_ERROR;

    // size = unum_formatDoubleCurrency(nf, from, currencyCode, result, resultLength, pos, status)
    size = unum_formatDoubleCurrency(nf, from, reinterpret_cast<UChar *>(currencyCode.data()),
                                     reinterpret_cast<UChar *>(result.data()), size, NULL, &status);

    // If overflow, then resize and retry
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        result.resize(size);
        status = U_ZERO_ERROR;
        size = unum_formatDoubleCurrency(nf, from, reinterpret_cast<UChar *>(currencyCode.data()),
                                         reinterpret_cast<UChar *>(result.data()), size, NULL, &status);
    }

    // If successful on first or second go, resize and return
    if (U_SUCCESS(status)) {
        result.resize(size);
        return result;
    }

    return QString();
}

UNumberFormat *qt_parseFormatter(UNumberFormat *mnf, QLocale::ParsingMode mode)
{
    // If same parsing mode, use the main formatter
    if (mode == QLocale::DefaultParsingMode || mode == qt_unum_parsingMode(mnf))
        return mnf;

    // If not same parsing mode then clone the formatter
    UErrorCode status = U_ZERO_ERROR;
    UNumberFormat *nf = unum_clone(mnf, &status);
    if (!U_SUCCESS(status))
        return 0;

    unum_setAttribute(nf, UNUM_LENIENT_PARSE, (mode == QLocale::LenientParsing));
    return nf;
}

void qt_printSettings(UNumberFormat *nf)
{
    qDebug() << "ULOC_REQUESTED_LOCALE                   "
             << qt_unum_getLocaleByType(nf, ULOC_REQUESTED_LOCALE);
    qDebug() << "ULOC_VALID_LOCALE                       "
             << qt_unum_getLocaleByType(nf, ULOC_VALID_LOCALE);
    qDebug() << "ULOC_ACTUAL_LOCALE                      "
             << qt_unum_getLocaleByType(nf, ULOC_ACTUAL_LOCALE);
    qDebug() << "UNUM_PARSE_INT_ONLY                     "
             << unum_getAttribute(nf, UNUM_PARSE_INT_ONLY);
    qDebug() << "UNUM_GROUPING_USED                      "
             << unum_getAttribute(nf, UNUM_GROUPING_USED);
    qDebug() << "UNUM_DECIMAL_ALWAYS_SHOWN               "
             << unum_getAttribute(nf, UNUM_DECIMAL_ALWAYS_SHOWN);
    qDebug() << "UNUM_MAX_INTEGER_DIGITS                 "
             << unum_getAttribute(nf, UNUM_MAX_INTEGER_DIGITS);
    qDebug() << "UNUM_MIN_INTEGER_DIGITS                 "
    << unum_getAttribute(nf, UNUM_MIN_INTEGER_DIGITS);
    qDebug() << "UNUM_INTEGER_DIGITS                     "
    << unum_getAttribute(nf, UNUM_INTEGER_DIGITS);
    qDebug() << "UNUM_MAX_FRACTION_DIGITS                "
    << unum_getAttribute(nf, UNUM_MAX_FRACTION_DIGITS);
    qDebug() << "UNUM_MIN_FRACTION_DIGITS                "
    << unum_getAttribute(nf, UNUM_MIN_FRACTION_DIGITS);
    qDebug() << "UNUM_FRACTION_DIGITS                    "
    << unum_getAttribute(nf, UNUM_FRACTION_DIGITS);
    qDebug() << "UNUM_MULTIPLIER                         "
    << unum_getAttribute(nf, UNUM_MULTIPLIER);
    qDebug() << "UNUM_GROUPING_SIZE                      "
    << unum_getAttribute(nf, UNUM_GROUPING_SIZE);
    qDebug() << "UNUM_ROUNDING_MODE                      "
    << unum_getAttribute(nf, UNUM_ROUNDING_MODE);
    qDebug() << "UNUM_ROUNDING_INCREMENT                 "
    << unum_getDoubleAttribute(nf, UNUM_ROUNDING_INCREMENT);
    qDebug() << "UNUM_FORMAT_WIDTH                       "
    << unum_getAttribute(nf, UNUM_FORMAT_WIDTH);
    qDebug() << "UNUM_PADDING_POSITION                   "
    << unum_getAttribute(nf, UNUM_PADDING_POSITION);
    qDebug() << "UNUM_SECONDARY_GROUPING_SIZE            "
    << unum_getAttribute(nf, UNUM_SECONDARY_GROUPING_SIZE);
    qDebug() << "UNUM_SIGNIFICANT_DIGITS_USED            "
    << unum_getAttribute(nf, UNUM_SIGNIFICANT_DIGITS_USED);
    qDebug() << "UNUM_MIN_SIGNIFICANT_DIGITS             "
    << unum_getAttribute(nf, UNUM_MIN_SIGNIFICANT_DIGITS);
    qDebug() << "UNUM_MAX_SIGNIFICANT_DIGITS             "
    << unum_getAttribute(nf, UNUM_MAX_SIGNIFICANT_DIGITS);
    qDebug() << "UNUM_LENIENT_PARSE                      "
    << unum_getAttribute(nf, UNUM_LENIENT_PARSE);
    qDebug() << "UNUM_PADDING_CHARACTER                  "
    << qt_unum_getTextAttribute(nf, UNUM_PADDING_CHARACTER);
    qDebug() << "UNUM_POSITIVE_PREFIX                    "
    << qt_unum_getTextAttribute(nf, UNUM_POSITIVE_PREFIX);
    qDebug() << "UNUM_POSITIVE_SUFFIX                    "
    << qt_unum_getTextAttribute(nf, UNUM_POSITIVE_SUFFIX);
    qDebug() << "UNUM_NEGATIVE_PREFIX                    "
    << qt_unum_getTextAttribute(nf, UNUM_NEGATIVE_PREFIX);
    qDebug() << "UNUM_NEGATIVE_SUFFIX                    "
    << qt_unum_getTextAttribute(nf, UNUM_NEGATIVE_SUFFIX);
    qDebug() << "UNUM_CURRENCY_CODE                      "
    << qt_unum_getTextAttribute(nf, UNUM_CURRENCY_CODE);
    qDebug() << "UNUM_DEFAULT_RULESET                    "
    << qt_unum_getTextAttribute(nf, UNUM_DEFAULT_RULESET);
    qDebug() << "UNUM_PUBLIC_RULESETS                    "
    << qt_unum_getTextAttribute(nf, UNUM_PUBLIC_RULESETS);
    qDebug() << "UNUM_DECIMAL_SEPARATOR_SYMBOL           "
    << qt_unum_getSymbol(nf, UNUM_DECIMAL_SEPARATOR_SYMBOL);
    qDebug() << "UNUM_GROUPING_SEPARATOR_SYMBOL          "
    << qt_unum_getSymbol(nf, UNUM_GROUPING_SEPARATOR_SYMBOL);
    qDebug() << "UNUM_PATTERN_SEPARATOR_SYMBOL           "
    << qt_unum_getSymbol(nf, UNUM_PATTERN_SEPARATOR_SYMBOL);
    qDebug() << "UNUM_PERCENT_SYMBOL                     "
    << qt_unum_getSymbol(nf, UNUM_PERCENT_SYMBOL);
    qDebug() << "UNUM_DIGIT_SYMBOL                       "
    << qt_unum_getSymbol(nf, UNUM_DIGIT_SYMBOL);
    qDebug() << "UNUM_MINUS_SIGN_SYMBOL                  "
    << qt_unum_getSymbol(nf, UNUM_MINUS_SIGN_SYMBOL);
    qDebug() << "UNUM_PLUS_SIGN_SYMBOL                   "
    << qt_unum_getSymbol(nf, UNUM_PLUS_SIGN_SYMBOL);
    qDebug() << "UNUM_CURRENCY_SYMBOL                    "
    << qt_unum_getSymbol(nf, UNUM_CURRENCY_SYMBOL);
    qDebug() << "UNUM_INTL_CURRENCY_SYMBOL               "
    << qt_unum_getSymbol(nf, UNUM_INTL_CURRENCY_SYMBOL);
    qDebug() << "UNUM_MONETARY_SEPARATOR_SYMBOL          "
    << qt_unum_getSymbol(nf, UNUM_MONETARY_SEPARATOR_SYMBOL);
    qDebug() << "UNUM_EXPONENTIAL_SYMBOL                 "
    << qt_unum_getSymbol(nf, UNUM_EXPONENTIAL_SYMBOL);
    qDebug() << "UNUM_PERMILL_SYMBOL                     "
    << qt_unum_getSymbol(nf, UNUM_PERMILL_SYMBOL);
    qDebug() << "UNUM_PAD_ESCAPE_SYMBOL                  "
    << qt_unum_getSymbol(nf, UNUM_PAD_ESCAPE_SYMBOL);
    qDebug() << "UNUM_INFINITY_SYMBOL                    "
    << qt_unum_getSymbol(nf, UNUM_INFINITY_SYMBOL);
    qDebug() << "UNUM_NAN_SYMBOL                         "
    << qt_unum_getSymbol(nf, UNUM_NAN_SYMBOL);
    qDebug() << "UNUM_SIGNIFICANT_DIGIT_SYMBOL           "
    << qt_unum_getSymbol(nf, UNUM_SIGNIFICANT_DIGIT_SYMBOL);
    qDebug() << "UNUM_MONETARY_GROUPING_SEPARATOR_SYMBOL "
    << qt_unum_getSymbol(nf, UNUM_MONETARY_GROUPING_SEPARATOR_SYMBOL);
    qDebug() << "UNUM_ZERO_DIGIT_SYMBOL                  "
    << qt_unum_getSymbol(nf, UNUM_ZERO_DIGIT_SYMBOL);
    qDebug() << "UNUM_ONE_DIGIT_SYMBOL                   "
    << qt_unum_getSymbol(nf, UNUM_ONE_DIGIT_SYMBOL);
    qDebug() << "UNUM_TWO_DIGIT_SYMBOL                   "
    << qt_unum_getSymbol(nf, UNUM_TWO_DIGIT_SYMBOL);
    qDebug() << "UNUM_THREE_DIGIT_SYMBOL                 "
    << qt_unum_getSymbol(nf, UNUM_THREE_DIGIT_SYMBOL);
    qDebug() << "UNUM_FIVE_DIGIT_SYMBOL                  "
    << qt_unum_getSymbol(nf, UNUM_FIVE_DIGIT_SYMBOL);
    qDebug() << "UNUM_SIX_DIGIT_SYMBOL                   "
    << qt_unum_getSymbol(nf, UNUM_SIX_DIGIT_SYMBOL);
    qDebug() << "UNUM_SEVEN_DIGIT_SYMBOL                 "
    << qt_unum_getSymbol(nf, UNUM_SEVEN_DIGIT_SYMBOL);
    qDebug() << "UNUM_EIGHT_DIGIT_SYMBOL                 "
    << qt_unum_getSymbol(nf, UNUM_EIGHT_DIGIT_SYMBOL);
    qDebug() << "UNUM_NINE_DIGIT_SYMBOL                  "
    << qt_unum_getSymbol(nf, UNUM_NINE_DIGIT_SYMBOL);
    qDebug() << "UNUM_FORMAT_SYMBOL_COUNT                "
    << qt_unum_getSymbol(nf, UNUM_FORMAT_SYMBOL_COUNT);
}

QIcuNumberFormatterPrivate::QIcuNumberFormatterPrivate(QNumberFormatter::NumberStyle style,
                                                 const QString &pattern)
{
    init(QString(), style, pattern);
}

QIcuNumberFormatterPrivate::QIcuNumberFormatterPrivate(const QString &locale,
                                                 QNumberFormatter::NumberStyle style,
                                                 const QString &pattern)
{
    init(locale, style, pattern);
}

void QIcuNumberFormatterPrivate::init(const QString &locale,
                                   QNumberFormatter::NumberStyle style,
                                   const QString &pattern)
{
    m_style = style;
    m_status = U_ZERO_ERROR;

    if (style == QNumberFormatter::DecimalPatternStyle || style == QNumberFormatter::RuleBasedStyle)
        //unum_open(style, pattern, patternLength, locale, parseErr, status)
        m_nf = unum_open(qt_numberFormatToIcuStyle(style),
                         reinterpret_cast<const UChar *>(pattern.data()), pattern.size(),
                         locale.toUtf8(), NULL, &m_status);
    else
        //unum_open(style, pattern, patternLength, locale, parseErr, status)
        m_nf = unum_open(qt_numberFormatToIcuStyle(style), NULL, -1, locale.toUtf8(), NULL, &m_status);

    // Match QLocale default behaviour
    if (isValid())
        unum_setAttribute(m_nf, UNUM_MAX_FRACTION_DIGITS, 6);

    // Debug code
    //if (isValid())
    //    qt_printSettings(m_nf);
}

QIcuNumberFormatterPrivate::~QIcuNumberFormatterPrivate()
{
    unum_close(m_nf);
}

bool QIcuNumberFormatterPrivate::isValid() const
{
    return (U_SUCCESS(m_status));
}

bool QIcuNumberFormatterPrivate::isInvalid() const
{
    return (!isValid());
}

QString QIcuNumberFormatterPrivate::localeCode(QLocale::LocaleCodeType type) const
{
    return qt_unum_getLocaleByType(m_nf, qt_localeTypeToIcuType(type));
}

QNumberFormatter::NumberStyle QIcuNumberFormatterPrivate::numberStyle() const
{
    return m_style;
}

QString QIcuNumberFormatterPrivate::currencyCode() const
{
    return qt_unum_getTextAttribute(m_nf, UNUM_CURRENCY_CODE);
}

QString QIcuNumberFormatterPrivate::numberSymbol(QNumberFormatter::NumberSymbol symbol) const
{
    return qt_unum_getSymbol(m_nf, qt_numberSymbolToIcuSymbol(symbol));
}

qint32 QIcuNumberFormatterPrivate::numberAttribute(QNumberFormatter::NumberAttribute attribute) const
{
    return unum_getAttribute(m_nf, qt_numberAttributeToIcuAttribute(attribute));
}

QString QIcuNumberFormatterPrivate::pattern() const
{
    return qt_unum_toPattern(m_nf);
}

bool QIcuNumberFormatterPrivate::usesSignificantDigits() const
{
    return unum_getAttribute(m_nf, UNUM_SIGNIFICANT_DIGITS_USED);
}

QNumberFormatter::RoundingMode QIcuNumberFormatterPrivate::roundingMode() const
{
    return qt_roundingModeToIcuMode((UNumberFormatRoundingMode)unum_getAttribute(m_nf, UNUM_ROUNDING_MODE));
}

double QIcuNumberFormatterPrivate::roundingIncrement() const
{
    return unum_getDoubleAttribute(m_nf, UNUM_ROUNDING_INCREMENT);
}

qint32 QIcuNumberFormatterPrivate::paddingWidth() const
{
    return unum_getAttribute(m_nf, UNUM_FORMAT_WIDTH);
}

QString QIcuNumberFormatterPrivate::padding() const
{
    return qt_unum_getTextAttribute(m_nf, UNUM_PADDING_CHARACTER);
}

QNumberFormatter::PadPosition QIcuNumberFormatterPrivate::paddingPosition() const
{
    return qt_icuPositionToPadPosition((UNumberFormatPadPosition)unum_getAttribute(m_nf, UNUM_PADDING_POSITION));
}

void QIcuNumberFormatterPrivate::setCurrencyCode(const QString &value)
{
    qt_unum_setTextAttribute(m_nf, UNUM_CURRENCY_CODE, value);
}

void QIcuNumberFormatterPrivate::setNumberSymbol(QNumberFormatter::NumberSymbol symbol, const QString &value)
{
    qt_unum_setSymbol(m_nf, qt_numberSymbolToIcuSymbol(symbol), value);
}

void QIcuNumberFormatterPrivate::setNumberAttribute(QNumberFormatter::NumberAttribute attribute, qint32 value)
{
    unum_setAttribute(m_nf, qt_numberAttributeToIcuAttribute(attribute), value);
}

void QIcuNumberFormatterPrivate::setPattern(const QString & pattern)
{
    qt_unum_applyPattern(m_nf, pattern);
}

void QIcuNumberFormatterPrivate::setUsesSignificantDigits(bool value)
{
    unum_setAttribute(m_nf, UNUM_SIGNIFICANT_DIGITS_USED, value);
}

void QIcuNumberFormatterPrivate::setRounding(QNumberFormatter::RoundingMode mode, double increment)
{
    qt_setRounding(m_nf, mode, increment);
}

void QIcuNumberFormatterPrivate::setPadding(qint32 width, const QString &padding,
                                         QNumberFormatter::PadPosition position)
{
    qt_setPadding(m_nf, width, padding, position);
}

QLocale::ParsingMode QIcuNumberFormatterPrivate::parsingMode() const
{
    return(qt_unum_parsingMode(m_nf));
}

void QIcuNumberFormatterPrivate::setParsingMode(QLocale::ParsingMode mode)
{
    // TODO is default strict or lenient???
    if (mode == QLocale::DefaultParsingMode)
        unum_setAttribute(m_nf, UNUM_LENIENT_PARSE, 0);
    else
        unum_setAttribute(m_nf, UNUM_LENIENT_PARSE, (mode == QLocale::LenientParsing));
}

QString QIcuNumberFormatterPrivate::toString(qint32 from) const
{
    return qt_unum_formatInt32(m_nf, from);
}

QString QIcuNumberFormatterPrivate::toString(qint64 from) const
{
    return qt_unum_formatInt64(m_nf, from);
}

QString QIcuNumberFormatterPrivate::toString(double from) const
{
    return qt_unum_formatDouble(m_nf, from);
}

QString QIcuNumberFormatterPrivate::toString(qint32 from, int width, const QString &padding,
                                          QNumberFormatter::PadPosition position) const
{
    // Clone the nf so we don't change the shared object
    UErrorCode status = U_ZERO_ERROR;
    UNumberFormat *nf = unum_clone(m_nf, &status);
    if (!U_SUCCESS(status))
        return QString();

    // Set the attributes only if not default values
    qt_setPadding(nf, width, padding, position);

    QString result = qt_unum_formatInt32(nf, from);
    unum_close(nf);
    return result;
}

QString QIcuNumberFormatterPrivate::toString(qint64 from, int width, const QString &padding,
                                          QNumberFormatter::PadPosition position) const
{
    // Clone the nf so we don't change the shared object
    UErrorCode status = U_ZERO_ERROR;
    UNumberFormat *nf = unum_clone(m_nf, &status);
    if (!U_SUCCESS(status))
        return QString();

    // Set the attributes only if not default values
    qt_setPadding(nf, width, padding, position);

    QString result = qt_unum_formatInt64(nf, from);
    unum_close(nf);
    return result;
}

QString QIcuNumberFormatterPrivate::toString(double from, int width, const QString &padding,
                                          QNumberFormatter::PadPosition position) const
{
    // Clone the nf so we don't change the shared object
    UErrorCode status = U_ZERO_ERROR;
    UNumberFormat *nf = unum_clone(m_nf, &status);
    if (!U_SUCCESS(status))
        return QString();

    // Set the attributes only if not default values
    qt_setPadding(nf, width, padding, position);

    QString result = qt_unum_formatDouble(nf, from);
    unum_close(nf);
    return result;
}

QString QIcuNumberFormatterPrivate::toString(double from,
                                          int maxIntDigits, int minIntDigits,
                                          int maxFracDigits, int minFracDigits,
                                          QNumberFormatter::RoundingMode roundMode,
                                          double roundIncrement) const
{
    return toString(from, maxIntDigits, minIntDigits, maxFracDigits, minFracDigits,
                    -1, QString(), QNumberFormatter::DefaultPadPosition,
                    roundMode, roundIncrement);
}

QString QIcuNumberFormatterPrivate::toString(double from,
                                          int maxSigDigits, int minSigDigits,
                                          QNumberFormatter::RoundingMode roundMode,
                                          double roundIncrement) const
{
    return toString(from, maxSigDigits, minSigDigits,
                    -1, QString(), QNumberFormatter::DefaultPadPosition,
                    roundMode, roundIncrement);
}

QString QIcuNumberFormatterPrivate::toString(double from,
                                          int maxIntDigits, int minIntDigits,
                                          int maxFracDigits, int minFracDigits,
                                          int width, const QString &padding,
                                          QNumberFormatter::PadPosition position,
                                          QNumberFormatter::RoundingMode mode, double increment) const
{
    // Clone the nf so we don't change the shared object
    UErrorCode status = U_ZERO_ERROR;
    UNumberFormat *nf = unum_clone(m_nf, &status);
    if (!U_SUCCESS(status))
        return QString();

    // Set the required size
    unum_setAttribute(nf, UNUM_MAX_INTEGER_DIGITS, maxIntDigits);
    unum_setAttribute(nf, UNUM_MIN_INTEGER_DIGITS, minIntDigits);
    unum_setAttribute(nf, UNUM_MAX_FRACTION_DIGITS, maxFracDigits);
    unum_setAttribute(nf, UNUM_MIN_FRACTION_DIGITS, minFracDigits);
    unum_setAttribute(nf, UNUM_SIGNIFICANT_DIGITS_USED, 0);

    // Set the optional attributes only if not default values
    qt_setPadding(nf, width, padding, position);
    qt_setRounding(nf, mode, increment);

    QString result = qt_unum_formatDouble(nf, from);
    unum_close(nf);
    return result;
}

QString QIcuNumberFormatterPrivate::toString(double from,
                                          int maxSigDigits, int minSigDigits,
                                          int width, const QString &padding,
                                          QNumberFormatter::PadPosition position,
                                          QNumberFormatter::RoundingMode mode, double increment) const
{
    // Clone the nf so we don't change the shared object
    UErrorCode status = U_ZERO_ERROR;
    UNumberFormat *nf = unum_clone(m_nf, &status);
    if (!U_SUCCESS(status))
        return QString();

    // Set the required size
    unum_setAttribute(nf, UNUM_MAX_SIGNIFICANT_DIGITS, maxSigDigits);
    unum_setAttribute(nf, UNUM_MIN_SIGNIFICANT_DIGITS, minSigDigits);
    unum_setAttribute(nf, UNUM_SIGNIFICANT_DIGITS_USED, 1);

    // Set the optional attributes only if not default values
    qt_setPadding(nf, width, padding, position);
    qt_setRounding(nf, mode, increment);

    QString result = qt_unum_formatDouble(nf, from);
    unum_close(nf);
    return result;
}

qint32 QIcuNumberFormatterPrivate::toInt32(const QString &from, bool *ok,
                                        QLocale::ParsingMode mode) const
{
    qint32 res = 0;
    bool success = false;

    // If not current parsing mode then clone the formatter
    UNumberFormat *nf = qt_parseFormatter(m_nf, mode);

    if (nf) {
        UErrorCode status = U_ZERO_ERROR;
        int pos = 0;
        // unum_parse(nf, text, textLength, pos, status)
        res = unum_parse(m_nf, reinterpret_cast<const UChar *>(from.constData()),
                         from.size(), &pos, &status);
        success = U_SUCCESS(status);
    }

    if (ok)
        *ok = success;

    return res;
}

qint64 QIcuNumberFormatterPrivate::toInt64(const QString &from, bool *ok,
                                        QLocale::ParsingMode mode) const
{
    qint64 res = 0;
    bool success = false;

    // If not current parsing mode then clone the formatter
    UNumberFormat *nf = qt_parseFormatter(m_nf, mode);

    if (nf) {
        UErrorCode status = U_ZERO_ERROR;
        int pos = 0;
        // unum_parse(nf, text, textLength, pos, status)
        res = unum_parseInt64(m_nf, reinterpret_cast<const UChar *>(from.constData()),
                         from.size(), &pos, &status);
        success = U_SUCCESS(status);
    }

    if (ok)
        *ok = success;

    return res;
}

double QIcuNumberFormatterPrivate::toDouble(const QString &from, bool *ok,
                                         QLocale::ParsingMode mode) const
{
    double res = 0;
    bool success = false;

    // If not current parsing mode then clone the formatter
    UNumberFormat *nf = qt_parseFormatter(m_nf, mode);

    if (nf) {
        UErrorCode status = U_ZERO_ERROR;
        int pos = 0;
        // unum_parse(nf, text, textLength, pos, status)
        res = unum_parseDouble(m_nf, reinterpret_cast<const UChar *>(from.constData()),
                         from.size(), &pos, &status);
        success = U_SUCCESS(status);
    }

    if (ok)
        *ok = success;

    return res;
}

QString QIcuNumberFormatterPrivate::toAnyCurrency(double from, const QString &currencyCode,
                                               int width, const QString &padding,
                                               QNumberFormatter::PadPosition position,
                                               QNumberFormatter::RoundingMode mode,
                                               double increment) const
{
    // If padding or rounding are not the default, clone the nf so we don't change the shared object
    if (width >= 0 || !padding.isNull() || position != QNumberFormatter::DefaultPadPosition ||
        mode != QNumberFormatter::DefaultRoundingMode || increment >= 0.0) {
        UErrorCode status = U_ZERO_ERROR;
        UNumberFormat *nf = unum_clone(m_nf, &status);
        if (!U_SUCCESS(status))
            return QString();

        // Set the optional attributes only if not default values
        qt_setPadding(nf, width, padding, position);
        qt_setRounding(nf, mode, increment);

        QString result = qt_unum_formatDoubleCurrency(nf, from, currencyCode);
        unum_close(nf);
        return result;
    }

    return qt_unum_formatDoubleCurrency(m_nf, from, currencyCode);
}

double QIcuNumberFormatterPrivate::fromAnyCurrency(const QString &from, QString *currencyCode, bool *ok,
                                                QLocale::ParsingMode mode) const
{
    UErrorCode status = U_ZERO_ERROR;
    int pos = 0;
    QString code(4, Qt::Uninitialized);
    UChar uc[4];

    // unum_parseDoubleCurrency(nf, text, textLength, pos, currencyCode, status)
    double amt = unum_parseDoubleCurrency(m_nf, reinterpret_cast<const UChar *>(from.constData()),
                                          from.size(), &pos,
                                          uc, &status);
                                          //reinterpret_cast<UChar *>(code.data()), &status);
    code = code.trimmed();
    code.squeeze();
    if (currencyCode)
        *currencyCode = code;
    if (ok)
        *ok = U_SUCCESS(status);
//qDebug() << from << U_SUCCESS(status) << status << u_errorName(status) << amt << code;
    return amt;
}
