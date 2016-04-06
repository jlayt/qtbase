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

#ifndef QDATETIMEFORMATTER_H_P
#define QDATETIMEFORMATTER_H_P

#include "qlocalecode.h"

QT_BEGIN_NAMESPACE

class QDateTimeFormatterPrivate;

class Q_CORE_EXPORT QDateTimeFormatter
{
public:
    enum ParsingMode {
        DefaultParsingMode,
        StrictParsing,
        LenientParsing,
    };

    // Create default style and locale
    QDateTimeFormatter();
    QDateTimeFormatter(const QDateTimeFormatter &other);
    ~QDateTimeFormatter();

    QDateTimeFormatter &operator=(const QDateTimeFormatter &other);

    bool isValid() const;

    QLocaleCode locale() const;

    QLocale::DateTimeStyle style() const;

    QString datePattern() const;
    QString timePattern() const;
    QString dateTimePattern() const;

    QString eraName(int era,
                    QLocale::ComponentStyle style = QLocale::DefaultComponentStyle,
                    QLocale::ComponentContext context = QLocale::DefaultContext) const;

    QString quarterName(int quarter,
                        QLocale::ComponentStyle style = QLocale::DefaultComponentStyle,
                        QLocale::ComponentContext context = QLocale::DefaultContext) const;

    QString monthName(int month,
                      QLocale::ComponentStyle style = QLocale::DefaultComponentStyle,
                      QLocale::ComponentContext context = QLocale::DefaultContext) const;

    QString weekdayName(int day,
                        QLocale::ComponentStyle style = QLocale::DefaultComponentStyle,
                        QLocale::ComponentContext context = QLocale::DefaultContext) const;

    QString dayPeriodName(const QTime &time,
                          QLocale::ComponentStyle style = QLocale::DefaultComponentStyle,
                          QLocale::ComponentContext context = QLocale::DefaultContext) const;

    qint32 shortYearWindowStart() const;

    QDateTimeFormatter::ParsingMode parsingMode() const;

    // Format with default
    QString toString(const QDate &date) const;
    QString toString(const QTime &time) const;
    QString toString(const QDateTime &dateTime) const;

    // Format with override pattern
    QString toString(const QDate &date, const QString &pattern) const;
    QString toString(const QTime &time, const QString &pattern) const;
    QString toString(const QDateTime &dateTime, const QString &pattern) const;

    // Parse with default
    QDate toDate(const QString &string, QDateTimeFormatter::ParsingMode mode = QDateTimeFormatter::DefaultParsingMode) const;
    QTime toTime(const QString &string, QDateTimeFormatter::ParsingMode mode = QDateTimeFormatter::DefaultParsingMode) const;
    QDateTime toDateTime(const QString &string, QDateTimeFormatter::ParsingMode mode = QDateTimeFormatter::DefaultParsingMode) const;

    // Parse with override pattern
    QDate toDate(const QString &string, const QString &pattern,
                 QDateTimeFormatter::ParsingMode mode = QDateTimeFormatter::DefaultParsingMode) const;
    QTime toTime(const QString &string, const QString &pattern,
                 QDateTimeFormatter::ParsingMode mode = QDateTimeFormatter::DefaultParsingMode) const;
    QDateTime toDateTime(const QString &string, const QString &pattern,
                         QDateTimeFormatter::ParsingMode mode = QDateTimeFormatter::DefaultParsingMode) const;

private:
    // Create locale with backend
    QDateTimeFormatter(QDateTimeFormatterPrivate &dd);
    friend class QDateTimeFormatterPrivate;
    friend class QCustomDateTimeFormatter;
    friend class QPatternDateTimeFormatter;
    QSharedDataPointer<QDateTimeFormatterPrivate> d;
};

class Q_CORE_EXPORT QCustomDateTimeFormatter : public QDateTimeFormatter
{
public:
    // Create named date style with default locale
    QCustomDateTimeFormatter(QLocale::DateTimeStyle style);
    // Create named date style with named locale
    QCustomDateTimeFormatter(QLocale::DateTimeStyle style, const QLocaleCode &locale);
    QCustomDateTimeFormatter(const QCustomDateTimeFormatter &other);
    ~QCustomDateTimeFormatter();

    QCustomDateTimeFormatter &operator=(const QCustomDateTimeFormatter &other);

    void setShortYearWindowStart(qint32 year);
    void setParsingMode(QDateTimeFormatter::ParsingMode mode);

private:
    // Create locale with backend
    QCustomDateTimeFormatter(QDateTimeFormatterPrivate &dd);
};

class Q_CORE_EXPORT QPatternDateTimeFormatter : public QDateTimeFormatter
{
public:
    // Create pattern in default or named locale
    QPatternDateTimeFormatter(const QString &date, const QString & time, const QLocaleCode &locale = QLocaleCode());
    QPatternDateTimeFormatter(const QPatternDateTimeFormatter &other);
    ~QPatternDateTimeFormatter();

    QPatternDateTimeFormatter &operator=(const QPatternDateTimeFormatter &other);

    void setDatePattern(const QString & pattern);
    void setTimePattern(const QString & pattern);
    void setDateTimePattern(const QString & pattern);
    void setShortYearWindowStart(qint32 year);
    void setParsingMode(QDateTimeFormatter::ParsingMode mode);

private:
    // Create locale with backend
    QPatternDateTimeFormatter(QDateTimeFormatterPrivate &dd);
};

QT_END_NAMESPACE

#endif // QDATETIMEFORMATTER_H
