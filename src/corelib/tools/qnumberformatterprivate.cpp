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

QNumberFormatterPrivate::QNumberFormatterPrivate(QNumberFormatter::NumberStyle style,
                                                 const QString &pattern)
{
    init(QStringLiteral("C"), style, pattern);
}

QNumberFormatterPrivate::QNumberFormatterPrivate(const QString &locale,
                                                 QNumberFormatter::NumberStyle style,
                                                 const QString &pattern)
{
    init(QStringLiteral("C"), style, pattern);
}

void QNumberFormatterPrivate::init(const QString &locale,
                                   QNumberFormatter::NumberStyle style,
                                   const QString &pattern)
{
    Q_UNUSED(pattern)
    m_locale = locale;
    m_style = style;
}

QNumberFormatterPrivate::~QNumberFormatterPrivate()
{
}

bool QNumberFormatterPrivate::isValid() const
{
    return true;
}

QString QNumberFormatterPrivate::localeCode(QLocale::LocaleCodeType type) const
{
    Q_UNUSED(type)
    return QStringLiteral("C");
}

QNumberFormatter::NumberStyle QNumberFormatterPrivate::numberStyle() const
{
    return m_style;
}

QString QNumberFormatterPrivate::currencyCode() const
{
    return QString();
}

QString QNumberFormatterPrivate::numberSymbol(QNumberFormatter::NumberSymbol symbol) const
{
    switch (symbol) {
    case QNumberFormatter::DecimalSeparatorSymbol :
        return QStringLiteral(".");
    case QNumberFormatter::GroupingSeparatorSymbol :
        return QStringLiteral(",");
    case QNumberFormatter::PercentSymbol :
        return QStringLiteral("%");
    case QNumberFormatter::MinusSignSymbol :
        return QStringLiteral("-");
    case QNumberFormatter::PlusSignSymbol :
        return QStringLiteral("+");
    case QNumberFormatter::ExponentialSymbol :
        return QStringLiteral("e");
    case QNumberFormatter::PerMillSymbol :
        return QStringLiteral("‰");
    case QNumberFormatter::InfinitySymbol :
        return QStringLiteral("∞");
    case QNumberFormatter::NaNSymbol :
        return QStringLiteral("NaN");
    case QNumberFormatter::CurrencySymbol :
        return QStringLiteral("$");
    case QNumberFormatter::CurrencyInternationalSymbol :
        return QStringLiteral("USD");
    case QNumberFormatter::MonetaryDecimalSeparatorSymbol :
        return QStringLiteral(".");
    case QNumberFormatter::MonetaryGroupingSeparatorSymbol :
        return QStringLiteral(",");
    case QNumberFormatter::PatternSeparatorSymbol :
        return QStringLiteral(".");
    case QNumberFormatter::PatternDigitSymbol :
        return QStringLiteral(".");
    case QNumberFormatter::PatternSignificantDigitSymbol :
        return QStringLiteral(".");
    case QNumberFormatter::PatternPadEscapeSymbol :
        return QStringLiteral(".");
    case QNumberFormatter::PatternFormatSymbolCount :
        return QStringLiteral(".");
    case QNumberFormatter::ZeroDigitSymbol :
        return QStringLiteral("0");
    case QNumberFormatter::OneDigitSymbol :
        return QStringLiteral("1");
    case QNumberFormatter::TwoDigitSymbol :
        return QStringLiteral("2");
    case QNumberFormatter::ThreeDigitSymbol :
        return QStringLiteral("3");
    case QNumberFormatter::FourDigitSymbol :
        return QStringLiteral("4");
    case QNumberFormatter::FiveDigitSymbol :
        return QStringLiteral("5");
    case QNumberFormatter::SixDigitSymbol :
        return QStringLiteral("6");
    case QNumberFormatter::SevenDigitSymbol :
        return QStringLiteral("7");
    case QNumberFormatter::EightDigitSymbol :
        return QStringLiteral("8");
    case QNumberFormatter::NineDigitSymbol :
        return QStringLiteral("9");
    }
}

