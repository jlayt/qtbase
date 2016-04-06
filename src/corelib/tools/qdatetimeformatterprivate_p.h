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

#ifndef QDATETIMEFORMATTERPRIVATE_P_H
#define QDATETIMEFORMATTERPRIVATE_P_H

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

#include "qdatetimeformatter.h"

#include "qdatetime.h"

#ifdef QT_USE_ICU
#include <unicode/udat.h>
#endif // QT_USE_ICU

QT_BEGIN_NAMESPACE

class Q_AUTOTEST_EXPORT QDateTimeFormatterPrivate : public QSharedData
{
public:
    QDateTimeFormatterPrivate(QLocale::DateTimeStyle style, const QLocaleCode &locale);
    QDateTimeFormatterPrivate(const QString date, const QString time, const QLocaleCode &locale);
    QDateTimeFormatterPrivate(const QDateTimeFormatterPrivate &other);
    virtual ~QDateTimeFormatterPrivate();

    virtual QDateTimeFormatterPrivate *clone();

    // Only for internal use, triggers creation of formatter!
    virtual bool isValid(QMetaType::Type type = QMetaType::QDateTime) const;

    virtual QLocaleCode locale() const;

    virtual QLocale::DateTimeStyle style() const;
    virtual QList<QLocale::DateTimeStyle> availableStyles();

    virtual QString datePattern() const;
    virtual QString timePattern() const;
    virtual QString dateTimePattern() const;

    virtual void setDatePattern(const QString & pattern);
    virtual void setTimePattern(const QString & pattern);
    virtual void setDateTimePattern(const QString & pattern);

    virtual qint32 shortYearWindowStart() const;
    virtual void setShortYearWindowStart(qint32 year);

    virtual QDateTimeFormatter::ParsingMode parsingMode() const;
    virtual void setParsingMode(QDateTimeFormatter::ParsingMode mode);

    // Format with default
    virtual QString toString(const QDate &date) const;
    virtual QString toString(const QTime &time) const;
    virtual QString toString(const QDateTime &dateTime) const;

    // Format with override pattern
    virtual QString toString(const QDate &date, const QString &pattern) const;
    virtual QString toString(const QTime &time, const QString &pattern) const;
    virtual QString toString(const QDateTime &dateTime, const QString &pattern) const;

    // Parse with default
    virtual QDate toDate(const QString &string, QDateTimeFormatter::ParsingMode mode) const;
    virtual QTime toTime(const QString &string, QDateTimeFormatter::ParsingMode mode) const;
    virtual QDateTime toDateTime(const QString &string, QDateTimeFormatter::ParsingMode mode) const;

    // Parse with override pattern
    virtual QDate toDate(const QString &string, const QString &pattern, QDateTimeFormatter::ParsingMode mode) const;
    virtual QTime toTime(const QString &string, const QString &pattern, QDateTimeFormatter::ParsingMode mode) const;
    virtual QDateTime toDateTime(const QString &string, const QString &pattern, QDateTimeFormatter::ParsingMode mode) const;

    virtual QString eraName(int era, QLocale::ComponentStyle style, QLocale::ComponentContext context) const;
    virtual QString quarterName(int quarter, QLocale::ComponentStyle style, QLocale::ComponentContext context) const;
    virtual QString monthName(int month, QLocale::ComponentStyle style, QLocale::ComponentContext context) const;
    virtual QString weekdayName(int day, QLocale::ComponentStyle style, QLocale::ComponentContext context) const;
    virtual QString dayPeriodName(const QTime &time, QLocale::ComponentStyle style, QLocale::ComponentContext context) const;

protected:
    QLocale::DateTimeStyle m_style;
    QLocaleCode m_locale;
};

template<> QDateTimeFormatterPrivate *QSharedDataPointer<QDateTimeFormatterPrivate>::clone();

#ifdef QT_USE_ICU
class Q_AUTOTEST_EXPORT QIcuDateTimeFormatterPrivate Q_DECL_FINAL : public QDateTimeFormatterPrivate
{
public:
    QIcuDateTimeFormatterPrivate(QLocale::DateTimeStyle style, const QLocaleCode &locale);
    QIcuDateTimeFormatterPrivate(const QIcuDateTimeFormatterPrivate &other);
    virtual ~QIcuDateTimeFormatterPrivate();

    QIcuDateTimeFormatterPrivate *clone() Q_DECL_OVERRIDE;

    bool isValid(QMetaType::Type type = QMetaType::QDateTime) const Q_DECL_OVERRIDE;

    QList<QLocale::DateTimeStyle> availableStyles() Q_DECL_OVERRIDE;

    QString datePattern() const Q_DECL_OVERRIDE;
    QString timePattern() const Q_DECL_OVERRIDE;
    QString dateTimePattern() const Q_DECL_OVERRIDE;

    void setDatePattern(const QString & pattern) Q_DECL_OVERRIDE;
    void setTimePattern(const QString & pattern) Q_DECL_OVERRIDE;
    void setDateTimePattern(const QString & pattern) Q_DECL_OVERRIDE;

    qint32 shortYearWindowStart() const Q_DECL_OVERRIDE;
    void setShortYearWindowStart(qint32 year) Q_DECL_OVERRIDE;

    QString toString(const QDate &date) const Q_DECL_OVERRIDE;
    QString toString(const QTime &time) const Q_DECL_OVERRIDE;
    QString toString(const QDateTime &dateTime) const Q_DECL_OVERRIDE;

    QString toString(const QDate &date, const QString &pattern) const Q_DECL_OVERRIDE;
    QString toString(const QTime &time, const QString &pattern) const Q_DECL_OVERRIDE;
    QString toString(const QDateTime &dateTime, const QString &pattern) const Q_DECL_OVERRIDE;

    QDate toDate(const QString &string, QDateTimeFormatter::ParsingMode mode) const Q_DECL_OVERRIDE;
    QTime toTime(const QString &string, QDateTimeFormatter::ParsingMode mode) const Q_DECL_OVERRIDE;
    QDateTime toDateTime(const QString &string, QDateTimeFormatter::ParsingMode mode) const Q_DECL_OVERRIDE;

    QDate toDate(const QString &string, const QString &pattern, QDateTimeFormatter::ParsingMode mode) const Q_DECL_OVERRIDE;
    QTime toTime(const QString &string, const QString &pattern, QDateTimeFormatter::ParsingMode mode) const Q_DECL_OVERRIDE;
    QDateTime toDateTime(const QString &string, const QString &pattern, QDateTimeFormatter::ParsingMode mode) const Q_DECL_OVERRIDE;

    QString eraName(int era, QLocale::ComponentStyle style, QLocale::ComponentContext context) const Q_DECL_OVERRIDE;
    QString quarterName(int quarter, QLocale::ComponentStyle style, QLocale::ComponentContext context) const Q_DECL_OVERRIDE;
    QString monthName(int month, QLocale::ComponentStyle style, QLocale::ComponentContext context) const Q_DECL_OVERRIDE;
    QString weekdayName(int day, QLocale::ComponentStyle style, QLocale::ComponentContext context) const Q_DECL_OVERRIDE;
    QString dayPeriodName(const QTime &time, QLocale::ComponentStyle style, QLocale::ComponentContext context) const Q_DECL_OVERRIDE;

private:
    UDateFormat *m_dateFormatter;
    UDateFormat *m_timeFormatter;
    UDateFormat *m_dateTimeFormatter;
};
#endif // QT_USE_ICU

QT_END_NAMESPACE

#endif // QDATETIMEFORMATTERPRIVATE_P_H
