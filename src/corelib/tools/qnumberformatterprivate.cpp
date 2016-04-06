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
    Default C locale formatter
*/

QNumberFormatterPrivate::QNumberFormatterPrivate(QLocale::NumberStyle style, const QLocaleCode &locale)
    : m_numberStyle(style),
      m_currencyStyle(QLocale::CurrencyStyle(-1)),
      m_locale(locale)
{
}

QNumberFormatterPrivate::QNumberFormatterPrivate(QLocale::CurrencyStyle style, const QLocaleCode &locale)
    : m_numberStyle(QLocale::NumberStyle(-1)),
      m_currencyStyle(style),
      m_locale(locale)
{
}
QNumberFormatterPrivate::QNumberFormatterPrivate(QLocale::NumberStyle style, const QString positive, const QString negative, const QLocaleCode &locale)
    : m_numberStyle(style),
      m_currencyStyle(QLocale::CurrencyStyle(-1)),
      m_locale(locale)
{
    setPattern(positive, negative);
}

QNumberFormatterPrivate::QNumberFormatterPrivate(QLocale::CurrencyStyle style, const QString positive, const QString negative, const QLocaleCode &locale)
    : m_numberStyle(QLocale::NumberStyle(-1)),
      m_currencyStyle(style),
      m_locale(locale)
{
    setPattern(positive, negative);
}

QNumberFormatterPrivate::QNumberFormatterPrivate(const QNumberFormatterPrivate &other)
    : QSharedData(other),
      m_numberStyle(other.m_numberStyle),
      m_currencyStyle(other.m_currencyStyle),
      m_locale(other.m_locale)
{
}

QNumberFormatterPrivate::~QNumberFormatterPrivate()
{
}

QNumberFormatterPrivate *QNumberFormatterPrivate::clone()
{
    return new QNumberFormatterPrivate(*this);
}

bool QNumberFormatterPrivate::isValid() const
{
    return true;
}

QLocaleCode QNumberFormatterPrivate::locale() const
{
    return m_locale;
}

QLocale::NumberStyle QNumberFormatterPrivate::numberStyle() const
{
    return m_numberStyle;
}

QList<QLocale::NumberStyle> QNumberFormatterPrivate::availableNumberStyles()
{
    QList<QLocale::NumberStyle> list;
    list << QLocale::DecimalStyle;
    return list;
}

QLocale::CurrencyStyle QNumberFormatterPrivate::currencyStyle() const
{
    return m_currencyStyle;
}

QList<QLocale::CurrencyStyle> QNumberFormatterPrivate::availableCurrencyStyles()
{
    QList<QLocale::CurrencyStyle> list;
    list << QLocale::CurrencySymbolStyle;
    return list;
}

QString QNumberFormatterPrivate::positivePattern() const
{
    //TODO Get the right value
    return QString();
}

QString QNumberFormatterPrivate::negativePattern() const
{
    //TODO Get the right value
    return QString();
}

void QNumberFormatterPrivate::setPattern(const QString &positive, const QString &negative)
{
    Q_UNUSED(positive)
    Q_UNUSED(negative)
}

QString QNumberFormatterPrivate::currencyCode() const
{
    return QStringLiteral("USD");
}

void QNumberFormatterPrivate::setCurrencyCode(const QString & currencyCode)
{
    Q_UNUSED(currencyCode)
}

QString QNumberFormatterPrivate::currencySymbol(QLocale::CurrencySymbolType symbol) const
{
    switch (symbol) {
    case QLocale::CurrencyStandardSymbol :
        return QStringLiteral("$");
    case QLocale::CurrencyInternationalSymbol :
        return QStringLiteral("USD");
    }
    return QStringLiteral("$");
}

void QNumberFormatterPrivate::setCurrencySymbol(QLocale::CurrencySymbolType symbol, const QString &value)
{
    Q_UNUSED(symbol)
    Q_UNUSED(value)
}

QString QNumberFormatterPrivate::symbol(QLocale::NumberSymbol symbol) const
{
    //TODO Get the right values
    switch (symbol) {
    case QLocale::DecimalSeparatorSymbol :
        return QStringLiteral(".");
    case QLocale::GroupingSeparatorSymbol :
        return QStringLiteral(",");
    case QLocale::PercentSymbol :
        return QStringLiteral("%");
    case QLocale::MinusSignSymbol :
        return QStringLiteral("-");
    case QLocale::PlusSignSymbol :
        return QStringLiteral("+");
    case QLocale::ExponentialSymbol :
        return QStringLiteral("e");
    case QLocale::PerMillSymbol :
        return QStringLiteral("‰");
    case QLocale::InfinitySymbol :
        return QStringLiteral("∞");
    case QLocale::NaNSymbol :
        return QStringLiteral("NaN");
    case QLocale::PositivePrefix :
        return QStringLiteral("");
    case QLocale::PositiveSuffix :
        return QStringLiteral("");
    case QLocale::NegativePrefix :
        return QStringLiteral("");
    case QLocale::NegativeSuffix :
        return QStringLiteral("");
    case QLocale::ZeroDigitSymbol :
        return QStringLiteral("0");
    }
    return QString();
}

void QNumberFormatterPrivate::setSymbol(QLocale::NumberSymbol symbol, const QString &value)
{
    Q_UNUSED(symbol)
    Q_UNUSED(value)
}

