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

#include "qnumberformatter.h"
#include "qnumberformatterprivate_p.h"

#include "private/qcore_mac_p.h"

#include <Foundation/NSNumberFormatter.h>
#include <qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    Private
*/

// Mac utilities

static NSNumberFormatterStyle qt_numberStyleToMacStyle(QNumberFormatter::NumberStyle style)
{
    switch (style) {
    case QNumberFormatter::DecimalStyle :
        return NSNumberFormatterDecimalStyle;
    case QNumberFormatter::ScientificStyle :
        return NSNumberFormatterScientificStyle;
    case QNumberFormatter::CurrencyStyle :
    case QNumberFormatter::CurrencyCodeStyle :
    case QNumberFormatter::CurrencyNameStyle :
        return NSNumberFormatterCurrencyStyle;
    case QNumberFormatter::PercentageStyle :
        return NSNumberFormatterPercentStyle;
    case QNumberFormatter::SpelloutStyle :
        return NSNumberFormatterSpellOutStyle;
    case QNumberFormatter::OrdinalStyle :
    case QNumberFormatter::DurationStyle :
    default :
        return NSNumberFormatterDecimalStyle;
    }
}

static void qt_setMacPadding(NSNumberFormatter *nf, qint32 width, const QString &padding,
                             QNumberFormatter::PadPosition position)
{
    if (width >= 0)
        [nf setFormatWidth:width];

    if (!padding.isNull())
        [nf setPaddingCharacter:QCFString::toNSString(padding)];

    switch (position) {
    case QNumberFormatter::PadBeforePrefix:
        [nf setPaddingPosition:NSNumberFormatterPadBeforePrefix];
    case QNumberFormatter::PadAfterPrefix:
        [nf setPaddingPosition:NSNumberFormatterPadAfterPrefix];
    case QNumberFormatter::PadBeforeSuffix:
        [nf setPaddingPosition:NSNumberFormatterPadBeforeSuffix];
    case QNumberFormatter::PadAfterSuffix:
        [nf setPaddingPosition:NSNumberFormatterPadAfterSuffix];
    case QNumberFormatter::DefaultPadPosition:
        // TODO What is the default?
        [nf setPaddingPosition:NSNumberFormatterPadBeforePrefix];
    }
}

static void qt_setMacRounding(NSNumberFormatter *nf,
                              QNumberFormatter::RoundingMode roundMode, double roundIncrement)
{
    switch (roundMode) {
    case QNumberFormatter::RoundCeiling:
        [nf setRoundingMode:NSNumberFormatterRoundCeiling];
    case QNumberFormatter::RoundFloor:
        [nf setRoundingMode:NSNumberFormatterRoundFloor];
    case QNumberFormatter::RoundDown:
        [nf setRoundingMode:NSNumberFormatterRoundDown];
    case QNumberFormatter::RoundUp:
        [nf setRoundingMode:NSNumberFormatterRoundUp];
    case QNumberFormatter::RoundHalfEven:
        [nf setRoundingMode:NSNumberFormatterRoundHalfEven];
    case QNumberFormatter::RoundHalfDown:
        [nf setRoundingMode:NSNumberFormatterRoundHalfDown];
    case QNumberFormatter::RoundHalfUp:
        [nf setRoundingMode:NSNumberFormatterRoundHalfUp];
    case QNumberFormatter::RoundUnnecessary:
    case QNumberFormatter::DefaultRoundingMode:
        // TODO What values???
        [nf setRoundingMode:NSNumberFormatterRoundHalfDown];
    }

    if (roundIncrement >= 0)
        [nf setRoundingIncrement:[NSNumber numberWithDouble:roundIncrement]];
}

static void qt_setMacIntFrac(NSNumberFormatter *nf,
                             int maxIntDigits, int minIntDigits,
                             int maxFracDigits, int minFracDigits)
{
    [nf setUsesSignificantDigits:false];
    if (maxIntDigits >= 0)
        [nf setMaximumIntegerDigits:maxIntDigits];
    if (minIntDigits >= 0)
        [nf setMinimumIntegerDigits:minIntDigits];
    if (maxFracDigits >= 0)
        [nf setMaximumFractionDigits:maxFracDigits];
    if (minFracDigits >= 0)
        [nf setMinimumFractionDigits:minFracDigits];
}

