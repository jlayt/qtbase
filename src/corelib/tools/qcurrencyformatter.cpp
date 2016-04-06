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

#include "qcurrencyformatter.h"
#include "qnumberformatterprivate_p.h"

QT_BEGIN_NAMESPACE

// Create Currency Formatter using appropriate backend
static QNumberFormatterPrivate *newBackendCurrencyFormatter(QLocale::CurrencyStyle style, const QLocaleCode &localeCode)
{
    return new QNumberFormatterPrivate(style, localeCode);
}

static QNumberFormatterPrivate *newBackendPatternCurrencyFormatter(const QString positive, const QString negative, const QLocaleCode &localeCode)
{
    return new QNumberFormatterPrivate(QLocale::CurrencySymbolStyle, positive, negative, localeCode);
}

// Singleton class to hold global default Currency Formatter for efficiency
class QCurrencyFormatterSingleton
{
public:
    QCurrencyFormatterSingleton() { backend = newBackendCurrencyFormatter(QLocale::CurrencySymbolStyle, QLocaleCode()); }

    QSharedDataPointer<QNumberFormatterPrivate> backend;
};

// App default Currency Formatter, able to be set by app to override global_system_cf
Q_GLOBAL_STATIC(QCurrencyFormatterSingleton, global_cf);

// System Currency Formatter, used for available styles, etc, never changed by app
Q_GLOBAL_STATIC(QCurrencyFormatterSingleton, global_system_cf);

static QNumberFormatterPrivate *defaultCurrencyFormatter()
{
    if (global_cf)
        return global_cf->backend;
    return global_system_cf->backend;
}


QCurrencyFormatter::QCurrencyFormatter()
{
    d = defaultCurrencyFormatter();
}

QCurrencyFormatter::QCurrencyFormatter(QNumberFormatterPrivate &dd)
                  : d(&dd)
{
}

QCurrencyFormatter::QCurrencyFormatter(const QCurrencyFormatter &other)
{
    d = other.d;
}

QCurrencyFormatter::~QCurrencyFormatter()
{
}

QCurrencyFormatter &QCurrencyFormatter::operator=(const QCurrencyFormatter &other)
{
    d = other.d;
    return *this;
}

bool QCurrencyFormatter::isValid() const
{
    return d->isValid();
}

QLocaleCode QCurrencyFormatter::locale() const
{
    return d->locale();
}

QLocale::CurrencyStyle QCurrencyFormatter::style() const
{
    return d->currencyStyle();
}

QString QCurrencyFormatter::currencyCode() const
{
    if (isValid())
        return d->currencyCode();
    else
        return QString();
}

QString QCurrencyFormatter::currencySymbol(QLocale::CurrencySymbolType symbol) const
{
    if (isValid())
        return d->currencySymbol(symbol);
    else
        return QString();
}

QString QCurrencyFormatter::positivePattern() const
{
    if (isValid())
        return d->positivePattern();
    else
        return QString();
}

QString QCurrencyFormatter::negativePattern() const
{
    if (isValid())
        return d->negativePattern();
    else
        return QString();
}

QString QCurrencyFormatter::symbol(QLocale::NumberSymbol symbol) const
{
    if (isValid())
        return d->symbol(symbol);
    else
        return QString();
}

qint32 QCurrencyFormatter::attribute(QLocale::NumberAttribute attribute) const
{
    if (isValid())
        return d->attribute(attribute);
    else
        return -1;
}

bool QCurrencyFormatter::flag(QLocale::NumberFlag flag) const
{
    if (isValid())
        return d->flag(flag);
    else
        return false;
}

QLocale::NumberFlags QCurrencyFormatter::flags() const
{
    if (isValid())
        return d->flags();
    else
        return 0;
}

QLocale::DecimalFormatMode QCurrencyFormatter::decimalFormatMode() const
{
    if (isValid())
        return d->decimalFormatMode();
    else
        return QLocale::IntegerFractionDigitsMode;
}

QLocale::RoundingMode QCurrencyFormatter::roundingMode() const
{
    if (isValid())
        return d->roundingMode();
    else
        return QLocale::DefaultRoundingMode;
}

double QCurrencyFormatter::roundingIncrement() const
{
    if (isValid())
        return d->roundingIncrement();
    else
        return -1.0;
}

qint32 QCurrencyFormatter::paddingWidth() const
{
    if (isValid())
        return d->paddingWidth();
    else
        return -1;
}

QString QCurrencyFormatter::padding() const
{
    if (isValid())
        return d->padding();
    else
        return QString();
}

QLocale::PadPosition QCurrencyFormatter::paddingPosition() const
{
    if (isValid())
        return d->paddingPosition();
    else
        return QLocale::DefaultPadPosition;
}

QString QCurrencyFormatter::toString(double from) const
{
    if (isValid())
        return d->toCurrencyString(from);
    else
        return QString();
}

double QCurrencyFormatter::toDouble(const QString &from, bool *ok, int *pos) const
{
    if (isValid()) {
        return d->toCurrencyDouble(from, ok, pos);
    } else {
        if (ok)
            *ok = false;
        if (pos)
            *pos = 0;
        return 0.0;
    }
}