qint32 QNumberFormatterPrivate::numberAttribute(QNumberFormatter::NumberAttribute attribute) const
{
    //TODO Get the right values
    switch (attribute) {
    case QNumberFormatter::MaximumIntegerDigits :
        return 0;
    case QNumberFormatter::MinimumIntegerDigits :
        return 0;
    case QNumberFormatter::MaximumFractionDigits :
        return 0;
    case QNumberFormatter::MinimumFractionDigits :
        return 0;
    case QNumberFormatter::MaximumSignificantDigits :
        return 0;
    case QNumberFormatter::MinimumSignificantDigits :
        return 0;
    case QNumberFormatter::Multiplier :
        return 0;
    case QNumberFormatter::PrimaryGroupingSize :
        return 3;
    case QNumberFormatter::SecondaryGroupingSize :
        return 3;
    }
}

QString QNumberFormatterPrivate::pattern() const
{
    //TODO Get the right values
    return QString();
}

bool QNumberFormatterPrivate::usesSignificantDigits() const
{
    //TODO Get the right values
    return true;
}

QNumberFormatter::RoundingMode QNumberFormatterPrivate::roundingMode() const
{
    //TODO Get the right values
    return QNumberFormatter::RoundHalfDown;
}

double QNumberFormatterPrivate::roundingIncrement() const
{
    //TODO Get the right values
    return 0.1;
}

qint32 QNumberFormatterPrivate::paddingWidth() const
{
    //TODO Get the right values
    return 1;
}

QString QNumberFormatterPrivate::padding() const
{
    //TODO Get the right values
    return QStringLiteral(" ");
}

QNumberFormatter::PadPosition QNumberFormatterPrivate::paddingPosition() const
{
    //TODO Get the right values
    return QNumberFormatter::PadBeforePrefix;
}

void QNumberFormatterPrivate::setCurrencyCode(const QString &value)
{
    Q_UNUSED(value)
}

void QNumberFormatterPrivate::setNumberSymbol(QNumberFormatter::NumberSymbol symbol, const QString &value)
{
    Q_UNUSED(symbol)
    Q_UNUSED(value)
}

void QNumberFormatterPrivate::setNumberAttribute(QNumberFormatter::NumberAttribute attribute, qint32 value)
{
    Q_UNUSED(attribute)
    Q_UNUSED(value)
}

void QNumberFormatterPrivate::setPattern(const QString & pattern)
{
    Q_UNUSED(pattern)
}

void QNumberFormatterPrivate::setUsesSignificantDigits(bool value)
{
    Q_UNUSED(value)
}

void QNumberFormatterPrivate::setRounding(QNumberFormatter::RoundingMode mode, double increment)
{
    Q_UNUSED(mode)
    Q_UNUSED(increment)
}

void QNumberFormatterPrivate::setPadding(qint32 width, const QString &padding,
                                         QNumberFormatter::PadPosition position)
{
    Q_UNUSED(width)
    Q_UNUSED(padding)
    Q_UNUSED(position)
}

QLocale::ParsingMode QNumberFormatterPrivate::parsingMode() const
{
    return(QNumberFormatter::LenientParsing);
}

void QNumberFormatterPrivate::setParsingMode(QLocale::ParsingMode mode)
{
    Q_UNUSED(mode)
}

QString QNumberFormatterPrivate::toString(qint32 from) const
{
    //TODO Replace with old QLocale code
    return QString::number(from);
}

QString QNumberFormatterPrivate::toString(qint64 from) const
{
    //TODO Replace with old QLocale code
    return QString::number(from);
}

QString QNumberFormatterPrivate::toString(double from) const
{
    //TODO Replace with old QLocale code
    return QString::number(from);
}

QString QNumberFormatterPrivate::toString(qint32 from, int width, const QString &padding,
                                          QNumberFormatter::PadPosition position) const
{
    //TODO Replace with old QLocale code
    Q_UNUSED(width)
    Q_UNUSED(padding)
    Q_UNUSED(position)
    return QString::number(from);
}

QString QNumberFormatterPrivate::toString(qint64 from, int width, const QString &padding,
                                          QNumberFormatter::PadPosition position) const
{
    //TODO Replace with old QLocale code
    Q_UNUSED(width)
    Q_UNUSED(padding)
    Q_UNUSED(position)
    return QString::number(from);
}