static void qt_setMacSig(NSNumberFormatter *nf, int maxSigDigits, int minSigDigits)
{
    [nf setUsesSignificantDigits:true];
    if (maxSigDigits >= 0)
        [nf setMaximumSignificantDigits:maxSigDigits];
    if (minSigDigits >= 0)
        [nf setMinimumSignificantDigits:minSigDigits];
}

// Class methods

QNumberFormatterPrivate::QNumberFormatterPrivate(QNumberFormatter::NumberStyle style,
                                                 const QString &pattern,
                                                 const QString &localeCode)
{
    // Not all styles are supported by Mac
    switch (style) {
    case QNumberFormatter::OrdinalStyle:
    case QNumberFormatter::DurationStyle:
        m_style = QNumberFormatter::DecimalStyle;
        break;
    case QNumberFormatter::CurrencyCodeStyle:
    case QNumberFormatter::CurrencyNameStyle:
        m_style = QNumberFormatter::CurrencyStyle;
        break;
    default:
        m_style = style;
        break;
    }

    // TODO can we init with the style and locale by default?
    m_nf = [[[NSNumberFormatter alloc] init] autorelease];

    if (m_nf) {
        // Set the locale if not the default
        if (!localeCode.isEmpty()) {
            NSString *macLocaleCode = QCFString::toNSString(localeCode);
            NSLocale *macLocale = [[NSLocale alloc] initWithLocaleIdentifier:macLocaleCode];
            [m_nf setLocale:macLocale];
            [macLocaleCode release];
            // TODO Do we need to [macLocale release]?
        }
        // Set either to use the pattern or to use the style
        if (style == QNumberFormatter::DecimalPatternStyle
            || style == QNumberFormatter::RuleBasedStyle) {
            [m_nf setFormat:QCFString::toNSString(pattern)];
        } else {
            [m_nf setNumberStyle:qt_numberStyleToMacStyle(style)];
        }
    }
}

QNumberFormatterPrivate::QNumberFormatterPrivate(const QNumberFormatterPrivate &other)
{
    m_style = other.m_style;
    m_nf = [other.m_nf mutableCopy];
}

QNumberFormatterPrivate::~QNumberFormatterPrivate()
{
    [m_nf release];
}

QNumberFormatterPrivate *QNumberFormatterPrivate::clone()
{
    return new QNumberFormatterPrivate(*this);
}

bool QNumberFormatterPrivate::isValid() const
{
    return m_nf;
}

QString QNumberFormatterPrivate::localeCode(QLocale::LocaleCodeType type) const
{
    if (type == QLocale::EffectiveLocaleCode)
        return QCFString::toQString([[m_nf locale] localeIdentifier]);
    return m_localeCode;
}

QNumberFormatter::NumberStyle QNumberFormatterPrivate::numberStyle() const
{
    return m_style;
}

QList<QNumberFormatter::NumberStyle> QNumberFormatterPrivate::availableNumberStyles()
{
    QList<QNumberFormatter::NumberStyle> list;

    list << QNumberFormatter::DecimalStyle
         << QNumberFormatter::ScientificStyle
         << QNumberFormatter::CurrencyStyle
         << QNumberFormatter::PercentageStyle
         << QNumberFormatter::SpelloutStyle
         << QNumberFormatter::DecimalPatternStyle
         // TODO check rules are supported
         << QNumberFormatter::RuleBasedStyle;

    return list;
}

QString QNumberFormatterPrivate::currencyCode() const
{
    return QCFString::toQString([m_nf currencyCode]);
}

void QNumberFormatterPrivate::setCurrencyCode(const QString &value)
{
    [m_nf setCurrencyCode:QCFString::toNSString(value)];
}