QString QCurrencyFormatter::toString(double from, const QString &currencyCode) const
{
    if (isValid())
        return d->toCurrencyString(from, currencyCode);
    else
        return QString();
}

double QCurrencyFormatter::toDouble(const QString &from, QString *currencyCode, bool *ok, int *pos) const
{
    if (isValid() && currencyCode) {
        return d->toCurrencyDouble(from, currencyCode, ok, pos);
    } else {
        if (currencyCode)
            *currencyCode = QString();
        if (ok)
            *ok = false;
        return 0.0;
    }
}

QList<QLocale::CurrencyStyle> QCurrencyFormatter::availableStyles()
{
    return global_system_cf->backend->availableCurrencyStyles();
}


QCustomCurrencyFormatter::QCustomCurrencyFormatter(QLocale::CurrencyStyle style, const QLocaleCode &localeCode)
    : QCurrencyFormatter(*newBackendCurrencyFormatter(style, localeCode))
{
}

QCustomCurrencyFormatter::QCustomCurrencyFormatter(QNumberFormatterPrivate &dd)
{
    d = &dd;
}

QCustomCurrencyFormatter::QCustomCurrencyFormatter(const QCustomCurrencyFormatter &other)
    : QCurrencyFormatter(other)
{
}

QCustomCurrencyFormatter::~QCustomCurrencyFormatter()
{
}

QCustomCurrencyFormatter &QCustomCurrencyFormatter::operator=(const QCustomCurrencyFormatter &other)
{
    d = other.d;
    return *this;
}

void QCustomCurrencyFormatter::setCurrencyCode(const QString &currencyCode)
{
    if (isValid())
        d->setCurrencyCode(currencyCode);
}

void QCustomCurrencyFormatter::setCurrencySymbol(QLocale::CurrencySymbolType symbol, const QString &value)
{
    if (isValid())
        d->setCurrencySymbol(symbol, value);
}

void QCustomCurrencyFormatter::setSymbol(QLocale::NumberSymbol symbol, const QString &value)
{
    if (isValid())
        d->setSymbol(symbol, value);
}

void QCustomCurrencyFormatter::setAttribute(QLocale::NumberAttribute attribute, qint32 value)
{
    if (isValid())
        d->setAttribute(attribute, value);
}

void QCustomCurrencyFormatter::setFlag(QLocale::NumberFlag flag, bool value)
{
    if (isValid())
        d->setFlag(flag, value);
}

void QCustomCurrencyFormatter::setFlags(QLocale::NumberFlags flags)
{
    if (isValid())
        d->setFlags(flags);
}

void QCustomCurrencyFormatter::setDecimalFormatMode(QLocale::DecimalFormatMode mode)
{
    if (isValid())
        d->setDecimalFormatMode(mode);
}

void QCustomCurrencyFormatter::setIntegerFractionDigits(qint32 maximumInteger, qint32 minimumInteger,
                                                        qint32 maximumFraction, qint32 minimumFraction)
{
    if (isValid())
        d->setIntegerFractionDigits(maximumInteger, minimumInteger, maximumFraction, minimumFraction);
}

void QCustomCurrencyFormatter::setSignificantDigits(qint32 maximum, qint32 minimum)
{
    if (isValid())
        d->setSignificantDigits(maximum, minimum);
}

void QCustomCurrencyFormatter::setRounding(QLocale::RoundingMode mode, double increment)
{
    if (isValid())
        d->setRounding(mode, increment);
}

void QCustomCurrencyFormatter::setPadding(qint32 width, const QString &padding, QLocale::PadPosition position)
{
    if (isValid())
        d->setPadding(width, padding, position);
}


QPatternCurrencyFormatter::QPatternCurrencyFormatter(const QString &positive, const QLocaleCode &localeCode)
    : QCurrencyFormatter(*newBackendPatternCurrencyFormatter(positive, QString(), localeCode))
{
}

QPatternCurrencyFormatter::QPatternCurrencyFormatter(const QString &positive, const QString &negative, const QLocaleCode &localeCode)
    : QCurrencyFormatter(*newBackendPatternCurrencyFormatter(positive, negative, localeCode))
{
}

QPatternCurrencyFormatter::QPatternCurrencyFormatter(const QPatternCurrencyFormatter &other)
    : QCurrencyFormatter(other)
{
}

QPatternCurrencyFormatter::~QPatternCurrencyFormatter()
{
}

QPatternCurrencyFormatter &QPatternCurrencyFormatter::operator=(const QPatternCurrencyFormatter &other)
{
    d = other.d;
    return *this;
}

void QPatternCurrencyFormatter::setPattern(const QString & positive, const QString & negative)
{
    if (isValid())
        d->setPattern(negative, positive);
}

void QPatternCurrencyFormatter::setFlag(QLocale::NumberFlag flag, bool value)
{
    if (isValid())
        d->setFlag(flag, value);
}

void QPatternCurrencyFormatter::setFlags(QLocale::NumberFlags flags)
{
    if (isValid())
        d->setFlags(flags);
}

void QPatternCurrencyFormatter::setRoundingMode(QLocale::RoundingMode mode)
{
    if (isValid())
        d->setRounding(mode, -1.0);
}