qint32 QNumberFormatterPrivate::attribute(QLocale::NumberAttribute attribute) const
{
    //TODO Get the right values
    switch (attribute) {
    case QLocale::MaximumIntegerDigits :
        return 0;
    case QLocale::MinimumIntegerDigits :
        return 0;
    case QLocale::MaximumFractionDigits :
        return 0;
    case QLocale::MinimumFractionDigits :
        return 0;
    case QLocale::MaximumSignificantDigits :
        return 0;
    case QLocale::MinimumSignificantDigits :
        return 0;
    case QLocale::Multiplier :
        return 0;
    case QLocale::PrimaryGroupingSize :
        return 3;
    case QLocale::SecondaryGroupingSize :
        return 3;
    }
    return 0;
}

void QNumberFormatterPrivate::setAttribute(QLocale::NumberAttribute attribute, qint32 value)
{
    Q_UNUSED(attribute)
    Q_UNUSED(value)
}

bool QNumberFormatterPrivate::flag(QLocale::NumberFlag flag) const
{
    Q_UNUSED(flag)
    return false;
}

void QNumberFormatterPrivate::setFlag(QLocale::NumberFlag flag, bool value)
{
    Q_UNUSED(flag)
    Q_UNUSED(value)
}

QLocale::NumberFlags QNumberFormatterPrivate::flags() const
{
    return 0;
}

void QNumberFormatterPrivate::setFlags(QLocale::NumberFlags flags)
{
    Q_UNUSED(flags)
}

QLocale::DecimalFormatMode QNumberFormatterPrivate::decimalFormatMode() const
{
    return QLocale::IntegerFractionDigitsMode;
}

void QNumberFormatterPrivate::setDecimalFormatMode(QLocale::DecimalFormatMode mode)
{
    Q_UNUSED(mode)
}

void QNumberFormatterPrivate::setIntegerFractionDigits(qint32 maximumInteger, qint32 minimumInteger,
                                                       qint32 maximumFraction, qint32 minimumFraction)
{
    setAttribute(QLocale::MaximumIntegerDigits, maximumInteger);
    setAttribute(QLocale::MinimumIntegerDigits, minimumInteger);
    setAttribute(QLocale::MaximumFractionDigits, maximumFraction);
    setAttribute(QLocale::MinimumFractionDigits, minimumFraction);
}

void QNumberFormatterPrivate::setSignificantDigits(qint32 maximum, qint32 minimum)
{
    setAttribute(QLocale::MaximumSignificantDigits, maximum);
    setAttribute(QLocale::MinimumSignificantDigits, minimum);
}

QLocale::RoundingMode QNumberFormatterPrivate::roundingMode() const
{
    //TODO Get the right value
    return QLocale::RoundHalfDown;
}

double QNumberFormatterPrivate::roundingIncrement() const
{
    //TODO Get the right value
    return 0.1;
}

void QNumberFormatterPrivate::setRounding(QLocale::RoundingMode mode, double increment)
{
    Q_UNUSED(mode)
    Q_UNUSED(increment)
}

qint32 QNumberFormatterPrivate::paddingWidth() const
{
    //TODO Get the right value
    return 1;
}

QString QNumberFormatterPrivate::padding() const
{
    //TODO Get the right value
    return QStringLiteral(" ");
}

QLocale::PadPosition QNumberFormatterPrivate::paddingPosition() const
{
    //TODO Get the right value
    return QLocale::PadBeforePrefix;
}

void QNumberFormatterPrivate::setPadding(qint32 width, const QString &padding,
                                         QLocale::PadPosition position)
{
    Q_UNUSED(width)
    Q_UNUSED(padding)
    Q_UNUSED(position)
}

QString QNumberFormatterPrivate::toString(qint64 from) const
{
    //TODO Replace with old QLocale code
    return QString::number(from);
}

QString QNumberFormatterPrivate::toString(quint64 from) const
{
    //TODO Replace with old QLocale code
    return QString::number(from);
}

QString QNumberFormatterPrivate::toString(double from) const
{
    //TODO Replace with old QLocale code
    return QString::number(from);
}

qint64 QNumberFormatterPrivate::toInt64(const QString &from, bool *ok, qint32 *pos = 0) const
{
    //TODO Replace with old QLocale code
    Q_UNUSED(pos)
    return from.toLongLong(ok);
}

quint64 QNumberFormatterPrivate::toUInt64(const QString &from, bool *ok, qint32 *pos = 0) const
{
    //TODO Replace with old QLocale code
    Q_UNUSED(pos)
    return from.toULongLong(ok);
}

double QNumberFormatterPrivate::toDouble(const QString &from, bool *ok, qint32 *pos = 0) const
{
    //TODO Replace with old QLocale code
    Q_UNUSED(pos)
    return from.toDouble(ok);
}

QString QNumberFormatterPrivate::toCurrencyString(double from) const
{
    //TODO Replace with old QLocale code
    return QString::number(from);
}

double QNumberFormatterPrivate::toCurrencyDouble(const QString &from, bool *ok, qint32 *pos) const
{
    //TODO Replace with old QLocale code
    Q_UNUSED(pos)
    return from.toDouble(ok);
}

QString QNumberFormatterPrivate::toCurrencyString(double from, const QString &currencyCode) const
{
    //TODO Replace with old QLocale code
    Q_UNUSED(currencyCode)
    return QString::number(from);
}

double QNumberFormatterPrivate::toCurrencyDouble(const QString &from, QString *currencyCode, bool *ok, int *pos) const
{
    //TODO Replace with old QLocale code
    Q_UNUSED(currencyCode)
    Q_UNUSED(pos)
    return from.toDouble(ok);
}

// Define template for derived classes to reimplement so QSharedDataPointer clone() works correctly
template<> QNumberFormatterPrivate *QSharedDataPointer<QNumberFormatterPrivate>::clone()
{
    return d->clone();
}
