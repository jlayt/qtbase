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

QT_BEGIN_NAMESPACE

// Create Number Formatter using appropriate platform backend
static QNumberFormatterPrivate *newBackendNumberFormatter(QLocale::NumberStyle style, const QLocaleCode &localeCode)
{
    return new QNumberFormatterPrivate(style, localeCode);
}

static QNumberFormatterPrivate *newBackendPatternNumberFormatter(const QString positive, const QString negative, const QLocaleCode &localeCode)
{
    return new QNumberFormatterPrivate(QLocale::DecimalStyle, positive, negative, localeCode);
}

// Singleton class to hold global default Number Formatter for efficiency
class QNumberFormatterSingleton
{
public:
    QNumberFormatterSingleton() { backend = newBackendNumberFormatter(QLocale::DecimalStyle, QLocaleCode()); }

    QSharedDataPointer<QNumberFormatterPrivate> backend;
};

// App default Number Formatter, able to be set by app to override global_system_cf
Q_GLOBAL_STATIC(QNumberFormatterSingleton, global_nf);

// System Number Formatter, used for available styles, etc, never changed by app
Q_GLOBAL_STATIC(QNumberFormatterSingleton, global_system_nf);

static QNumberFormatterPrivate *defaultNumberFormatter()
{
    if (global_nf)
        return global_nf->backend;
    return global_system_nf->backend;
}


QNumberFormatter::QNumberFormatter()
{
    d = defaultNumberFormatter();
}

QNumberFormatter::QNumberFormatter(QNumberFormatterPrivate &dd)
    : d(&dd)
{
}

QNumberFormatter::QNumberFormatter(const QNumberFormatter &other)
{
    d = other.d;
}

QNumberFormatter::~QNumberFormatter()
{
}

QNumberFormatter &QNumberFormatter::operator=(const QNumberFormatter &other)
{
    d = other.d;
    return *this;
}

bool QNumberFormatter::isValid() const
{
    return d->isValid();
}

QLocaleCode QNumberFormatter::locale() const
{
    return d->locale();
}

QLocale::NumberStyle QNumberFormatter::style() const
{
    return d->numberStyle();
}

QString QNumberFormatter::positivePattern() const
{
    if (isValid())
        return d->positivePattern();
    else
        return QString();
}

QString QNumberFormatter::negativePattern() const
{
    if (isValid())
        return d->negativePattern();
    else
        return QString();
}

QString QNumberFormatter::symbol(QLocale::NumberSymbol symbol) const
{
    if (isValid())
        return d->symbol(symbol);
    else
        return QString();
}

qint32 QNumberFormatter::attribute(QLocale::NumberAttribute attribute) const
{
    if (isValid())
        return d->attribute(attribute);
    else
        return -1;
}

bool QNumberFormatter::flag(QLocale::NumberFlag flag) const
{
    if (isValid())
        return d->flag(flag);
    else
        return false;
}

QLocale::NumberFlags QNumberFormatter::flags() const
{
    if (isValid())
        return d->flags();
    else
        return 0;
}

QLocale::DecimalFormatMode QNumberFormatter::decimalFormatMode() const
{
    if (isValid())
        return d->decimalFormatMode();
    else
        return QLocale::IntegerFractionDigitsMode;
}

QLocale::RoundingMode QNumberFormatter::roundingMode() const
{
    if (isValid())
        return d->roundingMode();
    else
        return QLocale::DefaultRoundingMode;
}

double QNumberFormatter::roundingIncrement() const
{
    if (isValid())
        return d->roundingIncrement();
    else
        return -1.0;
}

qint32 QNumberFormatter::paddingWidth() const
{
    if (isValid())
        return d->paddingWidth();
    else
        return -1;
}

QString QNumberFormatter::padding() const
{
    if (isValid())
        return d->padding();
    else
        return QString();
}

QLocale::PadPosition QNumberFormatter::paddingPosition() const
{
    if (isValid())
        return d->paddingPosition();
    else
        return QLocale::DefaultPadPosition;
}

QString QNumberFormatter::toString(qint64 from) const
{
    if (isValid())
        return d->toString(from);
    else
        return QString();
}

QString QNumberFormatter::toString(quint64 from) const
{
    if (isValid())
        return d->toString(from);
    else
        return QString();
}


QString QNumberFormatter::toString(double from) const
{
    if (isValid())
        return d->toString(from);
    else
        return QString();
}

qint64 QNumberFormatter::toInt64(const QString &from, bool *ok, qint32 *pos) const
{
    if (isValid()) {
        return d->toInt64(from, ok, pos);
    } else {
        if (ok)
            *ok = false;
        if (pos)
            *pos = 0;
        return 0;
    }
}