QString QNumberFormatterPrivate::numberSymbol(QNumberFormatter::NumberSymbol symbol) const
{
    switch (symbol) {
    case QNumberFormatter::DecimalSeparatorSymbol:
        return QCFString::toQString([m_nf decimalSeparator]);
    case QNumberFormatter::GroupingSeparatorSymbol:
        return QCFString::toQString([m_nf groupingSeparator]);
    case QNumberFormatter::PercentSymbol:
        return QCFString::toQString([m_nf percentSymbol]);
    case QNumberFormatter::MinusSignSymbol:
        return QCFString::toQString([m_nf minusSign]);
    case QNumberFormatter::PlusSignSymbol:
        return QCFString::toQString([m_nf plusSign]);
    case QNumberFormatter::ExponentialSymbol:
        return QCFString::toQString([m_nf exponentSymbol]);
    case QNumberFormatter::PerMillSymbol:
        return QCFString::toQString([m_nf perMillSymbol]);
    case QNumberFormatter::InfinitySymbol:
        return QCFString::toQString([m_nf positiveInfinitySymbol]);
    case QNumberFormatter::NaNSymbol:
        return QCFString::toQString([m_nf notANumberSymbol]);
    case QNumberFormatter::CurrencySymbol:
        return QCFString::toQString([m_nf currencySymbol]);
    case QNumberFormatter::CurrencyInternationalSymbol:
        return QCFString::toQString([m_nf internationalCurrencySymbol]);
    case QNumberFormatter::MonetaryDecimalSeparatorSymbol:
        return QCFString::toQString([m_nf currencyDecimalSeparator]);
    case QNumberFormatter::MonetaryGroupingSeparatorSymbol:
        return QCFString::toQString([m_nf currencyGroupingSeparator]);
    case QNumberFormatter::ZeroDigitSymbol:
        return QCFString::toQString([m_nf zeroSymbol]);
    case QNumberFormatter::PatternSeparatorSymbol:
    case QNumberFormatter::PatternDigitSymbol:
    case QNumberFormatter::PatternSignificantDigitSymbol:
    case QNumberFormatter::PatternPadEscapeSymbol:
    case QNumberFormatter::PatternFormatSymbolCount:
    case QNumberFormatter::OneDigitSymbol:
    case QNumberFormatter::TwoDigitSymbol:
    case QNumberFormatter::ThreeDigitSymbol:
    case QNumberFormatter::FourDigitSymbol:
    case QNumberFormatter::FiveDigitSymbol:
    case QNumberFormatter::SixDigitSymbol:
    case QNumberFormatter::SevenDigitSymbol:
    case QNumberFormatter::EightDigitSymbol:
    case QNumberFormatter::NineDigitSymbol:
        return QString();
    }
}

void QNumberFormatterPrivate::setNumberSymbol(QNumberFormatter::NumberSymbol symbol, const QString &value)
{
    switch (symbol) {
    case QNumberFormatter::DecimalSeparatorSymbol:
        [m_nf setDecimalSeparator:QCFString::toNSString(value)];
    case QNumberFormatter::GroupingSeparatorSymbol:
        [m_nf setGroupingSeparator:QCFString::toNSString(value)];
    case QNumberFormatter::PercentSymbol:
        [m_nf setPercentSymbol:QCFString::toNSString(value)];
    case QNumberFormatter::MinusSignSymbol:
        [m_nf setMinusSign:QCFString::toNSString(value)];
    case QNumberFormatter::PlusSignSymbol:
        [m_nf setPlusSign:QCFString::toNSString(value)];
    case QNumberFormatter::ExponentialSymbol:
        [m_nf setExponentSymbol:QCFString::toNSString(value)];
    case QNumberFormatter::PerMillSymbol:
        [m_nf setPerMillSymbol:QCFString::toNSString(value)];
    case QNumberFormatter::InfinitySymbol:
        [m_nf setPositiveInfinitySymbol:QCFString::toNSString(value)];
    case QNumberFormatter::NaNSymbol:
        [m_nf setNotANumberSymbol:QCFString::toNSString(value)];
    case QNumberFormatter::CurrencySymbol:
        [m_nf setCurrencySymbol:QCFString::toNSString(value)];
    case QNumberFormatter::CurrencyInternationalSymbol:
        [m_nf setInternationalCurrencySymbol:QCFString::toNSString(value)];
    case QNumberFormatter::MonetaryDecimalSeparatorSymbol:
        [m_nf setCurrencyDecimalSeparator:QCFString::toNSString(value)];
    case QNumberFormatter::MonetaryGroupingSeparatorSymbol:
        [m_nf setCurrencyGroupingSeparator:QCFString::toNSString(value)];
    case QNumberFormatter::ZeroDigitSymbol:
        [m_nf setZeroSymbol:QCFString::toNSString(value)];
    case QNumberFormatter::PatternSeparatorSymbol:
    case QNumberFormatter::PatternDigitSymbol:
    case QNumberFormatter::PatternSignificantDigitSymbol:
    case QNumberFormatter::PatternPadEscapeSymbol:
    case QNumberFormatter::PatternFormatSymbolCount:
    case QNumberFormatter::OneDigitSymbol:
    case QNumberFormatter::TwoDigitSymbol:
    case QNumberFormatter::ThreeDigitSymbol:
    case QNumberFormatter::FourDigitSymbol:
    case QNumberFormatter::FiveDigitSymbol:
    case QNumberFormatter::SixDigitSymbol:
    case QNumberFormatter::SevenDigitSymbol:
    case QNumberFormatter::EightDigitSymbol:
    case QNumberFormatter::NineDigitSymbol:
        return;
    }
}

