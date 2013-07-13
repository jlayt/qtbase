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
** Alternatively, this file may be used in accordance padding the terms and
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

QT_BEGIN_NAMESPACE

// Singleton class to hold global default Decimal Number Formatter for efficiency
class QNumberFormatterSingleton
{
public:
    QNumberFormatterSingleton() { backend = QNumberFormatterPrivate(QNumberFormatter::DecimalStyle,
                                                                    QString(), QString()); }

    QSharedDataPointer<QNumberFormatterPrivate> backend;
};

Q_GLOBAL_STATIC(QNumberFormatterSingleton, global_nf);

QNumberFormatter::QNumberFormatter()
                : d(global_nf.backend))
{
}

QNumberFormatter::QNumberFormatter(QNumberFormatter::NumberStyle style, const QString &localeCode)
                : d(QNumberFormatterPrivate(style, QString(), localeCode))
{
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

QString QNumberFormatter::localeCode(QLocale::LocaleCodeType type) const
{
    return d->localeCode(type);
}

QNumberFormatter::NumberStyle QNumberFormatter::numberStyle() const
{
    return d->numberStyle();
}

QString QNumberFormatter::currencyCode() const
{
    if (isValid())
        return d->currencyCode();
    else
        return QString();
}

QString QNumberFormatter::numberSymbol(NumberSymbol symbol) const
{
    if (isValid())
        return d->numberSymbol(symbol);
    else
        return QString();
}

qint32 QNumberFormatter::numberAttribute(NumberAttribute attribute) const
{
    if (isValid())
        return d->numberAttribute(attribute);
    else
        return -1;
}

bool QNumberFormatter::usesSignificantDigits() const
{
    if (isValid())
        return d->usesSignificantDigits();
    else
        return false;
}

QNumberFormatter::RoundingMode QNumberFormatter::roundingMode() const
{
    if (isValid())
        return d->roundingMode();
    else
        return DefaultRoundingMode;
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

QNumberFormatter::PadPosition QNumberFormatter::paddingPosition() const
{
    if (isValid())
        return d->paddingPosition();
    else
        return DefaultPadPosition;
}

QLocale::ParsingMode QNumberFormatter::parsingMode() const
{
    if (d->isValid())
        return d->parsingMode();
    else
        return QLocale::StrictParsing;
}

QString QNumberFormatter::toString(qint32 from) const
{
    if (isValid())
        return d->toString(from);
    else
        return QString();
}

QString QNumberFormatter::toString(qint64 from) const
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

QString QNumberFormatter::toString(qint32 from, int width, const QString &padding,
                                   QNumberFormatter::PadPosition position) const
{
    if (isValid())
        return d->toString(from, width, padding, position);
    else
        return QString();
}

QString QNumberFormatter::toString(qint64 from, int width, const QString &padding,
                                   QNumberFormatter::PadPosition position) const
{
    if (isValid())
        return d->toString(from, width, padding, position);
    else
        return QString();
}

QString QNumberFormatter::toString(double from, int width, const QString &padding,
                                   QNumberFormatter::PadPosition position) const
{
    if (isValid())
        return d->toString(from, width, padding, position);
    else
        return QString();
}

QString QNumberFormatter::toString(double from,
                                   int maxIntDigits, int minIntDigits,
                                   int maxFracDigits, int minFracDigits,
                                   QNumberFormatter::RoundingMode mode, double increment) const
{
    if (isValid())
        return d->toString(from,
                           maxIntDigits, minIntDigits,
                           maxFracDigits, minFracDigits,
                           mode, increment);
    else
        return QString();
}

QString QNumberFormatter::toString(double from,
                                   int maxSigDigits, int minSigDigits,
                                   QNumberFormatter::RoundingMode mode, double increment) const
{
    if (isValid())
        return d->toString(from,
                           maxSigDigits, minSigDigits,
                           mode, increment);
    else
        return QString();
}

QString QNumberFormatter::toString(double from,
                                   int maxIntDigits, int minIntDigits,
                                   int maxFracDigits, int minFracDigits,
                                   int width, const QString &padding, QNumberFormatter::PadPosition position,
                                   QNumberFormatter::RoundingMode mode, double increment) const
{
    if (isValid())
        return d->toString(from, 
                           maxIntDigits, minIntDigits, 
                           maxFracDigits, minFracDigits,
                           width, padding, position,
                           mode, increment);
    else
        return QString();
}

QString QNumberFormatter::toString(double from,
                                   int maxSigDigits, int minSigDigits,
                                   int width, const QString &padding, QNumberFormatter::PadPosition position,
                                   QNumberFormatter::RoundingMode mode, double increment) const
{
    if (isValid())
        return d->toString(from, maxSigDigits, minSigDigits, width, padding, position, mode, increment);
    else
        return QString();
}

qint32 QNumberFormatter::toInt32(const QString &from, bool *ok, QLocale::ParsingMode mode) const
{
    if (isValid()) {
        return d->toInt32(from, ok, mode);
    } else {
        if (ok)
            *ok = false;
        return 0;
    }
}

qint64 QNumberFormatter::toInt64(const QString &from, bool *ok, QLocale::ParsingMode mode) const
{
    if (isValid()) {
        return d->toInt64(from, ok, mode);
    } else {
        if (ok)
            *ok = false;
        return 0;
    }
}

double QNumberFormatter::toDouble(const QString &from, bool *ok, QLocale::ParsingMode mode) const
{
    if (isValid()) {
        return d->toDouble(from, ok, mode);
    } else {
        if (ok)
            *ok = false;
        return 0.0;
    }
}

QString QNumberFormatter::toAnyCurrency(double from, const QString &currencyCode,
                                        int width, const QString &padding, PadPosition position,
                                        RoundingMode mode, double increment) const
{
    if (isValid())
        return d->toAnyCurrency(from, currencyCode, width, padding, position, mode, increment);
    else
        return QString();
}

double QNumberFormatter::fromAnyCurrency(const QString &from, QString *currencyCode, bool *ok,
                                         QLocale::ParsingMode mode) const
{
    if (isValid() && currencyCode) {
        return d->fromAnyCurrency(from, currencyCode, ok, mode);
    } else {
        if (currencyCode)
            *currencyCode = QString();
        if (ok)
            *ok = false;
        return 0.0;
    }
}

QCustomNumberFormatter::QCustomNumberFormatter(QNumberFormatter::NumberStyle style,
                                               const QString &localeCode)
                      : QNumberFormatter(*new QNumberFormatterPrivate(style, localeCode))
{
}

QCustomNumberFormatter::QCustomNumberFormatter(const QString &locale,
                                               QNumberFormatter::NumberStyle style,
                                               const QString &pattern)
                      : QNumberFormatter(*new QNumberFormatterPrivate(style, pattern, localeCode))
{
}

QCustomNumberFormatter::QCustomNumberFormatter(const QCustomNumberFormatter &other)
{
    d = other.d;
}

QCustomNumberFormatter::~QCustomNumberFormatter()
{
}

QCustomNumberFormatter &QCustomNumberFormatter::operator=(const QCustomNumberFormatter &other)
{
    d = other.d;
    return *this;
}

void QCustomNumberFormatter::setCurrencyCode(const QString &currencyCode)
{
    d->setCurrencyCode(currencyCode);
}

QString QCustomNumberFormatter::pattern() const
{
    if (isValid())
        return d->pattern();
    else
        return QString();
}

void QCustomNumberFormatter::setPattern(const QString & pattern)
{
    if (isValid())
        d->setPattern(pattern);
}

void QCustomNumberFormatter::setNumberSymbol(QNumberFormatter::NumberSymbol symbol, const QString &value)
{
    if (isValid())
        d->setNumberSymbol(symbol, value);
}

void QCustomNumberFormatter::setNumberAttribute(QNumberFormatter::NumberAttribute attribute, qint32 value)
{
    if (isValid())
        d->setNumberAttribute(attribute, value);
}

void QCustomNumberFormatter::setUsesSignificantDigits(bool value)
{
    if (isValid())
        d->setUsesSignificantDigits(value);
}

void QCustomNumberFormatter::setRounding(QNumberFormatter::RoundingMode mode, double increment)
{
    if (isValid())
        d->setRounding(mode, increment);
}

void QCustomNumberFormatter::setPadding(qint32 width, const QString &padding,
                                        QNumberFormatter::PadPosition position)
{
    if (isValid())
        d->setPadding(width, padding, position);
}

void QCustomNumberFormatter::setParsingMode(QLocale::ParsingMode mode)
{
    if (d->isValid())
        d->setParsingMode(mode);
}
