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

#ifndef QNUMBERFORMATTERPRIVATE_P_H
#define QNUMBERFORMATTERPRIVATE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of internal files.  This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.
//

#include "qnumberformatter.h"

#ifdef QT_USE_ICU
#include <unicode/unum.h>
#endif // QT_USE_ICU

QT_BEGIN_NAMESPACE

class Q_AUTOTEST_EXPORT QNumberFormatterPrivate : public QSharedData
{
public:
    QNumberFormatterPrivate(QLocale::NumberStyle style, const QLocaleCode &locale);
    QNumberFormatterPrivate(QLocale::CurrencyStyle style, const QLocaleCode &locale);
    QNumberFormatterPrivate(QLocale::NumberStyle style, const QString positive, const QString negative, const QLocaleCode &locale);
    QNumberFormatterPrivate(QLocale::CurrencyStyle style, const QString positive, const QString negative, const QLocaleCode &locale);
    QNumberFormatterPrivate(const QNumberFormatterPrivate &other);
    virtual ~QNumberFormatterPrivate();

    virtual QNumberFormatterPrivate *clone();

    virtual bool isValid() const;

    virtual QLocaleCode locale() const;

    virtual QLocale::NumberStyle numberStyle() const;
    virtual QList<QLocale::NumberStyle> availableNumberStyles();

    virtual QLocale::CurrencyStyle currencyStyle() const;
    virtual QList<QLocale::CurrencyStyle> availableCurrencyStyles();

    virtual QString positivePattern() const;
    virtual QString negativePattern() const;
    virtual void setPattern(const QString &positive, const QString &negative);

    virtual QString currencyCode() const;
    virtual void setCurrencyCode(const QString &currencyCode);

    virtual QString currencySymbol(QLocale::CurrencySymbolType symbol) const;
    virtual void setCurrencySymbol(QLocale::CurrencySymbolType symbol, const QString &value);

    virtual QString symbol(QLocale::NumberSymbol symbol) const;
    virtual void setSymbol(QLocale::NumberSymbol symbol, const QString &value);

    virtual qint32 attribute(QLocale::NumberAttribute attribute) const;
    virtual void setAttribute(QLocale::NumberAttribute attribute, qint32 value);

    virtual bool flag(QLocale::NumberFlag flag) const;
    virtual void setFlag(QLocale::NumberFlag flag, bool value = true);

    virtual QLocale::NumberFlags flags() const;
    virtual void setFlags(QLocale::NumberFlags flags);

    virtual QLocale::DecimalFormatMode decimalFormatMode() const;
    virtual void setDecimalFormatMode(QLocale::DecimalFormatMode mode);

    virtual void setIntegerFractionDigits(qint32 maximumInteger, qint32 minimumInteger,
                                          qint32 maximumFraction, qint32 minimumFraction);
    virtual void setSignificantDigits(qint32 maximum, qint32 minimum);

    virtual QLocale::RoundingMode roundingMode() const;
    virtual double roundingIncrement() const;
    virtual void setRounding(QLocale::RoundingMode mode, double increment);

    virtual qint32 paddingWidth() const;
    virtual QString padding() const;
    virtual QLocale::PadPosition paddingPosition() const;
    virtual void setPadding(qint32 width, const QString &padding, QLocale::PadPosition position);

    virtual QString toString(qint64 from) const;
    virtual QString toString(quint64 from) const;
    virtual QString toString(double from) const;

    virtual qint64 toInt64(const QString &from, bool *ok, qint32 *pos) const;
    virtual quint64 toUInt64(const QString &from, bool *ok, qint32 *pos) const;
    virtual double toDouble(const QString &from, bool *ok, qint32 *pos) const;

    virtual QString toCurrencyString(double from) const;
    virtual double toCurrencyDouble(const QString &from, bool *ok, int *pos) const;

    virtual QString toCurrencyString(double from, const QString &currencyCode) const;
    virtual double toCurrencyDouble(const QString &from, QString *currencyCode, bool *ok, int *pos) const;

protected:
    QLocale::NumberStyle m_numberStyle;
    QLocale::CurrencyStyle m_currencyStyle;
    QLocaleCode m_locale;
};

template<> QNumberFormatterPrivate *QSharedDataPointer<QNumberFormatterPrivate>::clone();