qint32 QNumberFormatterPrivate::numberAttribute(QNumberFormatter::NumberAttribute attribute) const
{
    switch (attribute) {
    case QNumberFormatter::MaximumIntegerDigits:
        return [m_nf maximumIntegerDigits];
    case QNumberFormatter::MinimumIntegerDigits:
        return [m_nf minimumIntegerDigits];
    case QNumberFormatter::MaximumFractionDigits:
        return [m_nf maximumFractionDigits];
    case QNumberFormatter::MinimumFractionDigits:
        return [m_nf minimumFractionDigits];
    case QNumberFormatter::MaximumSignificantDigits:
        return [m_nf maximumSignificantDigits];
    case QNumberFormatter::MinimumSignificantDigits:
        return [m_nf minimumSignificantDigits];
    case QNumberFormatter::Multiplier:
        return [[m_nf multiplier] intValue];
    case QNumberFormatter::PrimaryGroupingSize:
        return [m_nf groupingSize];
    case QNumberFormatter::SecondaryGroupingSize:
        return [m_nf secondaryGroupingSize];
    }
}

void QNumberFormatterPrivate::setNumberAttribute(QNumberFormatter::NumberAttribute attribute, qint32 value)
{
    switch (attribute) {
    case QNumberFormatter::MaximumIntegerDigits :
        [m_nf setMaximumIntegerDigits:value];
    case QNumberFormatter::MinimumIntegerDigits :
        [m_nf setMinimumIntegerDigits:value];
    case QNumberFormatter::MaximumFractionDigits :
        [m_nf setMaximumFractionDigits:value];
    case QNumberFormatter::MinimumFractionDigits :
        [m_nf setMinimumFractionDigits:value];
    case QNumberFormatter::MaximumSignificantDigits :
        [m_nf setMaximumSignificantDigits:value];
    case QNumberFormatter::MinimumSignificantDigits :
        [m_nf setMinimumSignificantDigits:value];
    case QNumberFormatter::Multiplier :
        [m_nf setMultiplier:[NSNumber numberWithInt:value]];
    case QNumberFormatter::PrimaryGroupingSize :
        [m_nf setGroupingSize:value];
    case QNumberFormatter::SecondaryGroupingSize :
        [m_nf setSecondaryGroupingSize:value];
    }
}

QString QNumberFormatterPrivate::pattern() const
{
    return QCFString::toQString([m_nf format]);
}

void QNumberFormatterPrivate::setPattern(const QString & pattern)
{
    [m_nf setFormat:QCFString::toNSString(pattern)];
}

bool QNumberFormatterPrivate::usesSignificantDigits() const
{
    return [m_nf usesSignificantDigits];
}

void QNumberFormatterPrivate::setUsesSignificantDigits(bool value)
{
    [m_nf setUsesSignificantDigits:value];
}

QNumberFormatter::RoundingMode QNumberFormatterPrivate::roundingMode() const
{
    switch ([m_nf roundingMode]) {
    case NSNumberFormatterRoundCeiling:
        return QNumberFormatter::RoundCeiling;
    case NSNumberFormatterRoundFloor:
        return QNumberFormatter::RoundFloor;
    case NSNumberFormatterRoundDown:
        return QNumberFormatter::RoundDown;
    case NSNumberFormatterRoundUp:
        return QNumberFormatter::RoundUp;
    case NSNumberFormatterRoundHalfEven:
        return QNumberFormatter::RoundHalfEven;
    case NSNumberFormatterRoundHalfDown:
        return QNumberFormatter::RoundHalfDown;
    case NSNumberFormatterRoundHalfUp:
        return QNumberFormatter::RoundHalfUp;
    }
}


double QNumberFormatterPrivate::roundingIncrement() const
{
    return [[m_nf roundingIncrement] doubleValue];
}

void QNumberFormatterPrivate::setRounding(QNumberFormatter::RoundingMode mode, double increment)
{
    qt_setMacRounding(m_nf, mode, increment);
}

qint32 QNumberFormatterPrivate::paddingWidth() const
{
    return [m_nf formatWidth];
}

