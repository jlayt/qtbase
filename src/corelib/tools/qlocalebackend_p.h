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

#ifndef QLOCALEBACKEND_H
#define QLOCALEBACKEND_H

QT_BEGIN_NAMESPACE

class QLocale
{
public:
    QLocaleBackend();
    QLocaleBackend(const QString &name);
    QLocaleBackend(Language language, Script script, Country country);
    QLocaleBackend(const QLocaleBackend &other);
    ~QLocaleBackend();

    QLocaleBackend &operator=(const QLocaleBackend &other);

    bool operator==(const QLocaleBackend &other) const;

    QString bcp47Name() const;

    Language language() const;
    Script script() const;
    Country country() const;

    // ### QByteArray::fromRawData would be more optimal
    inline QString languageCode() const { return QLocalePrivate::languageToCode(QLocale::Language(m_data->m_language_id)); }
    inline QString scriptCode() const { return QLocalePrivate::scriptToCode(QLocale::Script(m_data->m_script_id)); }
    inline QString countryCode() const { return QLocalePrivate::countryToCode(QLocale::Country(m_data->m_country_id)); }

    QString nativeLanguageName() const;
    QString nativeCountryName() const;

    QStringList uiLanguages() const;

    // Number Methods
    QChar decimalPoint() const;
    QChar groupSeparator() const;
    QChar percent() const;
    QChar zeroDigit() const;
    QChar negativeSign() const;
    QChar positiveSign() const;
    QChar exponential() const;

    qint64 toInt64(const QString &s, QLocalePrivate::GroupSeparatorMode mode, bool *ok) const;
    quint64 toUint64(const QString &s, QLocalePrivate::GroupSeparatorMode mode, bool *ok) const;
    double toDouble(const QString &s, QLocalePrivate::GroupSeparatorMode mode, bool *ok) const;

    QString toString(qint64 i, QLocalePrivate::Flags flags) const;
    QString toString(quint64 i, QLocalePrivate::Flags flags) const;
    QString toString(double i, int prec, QLocalePrivate::DoubleForm form, QLocalePrivate::Flags flags) const;

    // Currency Methods
    QString currencySymbol(CurrencySymbolFormat) const;
    QString toCurrencyString(qint64, const QString &symbol) const;
    QString toCurrencyString(quint64, const QString &symbol) const;
    QString toCurrencyString(double, const QString &symbol) const;

    // Date/Time methods
    QString dateFormat(QLocale::FormatType format) const;
    QString timeFormat(QLocale::FormatType format) const;
    QString dateTimeFormat(QLocale::FormatType format) const;

    QString toString(const QDate &date, QLocale::FormatType format) const;
    QString toString(const QDate &date, const QString &format) const;
    QString toString(const QTime &time, QLocale::FormatType format) const;
    QString toString(const QTime &time, const QString &format) const;
    QString toString(const QDateTime &dateTime, QLocale::FormatType format) const;
    QString toString(const QDateTime &dateTime, const QString &format) const;

    QDate toDate(const QString &string, QLocale::FormatType) const;
    QDate toDate(const QString &string, const QString &format) const;
    QTime toTime(const QString &string, QLocale::FormatType) const;
    QTime toTime(const QString &string, const QString &format) const;
    QDateTime toDateTime(const QString &string, QLocale::FormatType format) const;
    QDateTime toDateTime(const QString &string, const QString &format) const;

    QString monthName(int, QLocale::FormatType format) const;
    QString standaloneMonthName(int, QLocale::FormatType format) const;
    QString dayName(int, QLocale::FormatType format) const;
    QString standaloneDayName(int, QLocale::FormatType format) const;

    Qt::DayOfWeek firstDayOfWeek() const;
    QList<Qt::DayOfWeek> weekdays() const;

    QString amText() const;
    QString pmText() const;

    // Other Methods
    MeasurementSystem measurementSystem() const;

    Qt::LayoutDirection textDirection() const;

    QString toUpper(const QString &str) const;
    QString toLower(const QString &str) const;

    QString quoteString(const QStringRef &str, QLocale::QuotationStyle style) const;

    QString createSeparatedList(const QStringList &list) const;

private:
};

QT_END_NAMESPACE

#endif // QLOCALE_H