#ifdef QT_USE_ICU
class Q_AUTOTEST_EXPORT QIcuNumberFormatterPrivate Q_DECL_FINAL : public QNumberFormatterPrivate
{
public:
    QIcuNumberFormatterPrivate(QLocale::NumberStyle style, const QLocaleCode &locale);
    QIcuNumberFormatterPrivate(QLocale::CurrencyStyle style, const QLocaleCode &locale);
    QIcuNumberFormatterPrivate(const QIcuNumberFormatterPrivate &other);
    ~QIcuNumberFormatterPrivate();

    QIcuNumberFormatterPrivate *clone() Q_DECL_OVERRIDE;

    bool isValid() const Q_DECL_OVERRIDE;

    QList<QLocale::NumberStyle> availableNumberStyles() Q_DECL_OVERRIDE;
    QList<QLocale::CurrencyStyle> availableCurrencyStyles() Q_DECL_OVERRIDE;

    QString positivePattern() const Q_DECL_OVERRIDE;
    QString negativePattern() const Q_DECL_OVERRIDE;
    void setPattern(const QString &positive, const QString &negative) Q_DECL_OVERRIDE;

    QString currencyCode() const Q_DECL_OVERRIDE;
    void setCurrencyCode(const QString &currencyCode) Q_DECL_OVERRIDE;

    QString currencySymbol(QLocale::CurrencySymbolType symbol) const Q_DECL_OVERRIDE;
    void setCurrencySymbol(QLocale::CurrencySymbolType symbol, const QString &value) Q_DECL_OVERRIDE;

    QString symbol(QLocale::NumberSymbol symbol) const Q_DECL_OVERRIDE;
    void setSymbol(QLocale::NumberSymbol symbol, const QString &value) Q_DECL_OVERRIDE;

    qint32 attribute(QLocale::NumberAttribute attribute) const Q_DECL_OVERRIDE;
    void setAttribute(QLocale::NumberAttribute attribute, qint32 value) Q_DECL_OVERRIDE;

    bool flag(QLocale::NumberFlag flag) const Q_DECL_OVERRIDE;
    void setFlag(QLocale::NumberFlag flag, bool value = true) Q_DECL_OVERRIDE;

    QLocale::NumberFlags flags() const Q_DECL_OVERRIDE;
    void setFlags(QLocale::NumberFlags flags) Q_DECL_OVERRIDE;

    QLocale::DecimalFormatMode decimalFormatMode() const Q_DECL_OVERRIDE;
    void setDecimalFormatMode(QLocale::DecimalFormatMode mode) Q_DECL_OVERRIDE;

    QLocale::RoundingMode roundingMode() const Q_DECL_OVERRIDE;
    double roundingIncrement() const Q_DECL_OVERRIDE;
    void setRounding(QLocale::RoundingMode mode, double increment) Q_DECL_OVERRIDE;

    qint32 paddingWidth() const Q_DECL_OVERRIDE;
    QString padding() const Q_DECL_OVERRIDE;
    QLocale::PadPosition paddingPosition() const Q_DECL_OVERRIDE;
    void setPadding(qint32 width, const QString &padding, QLocale::PadPosition position) Q_DECL_OVERRIDE;

    QString toString(qint64 from) const Q_DECL_OVERRIDE;
    QString toString(quint64 from) const Q_DECL_OVERRIDE;
    QString toString(double from) const Q_DECL_OVERRIDE;

    qint64 toInt64(const QString &from, bool *ok, qint32 *pos) const Q_DECL_OVERRIDE;
    quint64 toUInt64(const QString &from, bool *ok, qint32 *pos) const Q_DECL_OVERRIDE;
    double toDouble(const QString &from, bool *ok, qint32 *pos) const Q_DECL_OVERRIDE;

    QString toCurrencyString(double from) const Q_DECL_OVERRIDE;
    double toCurrencyDouble(const QString &from, bool *ok, int *pos) const Q_DECL_OVERRIDE;

    QString toCurrencyString(double from, const QString &currencyCode) const Q_DECL_OVERRIDE;
    double toCurrencyDouble(const QString &from, QString *currencyCode, bool *ok, int *pos) const Q_DECL_OVERRIDE;

private:
    UNumberFormat *m_unum;
    UErrorCode m_status;
};
#endif // QT_USE_ICU

QT_END_NAMESPACE

#endif // QNUMBERFORMATTERPRIVATE_P_H