QString QNumberFormatterPrivate::padding() const
{
    return QCFString::toQString([m_nf paddingCharacter]);
}

QNumberFormatter::PadPosition QNumberFormatterPrivate::paddingPosition() const
{
    switch ([m_nf paddingPosition]) {
    case NSNumberFormatterPadBeforePrefix:
        return QNumberFormatter::PadBeforePrefix;
    case NSNumberFormatterPadAfterPrefix:
        return QNumberFormatter::PadAfterPrefix;
    case NSNumberFormatterPadBeforeSuffix:
        return QNumberFormatter::PadBeforeSuffix;
    case NSNumberFormatterPadAfterSuffix:
        return QNumberFormatter::PadAfterSuffix;
    }
}

void QNumberFormatterPrivate::setPadding(qint32 width, const QString &padding,
                                         QNumberFormatter::PadPosition position)
{
    qt_setMacPadding(m_nf, width, padding, position);
}

QLocale::ParsingMode QNumberFormatterPrivate::parsingMode() const
{
    if ([m_nf isLenient])
        return QLocale::LenientParsing;
    else
        return QLocale::StrictParsing;
}

void QNumberFormatterPrivate::setParsingMode(QLocale::ParsingMode mode)
{
    if (mode == QLocale::LenientParsing)
        return [m_nf setLenient:true];
    else
        return [m_nf setLenient:false];
}

QString QNumberFormatterPrivate::toString(qint32 from) const
{
    return QCFString::toQString([m_nf stringFromNumber:[NSNumber numberWithInt:from]]);
}

QString QNumberFormatterPrivate::toString(qint64 from) const
{
    return QCFString::toQString([m_nf stringFromNumber:[NSNumber numberWithLongLong:from]]);
}

QString QNumberFormatterPrivate::toString(double from) const
{
    return QCFString::toQString([m_nf stringFromNumber:[NSNumber numberWithDouble:from]]);
}

QString QNumberFormatterPrivate::toString(qint32 from, int width, const QString &padding,
                                          QNumberFormatter::PadPosition position) const
{
    // Clone the nf so we don't change the shared object
    // TODO confirm is a clone!  Otherwise just create a new one (but slower?)
    NSNumberFormatter *nf = [m_nf mutableCopy];
    qt_setMacPadding(nf, width, padding, position);
    QString result = QCFString::toQString([nf stringFromNumber:[NSNumber numberWithInt:from]]);
    [nf release];
    return result;
}

QString QNumberFormatterPrivate::toString(qint64 from, int width, const QString &padding,
                                          QNumberFormatter::PadPosition position) const
{
    // Clone the nf so we don't change the shared object
    // TODO confirm is a clone!  Otherwise just create a new one (but slower?)
    NSNumberFormatter *nf = [m_nf mutableCopy];
    qt_setMacPadding(nf, width, padding, position);
    QString result = QCFString::toQString([nf stringFromNumber:[NSNumber numberWithLongLong:from]]);
    [nf release];
    return result;
}

QString QNumberFormatterPrivate::toString(double from, int width, const QString &padding,
                                          QNumberFormatter::PadPosition position) const
{
    // Clone the nf so we don't change the shared object
    // TODO confirm is a clone!  Otherwise just create a new one (but slower?)
    NSNumberFormatter *nf = [m_nf mutableCopy];
    qt_setMacPadding(nf, width, padding, position);
    QString result = QCFString::toQString([nf stringFromNumber:[NSNumber numberWithDouble:from]]);
    [nf release];
    return result;
}

QString QNumberFormatterPrivate::toString(double from,
                                          int maxIntDigits, int minIntDigits,
                                          int maxFracDigits, int minFracDigits,
                                          QNumberFormatter::RoundingMode roundMode,
                                          double roundIncrement) const
{
    // Clone the nf so we don't change the shared object
    // TODO confirm is a clone!  Otherwise just create a new one (but slower?)
    NSNumberFormatter *nf = [m_nf mutableCopy];
    qt_setMacIntFrac(nf, maxIntDigits, minIntDigits, maxFracDigits, minFracDigits);
    qt_setMacRounding(nf, roundMode, roundIncrement);
    QString result = QCFString::toQString([nf stringFromNumber:[NSNumber numberWithDouble:from]]);
    [nf release];
    return result;
}

