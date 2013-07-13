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

#ifdef Q_OS_MAC
#ifdef __OBJC__
@class NSNumberFormatter;
#else
class NSNumberFormatter;
#endif // __OBJC__
#endif // Q_OS_MAC

#ifdef QT_USE_ICU
#include "unicode/unum.h"
#endif // QT_USE_ICU

QT_BEGIN_NAMESPACE

class QNumberFormatterPrivate : public QSharedData
{
public:
    QNumberFormatterPrivate(QNumberFormatter::NumberStyle style, const QString &patternOrRules,
                            const QString &localeCode);
    QNumberFormatterPrivate(const QNumberFormatterPrivate &other);
    ~QNumberFormatterPrivate();

    virtual QNumberFormatterPrivate *clone();

    bool isValid() const;

    QString localeCode(QLocale::LocaleCodeType type = QLocale::RequestedLocaleCode) const;

    QNumberFormatter::NumberStyle numberStyle() const;
    static QList<QNumberFormatter::NumberStyle> availableNumberStyles();

    QString pattern() const;
    void setPattern(const QString & pattern);

    QString currencyCode() const;
    void setCurrencyCode(const QString & currencyCode);

    QString numberSymbol(QNumberFormatter::NumberSymbol symbol) const;
    void setNumberSymbol(QNumberFormatter::NumberSymbol symbol, const QString &value);

    qint32 numberAttribute(QNumberFormatter::NumberAttribute attribute) const;
    void setNumberAttribute(QNumberFormatter::NumberAttribute attribute, qint32 value);

    bool usesSignificantDigits() const;
    void setUsesSignificantDigits(bool value);

    QLocale::ParsingMode parsingMode() const;
    void setParsingMode(QLocale::ParsingMode mode);

    QNumberFormatter::RoundingMode roundingMode() const;
    double roundingIncrement() const;
    void setRounding(QNumberFormatter::RoundingMode mode, double increment);

    qint32 paddingWidth() const;
    QString padding() const;
    QNumberFormatter::PadPosition paddingPosition() const;
    void setPadding(qint32 width, const QString &padding,
                    QNumberFormatter::PadPosition position);

    QString toString(qint32 from) const;
    QString toString(qint64 from) const;
    QString toString(double from) const;

    QString toString(qint32 from, int width, const QString &padding,
                     QNumberFormatter::PadPosition position) const;
    QString toString(qint64 from, int width, const QString &padding,
                     QNumberFormatter::PadPosition position) const;
    QString toString(double from, int width, const QString &padding,
                     QNumberFormatter::PadPosition position) const;

    QString toString(double from,
                     int maxIntDigits, int minIntDigits, int maxFracDigits, int MinFracDigits,
                     QNumberFormatter::RoundingMode mode, double increment) const;

    QString toString(double from,
                     int maxSigDigits, int minSigDigits,
                     QNumberFormatter::RoundingMode mode, double increment) const;

    QString toString(double from,
                     int maxIntDigits, int minIntDigits, int maxFracDigits, int MinFracDigits,
                     int width, const QString &padding, QNumberFormatter::PadPosition position,
                     QNumberFormatter::RoundingMode mode, double increment) const;

    QString toString(double from,
                     int maxSigDigits, int minSigDigits,
                     int width, const QString &padding, QNumberFormatter::PadPosition position,
                     QNumberFormatter::RoundingMode mode, double increment) const;

    qint32 toInt32(const QString &from, bool *ok, QLocale::ParsingMode mode) const;
    qint64 toInt64(const QString &from, bool *ok, QLocale::ParsingMode mode) const;
    double toDouble(const QString &from, bool *ok, QLocale::ParsingMode mode) const;

    QString toAnyCurrency(double from, const QString &currencyCode,
                          int width, const QString &padding, QNumberFormatter::PadPosition position,
                          QNumberFormatter::RoundingMode mode, double increment) const;
    double fromAnyCurrency(const QString &from, QString *currencyCode, bool *ok,
                           QLocale::ParsingMode mode) const;

private:
    QNumberFormatter::NumberStyle m_style;
    QString m_localeCode;

#ifdef QT_USE_ICU
    UNumberFormat *m_nf;
    UErrorCode m_status;
#endif // QT_USE_ICU

#ifdef Q_OS_MAC
    NSNumberFormatter *m_nf;
#endif // Q_OS_MAC
};

template<> QNumberFormatterPrivate *QSharedDataPointer<QNumberFormatterPrivate>::clone();

QT_END_NAMESPACE

#endif // QNUMBERFORMATTERPRIVATE_P_H