QString QNumberFormatterPrivate::toString(double from, int width, const QString &padding,
                                          QNumberFormatter::PadPosition position) const
{
    //TODO Replace with old QLocale code
    Q_UNUSED(width)
    Q_UNUSED(padding)
    Q_UNUSED(position)
    return QString::number(from);
}

QString QNumberFormatterPrivate::toString(double from,
                                          int maxIntDigits, int minIntDigits,
                                          int maxFracDigits, int minFracDigits,
                                          QNumberFormatter::RoundingMode roundMode,
                                          double roundIncrement) const
{
    //TODO Replace with old QLocale code
    Q_UNUSED(maxIntDigits)
    Q_UNUSED(minIntDigits)
    Q_UNUSED(maxFracDigits)
    Q_UNUSED(minFracDigits)
    Q_UNUSED(roundMode)
    Q_UNUSED(roundIncrement)
    return QString::number(from);
}

QString QNumberFormatterPrivate::toString(double from,
                                          int maxSigDigits, int minSigDigits,
                                          QNumberFormatter::RoundingMode roundMode,
                                          double roundIncrement) const
{
    //TODO Replace with old QLocale code
    Q_UNUSED(maxIntDigits)
    Q_UNUSED(minIntDigits)
    Q_UNUSED(maxFracDigits)
    Q_UNUSED(minFracDigits)
    Q_UNUSED(roundMode)
    Q_UNUSED(roundIncrement)
    return QString::number(from);
}

QString QNumberFormatterPrivate::toString(double from,
                                          int maxIntDigits, int minIntDigits,
                                          int maxFracDigits, int minFracDigits,
                                          int width, const QString &padding,
                                          QNumberFormatter::PadPosition position,
                                          QNumberFormatter::RoundingMode mode, double increment) const
{
    //TODO Replace with old QLocale code
    Q_UNUSED(maxIntDigits)
    Q_UNUSED(minIntDigits)
    Q_UNUSED(maxFracDigits)
    Q_UNUSED(minFracDigits)
    Q_UNUSED(roundMode)
    Q_UNUSED(roundIncrement)
    return QString::number(from);
}

QString QNumberFormatterPrivate::toString(double from,
                                          int maxSigDigits, int minSigDigits,
                                          int width, const QString &padding,
                                          QNumberFormatter::PadPosition position,
                                          QNumberFormatter::RoundingMode mode, double increment) const
{
    //TODO Replace with old QLocale code
    Q_UNUSED(maxSigDigits)
    Q_UNUSED(minSigDigits)
    Q_UNUSED(width)
    Q_UNUSED(padding)
    Q_UNUSED(position)
    Q_UNUSED(mode)
    Q_UNUSED(increment)
    return QString::number(from);
}

qint32 QNumberFormatterPrivate::toInt32(const QString &from, bool *ok,
                                        QLocale::ParsingMode mode) const
{
    //TODO Replace with old QLocale code
    Q_UNUSED(mode)
    return from.toInt(ok);
}

qint64 QNumberFormatterPrivate::toInt64(const QString &from, bool *ok,
                                        QLocale::ParsingMode mode) const
{
    //TODO Replace with old QLocale code
    Q_UNUSED(mode)
    return from.toLongLong(ok);
}

double QNumberFormatterPrivate::toDouble(const QString &from, bool *ok,
                                         QLocale::ParsingMode mode) const
{
    //TODO Replace with old QLocale code
    Q_UNUSED(mode)
    return from.toDouble(ok);
}

QString QNumberFormatterPrivate::toAnyCurrency(double from, const QString &currencyCode,
                                               int width, const QString &padding,
                                               QNumberFormatter::PadPosition position,
                                               QNumberFormatter::RoundingMode mode,
                                               double increment) const
{
    //TODO Replace with old QLocale code
    Q_UNUSED(currencyCode)
    Q_UNUSED(width)
    Q_UNUSED(padding)
    Q_UNUSED(position)
    Q_UNUSED(mode)
    Q_UNUSED(increment)
    return QString::number(from);
}

double QNumberFormatterPrivate::fromAnyCurrency(const QString &from, QString *currencyCode, bool *ok,
                                                QLocale::ParsingMode mode) const
{
    //TODO Replace with old QLocale code
    Q_UNUSED(currencyCode)
    Q_UNUSED(mode)
    return from.toDouble(ok);
}
