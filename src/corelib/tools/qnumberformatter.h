/****************************************************************************
**
** Copyright (C) 2013 John Layt jlayt@kde.org
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

#ifndef QNUMBERFORMATTER_H
#define QNUMBERFORMATTER_H

#include "qlocale.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QNumberFormatterPrivate;

class Q_CORE_EXPORT QNumberFormatter
{
public:
    // TODO Move these enums to QLocale?

    // Mac still to integrate:
    //   negativeFormat
    //   positiveFormat
    //   usesGroupingSeparator
    //   alwaysShowsDecimalSeparator
    //   allowsFloats
    //   minimum
    //   maximum
    //   isPartialStringValidationEnabled

    // Pre-defined number styles
    // ICU UNumberFormatStyle
    // Mac NSNumberFormatterStyle
    enum NumberStyle {
        // ICU and Mac
        DecimalStyle = 1,     // 10.00
        ScientificStyle,      // 1.0E2
        CurrencyStyle,        // $10.00
        PercentageStyle,      // 10%
        SpelloutStyle,        // ten
        DecimalPatternStyle,  // User defined pattern and symbols
        // TODO Check on Mac
        RuleBasedStyle,       // User defined rules
        // ICU Only
        OrdinalStyle,         // 10th
        DurationStyle,        // 00:00:10
        // ICU >= 4.8 Only
        CurrencyCodeStyle,    // USD10.00
        CurrencyNameStyle,    // 10 US dollars
        LastNumberStyle = CurrencyNameStyle
    };

    // String Symbols and attributes affecting how a number is formatted
    // Selected ICU UNumberFormatSymbol and UNumberTextAttribute
    // Mac not an enum, uses named method calls, also has nilSymbol and negativeInfinitySymbol

    enum NumberSymbol {
        // ICU and Mac
        DecimalSeparatorSymbol = 1,
        GroupingSeparatorSymbol,
        PercentSymbol,
        MinusSignSymbol,
        PlusSignSymbol,
        ExponentialSymbol,
        PerMillSymbol,
        InfinitySymbol,
        NaNSymbol,
        CurrencySymbol,
        CurrencyInternationalSymbol,
        MonetaryDecimalSeparatorSymbol,
        MonetaryGroupingSeparatorSymbol,
        // ICU only
        PatternSeparatorSymbol,
        PatternDigitSymbol,
        PatternSignificantDigitSymbol,
        PatternPadEscapeSymbol,
        PatternFormatSymbolCount,
        // ICU > 4.6 and Mac
        ZeroDigitSymbol,
        // ICU > 4.6 only
        OneDigitSymbol,
        TwoDigitSymbol,
        ThreeDigitSymbol,
        FourDigitSymbol,
        FiveDigitSymbol,
        SixDigitSymbol,
        SevenDigitSymbol,
        EightDigitSymbol,
        NineDigitSymbol
    };

    // Integer Attibutes affecting how a number is formatted
    // String, bool, enum, and double attributes are handled separately
    // ICU UNumberAttribute
    enum NumberAttribute {
        MaximumIntegerDigits = 1,
        MinimumIntegerDigits,
        MaximumFractionDigits,
        MinimumFractionDigits,
        MaximumSignificantDigits,
        MinimumSignificantDigits,
        Multiplier,
        PrimaryGroupingSize,
        SecondaryGroupingSize
    };

    // Rounding modes
    // ICU UNumberFormatRoundingMode
    // Mac NSNumberFormatterRoundingMode
    enum RoundingMode {
        DefaultRoundingMode = 0,
        RoundCeiling,
        RoundFloor,
        RoundDown,
        RoundUp,
        RoundHalfEven,
        RoundHalfDown,
        RoundHalfUp,
        // ICU only
        RoundUnnecessary
    };

    // ICU UNumberFormatPadPosition
    // Mac NSNumberFormatterPadPosition
    enum PadPosition {
        DefaultPadPosition = 0,
        PadBeforePrefix,
        PadAfterPrefix,
        PadBeforeSuffix,
        PadAfterSuffix
    };

    // Create default decimal formatter in default locale
    QNumberFormatter();
    // Create named style formatter using named locale
    // TODO change localeCode QString to QLocaleCode?
    QNumberFormatter(NumberStyle style, const QString &localeCode = QString());
    ~QNumberFormatter();

    QNumberFormatter(const QNumberFormatter &other);
    QNumberFormatter &operator=(const QNumberFormatter &other);

    static QNumberFormatter system(NumberStyle style = DecimalStyle);

    bool isValid() const;

    // TODO change localeCode QString to QLocaleCode?
    QString localeCode(QLocale::LocaleCodeType type = QLocale::RequestedLocaleCode) const;
    NumberStyle numberStyle() const;
    static QList<QNumberFormatter::NumberStyle> availableNumberStyles();
    QString currencyCode() const;

    QString numberSymbol(NumberSymbol symbol) const;
    qint32 numberAttribute(NumberAttribute attribute) const;
    bool usesSignificantDigits() const;

    // Number Rounding settings
    RoundingMode roundingMode() const;
    double roundingIncrement() const;

    // Number Padding settings
    qint32 paddingWidth() const;
    QString padding() const;
    PadPosition paddingPosition() const;

    QLocale::ParsingMode parsingMode() const;

    // Uses locale default settings
    QString toString(qint32 from) const;
    QString toString(qint64 from) const;
    QString toString(double from) const;

    // Override padding only
    QString toString(qint32 from, int width, const QString &padding = QString(),
                     PadPosition position = DefaultPadPosition) const;
    QString toString(qint64 from, int width, const QString &padding = QString(),
                     PadPosition position = DefaultPadPosition) const;
    QString toString(double from, int width, const QString &padding = QString(),
                     PadPosition position = DefaultPadPosition) const;

    // Override Int/Frac digits and optionally rounding mode
    QString toString(double from,
                     int maxIntDigits, int minIntDigits, int maxFracDigits, int MinFracDigits,
                     RoundingMode mode = DefaultRoundingMode,
                     double roundIncrement = -1.0) const;

    // Override significant digits and optionally rounding mode
    QString toString(double from,
                     int maxSigDigits, int minSigDigits,
                     RoundingMode mode = DefaultRoundingMode,
                     double roundIncrement = -1.0) const;

    // Override all Int/Frac options
    QString toString(double from,
                     int maxIntDigits, int minIntDigits, int maxFracDigits, int MinFracDigits,
                     int width = -1, const QString &padding = QString(),
                     PadPosition position = DefaultPadPosition,
                     RoundingMode mode = DefaultRoundingMode, double roundIncrement = -1.0) const;

    // Override all significant digits options
    QString toString(double from,
                     int maxSigDigits, int minSigDigits,
                     int width = -1, const QString &padding = QString(),
                     PadPosition position = DefaultPadPosition,
                     RoundingMode mode = DefaultRoundingMode, double roundIncrement = -1.0) const;

    // Uses locale default settings
    qint32 toInt32(const QString &from, bool *ok = 0,
                   QLocale::ParsingMode mode = QLocale::DefaultParsingMode) const;
    qint64 toInt64(const QString &from, bool *ok = 0,
                   QLocale::ParsingMode mode = QLocale::DefaultParsingMode) const;
    double toDouble(const QString &from, bool *ok = 0,
                    QLocale::ParsingMode mode = QLocale::DefaultParsingMode) const;

    // Format/Parse any currency using this locale settings, only if NumberFormat is Currency type
    // Format will use the correct number of decimal places for currency code given
    QString toAnyCurrency(double from, const QString &currencyCode,
                          int width = -1, const QString &padding = QString(),
                          PadPosition position = DefaultPadPosition,
                          RoundingMode mode = DefaultRoundingMode, double roundIncrement = -1.0) const;
    double fromAnyCurrency(const QString &from, QString *currencyCode, bool *ok = 0,
                           QLocale::ParsingMode mode = QLocale::DefaultParsingMode) const;

private:
    // Create locale with backend
    QNumberFormatter(QNumberFormatterPrivate &dd);
    friend class QNumberFormatterPrivate;
    friend class QCustomNumberFormatter;
    QSharedDataPointer<QNumberFormatterPrivate> d;
};

class Q_CORE_EXPORT QCustomNumberFormatter : public QNumberFormatter
{
public:
    // Create named style in named locale
    QCustomNumberFormatter(QNumberFormatter::NumberStyle style,
                           const QString &localeCode = QString());
    // Create named locale in named locale using custom pattern or rules
    QCustomNumberFormatter(QNumberFormatter::NumberStyle style, const QString &patternOrRules,
                           const QString &localeCode = QString());
    ~QCustomNumberFormatter();

    QCustomNumberFormatter(const QCustomNumberFormatter &other);
    QCustomNumberFormatter &operator=(const QCustomNumberFormatter &other);

    // Clone the other/system/default nf and return custom version
    QCustomNumberFormatter(const QNumberFormatter &other);
    QCustomNumberFormatter &operator=(const QNumberFormatter &other);
    static QCustomNumberFormatter system(NumberStyle style = DecimalStyle);

    void setCurrencyCode(const QString & currencyCode);

    QString pattern() const;
    void setPattern(const QString & pattern);

    void setNumberSymbol(QNumberFormatter::NumberSymbol symbol, const QString &value);
    void setNumberAttribute(QNumberFormatter::NumberAttribute attribute, qint32 value);
    void setUsesSignificantDigits(bool value);

    void setRounding(QNumberFormatter::RoundingMode mode, double increment = -1.0);
    void setPadding(qint32 width, const QString &padding,
                    QNumberFormatter::PadPosition position = DefaultPadPosition);

    void setParsingMode(QLocale::ParsingMode mode);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QNUMBERFORMATTER_H