QString QNumberFormatterPrivate::toString(double from,
                                          int maxSigDigits, int minSigDigits,
                                          QNumberFormatter::RoundingMode roundMode,
                                          double roundIncrement) const
{
    // Clone the nf so we don't change the shared object
    // TODO confirm is a clone!  Otherwise just create a new one (but slower?)
    NSNumberFormatter *nf = [m_nf mutableCopy];
    qt_setMacSig(nf, maxSigDigits, minSigDigits);
    qt_setMacRounding(nf, roundMode, roundIncrement);
    QString result = QCFString::toQString([nf stringFromNumber:[NSNumber numberWithDouble:from]]);
    [nf release];
    return result;
}

QString QNumberFormatterPrivate::toString(double from,
                                          int maxIntDigits, int minIntDigits,
                                          int maxFracDigits, int minFracDigits,
                                          int width, const QString &padding,
                                          QNumberFormatter::PadPosition position,
                                          QNumberFormatter::RoundingMode mode, double increment) const
{
    // Clone the nf so we don't change the shared object
    // TODO confirm is a clone!  Otherwise just create a new one (but slower?)
    NSNumberFormatter *nf = [m_nf mutableCopy];
    qt_setMacIntFrac(nf, maxIntDigits, minIntDigits, maxFracDigits, minFracDigits);
    qt_setMacPadding(nf, width, padding, position);
    qt_setMacRounding(nf, mode, increment);
    QString result = QCFString::toQString([nf stringFromNumber:[NSNumber numberWithDouble:from]]);
    [nf release];
    return result;
}

QString QNumberFormatterPrivate::toString(double from,
                                          int maxSigDigits, int minSigDigits,
                                          int width, const QString &padding,
                                          QNumberFormatter::PadPosition position,
                                          QNumberFormatter::RoundingMode mode, double increment) const
{
    // Clone the nf so we don't change the shared object
    // TODO confirm is a clone!  Otherwise just create a new one (but slower?)
    NSNumberFormatter *nf = [m_nf mutableCopy];
    qt_setMacSig(nf, maxSigDigits, minSigDigits);
    qt_setMacPadding(nf, width, padding, position);
    qt_setMacRounding(nf, mode, increment);
    QString result = QCFString::toQString([nf stringFromNumber:[NSNumber numberWithDouble:from]]);
    [nf release];
    return result;
}

qint32 QNumberFormatterPrivate::toInt32(const QString &from, bool *ok,
                                        QLocale::ParsingMode mode) const
{
    // Clone the nf so we don't change the shared object
    // TODO confirm is a clone!  Otherwise just create a new one (but slower?)
    NSNumberFormatter *nf = [m_nf mutableCopy];
    [nf setLenient:(mode == QLocale::LenientParsing)];
    NSNumber *num = [nf numberFromString:QCFString::fromCFString(from)];
    qint32 result = num->longValue();
    [nf release];
    [num release];
    return result;
}

qint64 QNumberFormatterPrivate::toInt64(const QString &from, bool *ok,
                                        QLocale::ParsingMode mode) const
{
    // Clone the nf so we don't change the shared object
    // TODO confirm is a clone!  Otherwise just create a new one (but slower?)
    NSNumberFormatter *nf = [m_nf mutableCopy];
    [nf setLenient:(mode == QLocale::LenientParsing)];
    NSNumber *num = [nf numberFromString:QCFString::fromCFString(from)];
    qint64 result = num->longLongValue();
    [num release];
    [nf release];
    return result;
}

double QNumberFormatterPrivate::toDouble(const QString &from, bool *ok,
                                         QLocale::ParsingMode mode) const
{
    // Clone the nf so we don't change the shared object
    // TODO confirm is a clone!  Otherwise just create a new one (but slower?)
    NSNumberFormatter *nf = [m_nf mutableCopy];
    [nf setLenient:(mode == QLocale::LenientParsing)];
    NSNumber *num = [nf numberFromString:QCFString::fromCFString(from)];
    double result = num->doubleValue();
    [num release];
    [nf release];
    return result;
}

QString QNumberFormatterPrivate::toAnyCurrency(double from, const QString &currencyCode,
                                               int width, const QString &padding,
                                               QNumberFormatter::PadPosition position,
                                               QNumberFormatter::RoundingMode mode,
                                               double increment) const
{
    QString result;
    return result;
}

double QNumberFormatterPrivate::fromAnyCurrency(const QString &from, QString *currencyCode, bool *ok,
                                                QLocale::ParsingMode mode) const
{
    double result = 0.0;
    return result;
}
