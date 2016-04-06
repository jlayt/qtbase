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

#ifndef QCURRENCYFORMATTER_H
#define QCURRENCYFORMATTER_H

#include "QtCore/qlocale.h"
#include "QtCore/qlocalecode.h"

QT_BEGIN_NAMESPACE

class QNumberFormatterPrivate;

class Q_CORE_EXPORT QCurrencyFormatter
{
public:
    // Create default currency formatter in default locale
    QCurrencyFormatter();
    virtual ~QCurrencyFormatter();

    QCurrencyFormatter(const QCurrencyFormatter &other);
    QCurrencyFormatter &operator=(const QCurrencyFormatter &other);

    bool isValid() const;

    QLocaleCode locale() const;

    QLocale::CurrencyStyle style() const;

    QString currencyCode() const;
    QString currencySymbol(QLocale::CurrencySymbolType symbol) const;

    QString positivePattern() const;
    QString negativePattern() const;

    QString symbol(QLocale::NumberSymbol symbol) const;
    qint32 attribute(QLocale::NumberAttribute attribute) const;
    bool flag(QLocale::NumberFlag flag) const;
    QLocale::NumberFlags flags() const;

    QLocale::DecimalFormatMode decimalFormatMode() const;

    // Number Rounding settings
    QLocale::RoundingMode roundingMode() const;
    double roundingIncrement() const;

    // Number Padding settings
    qint32 paddingWidth() const;
    QString padding() const;
    QLocale::PadPosition paddingPosition() const;

    // Uses locale default settings
    QString toString(double from) const;
    double toDouble(const QString &from, bool *ok = Q_NULLPTR, int *pos = Q_NULLPTR) const;

    // Format/Parse any currency using this locale settings
    // Format will use the correct number of decimal places for currency code given
    QString toString(double from, const QString &currencyCode) const;
    double toDouble(const QString &from, QString *currencyCode, bool *ok = Q_NULLPTR, int *pos = Q_NULLPTR) const;

    static QList<QLocale::CurrencyStyle> availableStyles();

private:
    // Create locale with backend
    QCurrencyFormatter(QNumberFormatterPrivate &dd);
    friend class QNumberFormatterPrivate;
    friend class QCustomCurrencyFormatter;
    friend class QPatternCurrencyFormatter;
    QSharedDataPointer<QNumberFormatterPrivate> d;
};

class Q_CORE_EXPORT QCustomCurrencyFormatter : public QCurrencyFormatter
{
public:
    // Create named style in default or named locale
    QCustomCurrencyFormatter(QLocale::CurrencyStyle style, const QLocaleCode &localeCode = QLocaleCode());
    QCustomCurrencyFormatter(const QCustomCurrencyFormatter &other);
    virtual ~QCustomCurrencyFormatter();

    QCustomCurrencyFormatter &operator=(const QCustomCurrencyFormatter &other);

    void setCurrencyCode(const QString & currencyCode);
    void setCurrencySymbol(QLocale::CurrencySymbolType symbol, const QString &value);

    void setSymbol(QLocale::NumberSymbol symbol, const QString &value);
    void setAttribute(QLocale::NumberAttribute attribute, qint32 value);
    void setFlag(QLocale::NumberFlag flag, bool value = true);
    void setFlags(QLocale::NumberFlags flags);

    void setDecimalFormatMode(QLocale::DecimalFormatMode mode);
    void setIntegerFractionDigits(qint32 maximumInteger, qint32 minimumInteger,
                                  qint32 maximumFraction, qint32 minimumFraction);
    void setSignificantDigits(qint32 maximum, qint32 minimum);

    void setRounding(QLocale::RoundingMode mode, double increment = -1.0);
    void setPadding(qint32 width, const QString &padding, QLocale::PadPosition position = QLocale::DefaultPadPosition);

private:
    // Create locale with backend
    QCustomCurrencyFormatter(QNumberFormatterPrivate &dd);
};

class Q_CORE_EXPORT QPatternCurrencyFormatter : public QCurrencyFormatter
{
public:
    // Create pattern in default or named locale
    QPatternCurrencyFormatter(const QString &positive, const QLocaleCode &localeCode = QLocaleCode());
    QPatternCurrencyFormatter(const QString &positive, const QString & negative, const QLocaleCode &localeCode = QLocaleCode());
    QPatternCurrencyFormatter(const QPatternCurrencyFormatter &other);
    virtual ~QPatternCurrencyFormatter();

    QPatternCurrencyFormatter &operator=(const QPatternCurrencyFormatter &other);

    void setPattern(const QString &positive, const QString &negative = QString());
    void setFlag(QLocale::NumberFlag flag, bool value = true);
    void setFlags(QLocale::NumberFlags flags);
    void setRoundingMode(QLocale::RoundingMode mode);

private:
    // Create locale with backend
    QPatternCurrencyFormatter(QNumberFormatterPrivate &dd);
};

QT_END_NAMESPACE

#endif // QCURRENCYFORMATTER_H
