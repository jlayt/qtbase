/****************************************************************************
**
** Copyright (C) 2012 John Layt <jlayt@kde.org>
** Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCALENDARSYSTEM_H
#define QCALENDARSYSTEM_H

#include <QtCore/qnamespace.h>
#include <QtCore/qlocale.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QCalendarSystemPrivate;
class QDate;

class Q_CORE_EXPORT QCalendarSystem
{
public:
    explicit QCalendarSystem(QLocale::CalendarSystem calendar = QLocale::DefaultCalendar);
    ~QCalendarSystem();

    QLocale::CalendarSystem calendarSystem() const;

    QDate epoch() const;
    QDate earliestValidDate() const;
    QDate latestValidDate() const;
    int maximumMonthsInYear() const;
    int maximumDaysInYear() const;
    int maximumDaysInMonth() const;

    bool isValid(const QDate &date) const;
    bool isValid(int year, int month, int day) const;
    bool isValid(int year, int dayOfYear) const;

    QDate date(int year, int month, int day) const;
    QDate date(int year, int dayOfYear) const;

    void getDate(const QDate &date, int *year, int *month, int *day) const;

    int year(const QDate &date) const;
    int month(const QDate &date) const;
    int day(const QDate &date) const;

    int quarter(const QDate &date) const;
    int quarter(int year, int month, int day) const;

    int dayOfYear(const QDate &date) const;
    int dayOfYear(int year, int month, int day) const;

    int dayOfWeek(const QDate &date) const;
    int dayOfWeek(int year, int month, int day) const;

    int weekNumber(const QDate &date, int *yearNum = 0,
                   QLocale::WeekNumberSystem system = QLocale::DefaultWeekNumber) const;
    int weekNumber(int year, int month, int day, int *yearNum = 0,
                   QLocale::WeekNumberSystem system = QLocale::DefaultWeekNumber) const;

    int monthsInYear(const QDate &date) const;
    int monthsInYear(int year) const;

    int weeksInYear(const QDate &date,
                    QLocale::WeekNumberSystem system = QLocale::DefaultWeekNumber) const;
    int weeksInYear(int year,
                    QLocale::WeekNumberSystem system = QLocale::DefaultWeekNumber) const;

    int daysInYear(const QDate &date) const;
    int daysInYear(int year) const;

    int daysInMonth(const QDate &date) const;
    int daysInMonth(int year, int month) const;

    int daysInWeek() const;

    bool isLeapYear(const QDate &date) const;
    bool isLeapYear(int year) const;

    QDate addYears(const QDate &date, int years) const;
    QDate addMonths(const QDate &date, int months) const;
    QDate addDays(const QDate &date, int days) const;

    int yearsDifference(const QDate &fromDate, const QDate &toDate) const;
    int monthsDifference(const QDate &fromDate, const QDate &toDate) const;
    qint64 daysDifference(const QDate &fromDate, const QDate &toDate) const;

    void dateDifference(const QDate &fromDate, const QDate &toDate,
                        int *years, int *months, int *days, int *direction) const;

    QDate firstDayOfYear(const QDate &date) const;
    QDate firstDayOfYear(int year) const;
    QDate lastDayOfYear(const QDate &date) const;
    QDate lastDayOfYear(int year) const;

    QDate firstDayOfMonth(const QDate &date) const;
    QDate firstDayOfMonth(int year, int month) const;
    QDate lastDayOfMonth(const QDate &date) const;
    QDate lastDayOfMonth(int year, int month) const;

private:
    QCalendarSystemPrivate * const d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QCALENDARSYSTEM_H
