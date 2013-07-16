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

#include "qdatetimeformatter_p.h"

#include "qdatetime.h"

#include "unicode/udat.h"

QT_BEGIN_NAMESPACE

class QDateTimeFormatterPrivate : public QSharedData
{
public:
    QDateTimeFormatterPrivate(QDateTimeFormatter::DateTimeStyle style, const QString &localeCode);
    ~QDateTimeFormatterPrivate();

    // Return if a sepcific ICU formatter is valid
    // Only for internal use, triggers creation of formatter!
    bool isValid(QMetaType::Type type = QMetaType::QDateTime) const;

    QString localeCode(QLocale::LocaleCodeType type = QLocale::RequestedLocaleCode) const;

    QDateTimeFormatter::DateTimeStyle style() const;

    QString datePattern() const;
    QString timePattern() const;
    QString dateTimePattern() const;

    void setDatePattern(const QString & pattern);
    void setTimePattern(const QString & pattern);
    void setDateTimePattern(const QString & pattern);

    qint32 shortYearWindowStart() const;
    void setShortYearWindowStart(qint32 year);

    QLocale::ParsingMode parsingMode() const;
    void setParsingMode(QLocale::ParsingMode mode);

    // Format with default
    QString toString(const QDate &date) const;
    QString toString(const QTime &time) const;
    QString toString(const QDateTime &dateTime) const;

    // Format with override pattern
    QString toString(const QDate &date, const QString &pattern) const;
    QString toString(const QTime &time, const QString &pattern) const;
    QString toString(const QDateTime &dateTime, const QString &pattern) const;

    // Parse with default
    QDate toDate(const QString &string,
                 QLocale::ParsingMode mode = QLocale::DefaultParsingMode) const;
    QTime toTime(const QString &string,
                 QLocale::ParsingMode mode = QLocale::DefaultParsingMode) const;
    QDateTime toDateTime(const QString &string,
                         QLocale::ParsingMode mode = QLocale::DefaultParsingMode) const;

    // Parse with override pattern
    QDate toDate(const QString &string, const QString &pattern,
                 QLocale::ParsingMode mode = QLocale::DefaultParsingMode) const;
    QTime toTime(const QString &string, const QString &pattern,
                 QLocale::ParsingMode mode = QLocale::DefaultParsingMode) const;
    QDateTime toDateTime(const QString &string, const QString &pattern,
                         QLocale::ParsingMode mode = QLocale::DefaultParsingMode) const;

    QString eraName(int era,
                    QDateTimeFormatter::ComponentStyle style = QDateTimeFormatter::DefaultComponentStyle,
                    QDateTimeFormatter::ComponentContext context = QDateTimeFormatter::DefaultContext) const;

    QString quarterName(int quarter,
                        QDateTimeFormatter::ComponentStyle style = QDateTimeFormatter::DefaultComponentStyle,
                        QDateTimeFormatter::ComponentContext context = QDateTimeFormatter::DefaultContext) const;

    QString monthName(int month,
                      QDateTimeFormatter::ComponentStyle style = QDateTimeFormatter::DefaultComponentStyle,
                      QDateTimeFormatter::ComponentContext context = QDateTimeFormatter::DefaultContext) const;

    QString weekdayName(int day,
                        QDateTimeFormatter::ComponentStyle style = QDateTimeFormatter::DefaultComponentStyle,
                        QDateTimeFormatter::ComponentContext context = QDateTimeFormatter::DefaultContext) const;

    QString dayPeriodName(const QTime &time,
                          QDateTimeFormatter::ComponentStyle style = QDateTimeFormatter::DefaultComponentStyle,
                          QDateTimeFormatter::ComponentContext context = QDateTimeFormatter::DefaultContext) const;

private:
    QString m_locale;
    QDateTimeFormatter::DateTimeStyle m_style;

#ifdef QT_USE_ICU
    UDateFormat *m_dateFormatter;
    UDateFormat *m_timeFormatter;
    UDateFormat *m_dateTimeFormatter;
#endif // QT_USE_ICU

#ifdef Q_OS_MAC
    NSDateFormatter *m_dtf;
#endif // Q_OS_MAC
};

QT_END_NAMESPACE

#endif // QDATETIMEFORMATTERPRIVATE_P_H
