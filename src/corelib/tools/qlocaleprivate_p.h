/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QLOCALEPRIVATE_P_H
#define QLOCALEPRIVATE_P_H

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

#include "QtCore/qstring.h"
#include "QtCore/qvarlengtharray.h"
#include "QtCore/qvariant.h"

#include "qlocale.h"

QT_BEGIN_NAMESPACE

class Q_CORE_EXPORT QLocalePrivate
{
public:
    QLocale();
    QLocale(const QString &name);
    QLocale(Language language, Country country);
    QLocale(Language language, Script script, Country country);
    QLocale(const QLocale &other);
    ~QLocale();

    QLocale &operator=(const QLocale &other);

    bool operator==(const QLocale &other) const;
    bool operator!=(const QLocale &other) const;

    static QLocale system();
    static void setDefault(const QLocale &locale);

    // Locale functions

    QString name() const;
    QString bcp47Name() const;

    Language language() const;
    Script script() const;
    Country country() const;

    QString nativeLanguageName() const;
    QString nativeCountryName() const;

    static QString languageToString(Language language);
    static QString countryToString(Country country);
    static QString scriptToString(Script script);

    static QList<QLocale> matchingLocales(QLocale::Language language, QLocale::Script script, QLocale::Country country);
    static QList<Country> countriesForLanguage(Language lang);

    QStringList uiLanguages() const;

    // Number functions

    QChar decimalPoint() const;
    QChar groupSeparator() const;
    QChar percent() const;
    QChar zeroDigit() const;
    QChar negativeSign() const;
    QChar positiveSign() const;
    QChar exponential() const;

    void setNumberOptions(NumberOptions options);
    NumberOptions numberOptions() const;

    short toShort(const QStringRef &s, bool *ok) const;
    ushort toUShort(const QStringRef &s, bool *ok) const;
    int toInt(const QStringRef &s, bool *ok) const;
    uint toUInt(const QStringRef &s, bool *ok) const;
    qint64 toInt64(const QStringRef &s, bool *ok) const;
    quint64 toUInt64(const QStringRef &s, bool *ok) const;
    float toFloat(const QStringRef &s, bool *ok) const;
    double toDouble(const QStringRef &s, bool *ok) const;

    QString toString(qint64 i) const;
    QString toString(quint64 i) const;
    QString toString(double i, char f = 'g', int prec = 6) const;

    // Date/Time functions

    QString dateFormat(FormatType format) const;
    QString timeFormat(FormatType format) const;
    QString dateTimeFormat(FormatType format) const;

    QString monthName(int, FormatType format) const;
    QString standaloneMonthName(int, FormatType format) const;
    QString dayName(int, FormatType format) const;
    QString standaloneDayName(int, FormatType format) const;

    Qt::DayOfWeek firstDayOfWeek() const;
    QList<Qt::DayOfWeek> weekdays() const;

    QString amText() const;
    QString pmText() const;

    QString toString(const QDate &date, FormatType format) const;
    QString toString(const QTime &time, FormatType format) const;
    QString toString(const QDateTime &dateTime, FormatType format) const;

    QString toString(const QDate &date, const QString &formatStr) const;
    QString toString(const QTime &time, const QString &formatStr) const;
    QString toString(const QDateTime &dateTime, const QString &format) const;

#ifndef QT_NO_DATESTRING
    QDate toDate(const QString &string) const;
    QTime toTime(const QString &string, FormatType) const;
    QDateTime toDateTime(const QString &string, FormatType format) const;

    QDate toDate(const QString &string, const QString &format) const;
    QTime toTime(const QString &string, const QString &format) const;
    QDateTime toDateTime(const QString &string, const QString &format) const;
#endif

    // Currency functions

    QString currencySymbol(CurrencySymbolFormat) const;

    QString toCurrencyString(qint64, const QString &symbol) const;
    QString toCurrencyString(quint64, const QString &symbol) const;
    QString toCurrencyString(double, const QString &symbol) const;

    // String functions

    Qt::LayoutDirection textDirection() const;

    QString toUpper(const QString &str) const;
    QString toLower(const QString &str) const;

    QString quoteString(const QStringRef &str, QuotationStyle style = StandardQuotation) const;

    QString createSeparatedList(const QStringList &strl) const;

    // Misc functions

    MeasurementSystem measurementSystem() const;
};

QT_END_NAMESPACE

#endif // QLOCALEPRIVATE_P_H