quint64 QNumberFormatter::toUInt64(const QString &from, bool *ok, qint32 *pos) const
{
    if (isValid()) {
        return d->toUInt64(from, ok, pos);
    } else {
        if (ok)
            *ok = false;
        if (pos)
            *pos = 0;
        return 0;
    }
}

double QNumberFormatter::toDouble(const QString &from, bool *ok, qint32 *pos) const
{
    if (isValid()) {
        return d->toDouble(from, ok, pos);
    } else {
        if (ok)
            *ok = false;
        if (pos)
            *pos = 0;
        return 0.0;
    }
}

QList<QLocale::NumberStyle> availableStyles()
{
    return global_system_nf->backend->availableNumberStyles();
}


QCustomNumberFormatter::QCustomNumberFormatter(QLocale::NumberStyle style, const QLocaleCode &localeCode)
    : QNumberFormatter(*newBackendNumberFormatter(style, localeCode))
{
}

QCustomNumberFormatter::QCustomNumberFormatter(QNumberFormatterPrivate &dd)
{
    d = &dd;
}

QCustomNumberFormatter::QCustomNumberFormatter(const QCustomNumberFormatter &other)
    : QNumberFormatter(other)
{
}

QCustomNumberFormatter::~QCustomNumberFormatter()
{
}

QCustomNumberFormatter &QCustomNumberFormatter::operator=(const QCustomNumberFormatter &other)
{
    d = other.d;
    return *this;
}

void QCustomNumberFormatter::setSymbol(QLocale::NumberSymbol symbol, const QString &value)
{
    if (isValid())
        d->setSymbol(symbol, value);
}

void QCustomNumberFormatter::setAttribute(QLocale::NumberAttribute attribute, qint32 value)
{
    if (isValid())
        d->setAttribute(attribute, value);
}

void QCustomNumberFormatter::setFlag(QLocale::NumberFlag flag, bool value)
{
    if (isValid())
        d->setFlag(flag, value);
}

void QCustomNumberFormatter::setFlags(QLocale::NumberFlags flags)
{
    if (isValid())
        d->setFlags(flags);
}

void QCustomNumberFormatter::setDecimalFormatMode(QLocale::DecimalFormatMode mode)
{
    if (isValid())
        d->setDecimalFormatMode(mode);
}

void QCustomNumberFormatter::setIntegerFractionDigits(qint32 maximumInteger, qint32 minimumInteger,
                                                      qint32 maximumFraction, qint32 minimumFraction)
{
    if (isValid())
        d->setIntegerFractionDigits(maximumInteger, minimumInteger, maximumFraction, minimumFraction);
}

void QCustomNumberFormatter::setSignificantDigits(qint32 maximum, qint32 minimum)
{
    if (isValid())
        d->setSignificantDigits(maximum, minimum);
}

void QCustomNumberFormatter::setRounding(QLocale::RoundingMode mode, double increment)
{
    if (isValid())
        d->setRounding(mode, increment);
}

void QCustomNumberFormatter::setPadding(qint32 width, const QString &padding, QLocale::PadPosition position)
{
    if (isValid())
        d->setPadding(width, padding, position);
}


QPatternNumberFormatter::QPatternNumberFormatter(const QString &positive, const QLocaleCode &localeCode)
    : QNumberFormatter(*newBackendPatternNumberFormatter(positive, QString(), localeCode))
{
}

QPatternNumberFormatter::QPatternNumberFormatter(const QString &positive, const QString &negative, const QLocaleCode &localeCode)
    : QNumberFormatter(*newBackendPatternNumberFormatter(positive, negative, localeCode))
{
}

QPatternNumberFormatter::QPatternNumberFormatter(const QPatternNumberFormatter &other)
    : QNumberFormatter(other)
{
}

QPatternNumberFormatter::~QPatternNumberFormatter()
{
}

QPatternNumberFormatter &QPatternNumberFormatter::operator=(const QPatternNumberFormatter &other)
{
    d = other.d;
    return *this;
}

void QPatternNumberFormatter::setPattern(const QString & positive, const QString & negative)
{
    if (isValid())
        d->setPattern(negative, positive);
}

void QPatternNumberFormatter::setFlag(QLocale::NumberFlag flag, bool value)
{
    if (isValid())
        d->setFlag(flag, value);
}

void QPatternNumberFormatter::setFlags(QLocale::NumberFlags flags)
{
    if (isValid())
        d->setFlags(flags);
}

void QPatternNumberFormatter::setRoundingMode(QLocale::RoundingMode mode)
{
    if (isValid())
        d->setRounding(mode, -1.0);
}
