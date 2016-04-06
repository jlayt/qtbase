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

#ifndef QNUMBERFORMATTER_H
#define QNUMBERFORMATTER_H

#include "QtCore/qlocale.h"
#include "QtCore/qlocalecode.h"

QT_BEGIN_NAMESPACE

class QNumberFormatterPrivate;

class Q_CORE_EXPORT QNumberFormatter
{
public:
    // Create default decimal formatter in default locale
    QNumberFormatter();
    virtual ~QNumberFormatter();

    QNumberFormatter(const QNumberFormatter &other);
    QNumberFormatter &operator=(const QNumberFormatter &other);

    bool isValid() const;

    QLocaleCode locale() const;

    QLocale::NumberStyle style() const;

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

    // Format as strings
    QString toString(qint64 from) const;
    QString toString(quint64 from) const;
    QString toString(double from) const;

    // Parse from strings
    qint64 toInt64(const QString &from, bool *ok = Q_NULLPTR, qint32 *pos = Q_NULLPTR) const;
    quint64 toUInt64(const QString &from, bool *ok = Q_NULLPTR, qint32 *pos = Q_NULLPTR) const;
    double toDouble(const QString &from, bool *ok = Q_NULLPTR, qint32 *pos = Q_NULLPTR) const;

    // Available custom styles on this platform
    static QList<QLocale::NumberStyle> availableStyles();

private:
    // Create locale with backend
    QNumberFormatter(QNumberFormatterPrivate &dd);
    friend class QNumberFormatterPrivate;
    friend class QCustomNumberFormatter;
    friend class QPatternNumberFormatter;
    QSharedDataPointer<QNumberFormatterPrivate> d;
};

class Q_CORE_EXPORT QCustomNumberFormatter : public QNumberFormatter
{
public:
    // Create named style in default or named locale
    QCustomNumberFormatter(QLocale::NumberStyle style, const QLocaleCode &localeCode = QLocaleCode());
    QCustomNumberFormatter(const QCustomNumberFormatter &other);
    virtual ~QCustomNumberFormatter();

    QCustomNumberFormatter &operator=(const QCustomNumberFormatter &other);

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
    QCustomNumberFormatter(QNumberFormatterPrivate &dd);
};

class Q_CORE_EXPORT QPatternNumberFormatter : public QNumberFormatter
{
public:
    // Create pattern in default or named locale
    QPatternNumberFormatter(const QString &positive, const QLocaleCode &localeCode = QLocaleCode());
    QPatternNumberFormatter(const QString &positive, const QString & negative, const QLocaleCode &localeCode = QLocaleCode());
    QPatternNumberFormatter(const QPatternNumberFormatter &other);
    virtual ~QPatternNumberFormatter();

    QPatternNumberFormatter &operator=(const QPatternNumberFormatter &other);

    void setPattern(const QString &positive, const QString &negative = QString());
    void setFlag(QLocale::NumberFlag flag, bool value = true);
    void setFlags(QLocale::NumberFlags flags);
    void setRoundingMode(QLocale::RoundingMode mode);

private:
    // Create locale with backend
    QPatternNumberFormatter(QNumberFormatterPrivate &dd);
};

QT_END_NAMESPACE

#endif // QNUMBERFORMATTER_H
