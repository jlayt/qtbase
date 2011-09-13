/****************************************************************************
**
** Copyright (C) 2011 John Layt <jlayt@kde.org>
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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


#include <QtTest/QtTest>
#include <qdatecalculator.h>
#include <qdatetime.h>
#include <qlocale.h>

//TESTED_CLASS=
//TESTED_FILES=corelib/tools/qdatecalculator.h corelib/tools/qdatecalculator.cpp

class tst_QCalendarSystem : public QObject
{
Q_OBJECT

public:
    tst_QCalendarSystem();
    virtual ~tst_QCalendarSystem();

    void roundtrip(QLocale::CalendarSystem calSys, QDate startDate, QDate endDate);
    void testDate(QLocale::CalendarSystem calSys, int year, int month, int day, qint64 jd);
    void testMonths(QLocale::CalendarSystem calSys, int year, int month, int day, bool isValid);
    void testYears(QLocale::CalendarSystem calSys, int year, int miy, int diy, bool isLeap, bool isValid);

public slots:
    void init();
    void cleanup();

private slots:
    void ymd();
    void ordinal();
    void quarter_data();
    void quarter();
    void dayOfYear_data();
    void dayOfYear();
    void dayOfWeek_data();
    void dayOfWeek();
    void weekNumber_data();
    void weekNumber();
    void daysInWeek();
    void isLeapYear();
    void addYears_data();
    void addYears();
    void addMonths_data();
    void addMonths();
    void addDays_data();
    void addDays();
    void yearsDifference_data();
    void yearsDifference();
    void monthsDifference_data();
    void monthsDifference();
    void daysDifference_data();
    void daysDifference();
    void dateDifference_data();
    void dateDifference();
    void firstDayOfYear();
    void lastDayOfYear();
    void firstDayOfMonth();
    void lastDayOfMonth();

    void gregorianCalendar();
    void gregorianRoundtrip();
    void gregorianDate_data();
    void gregorianDate();
    void gregorianYears_data();
    void gregorianYears();
    void gregorianMonths_data();
    void gregorianMonths();

//TODO Tests for Coptic, Ethipoic, EthiopicAmeteAlem, IndianNational, IslamicCivil

    void iso8601Calendar();
    void iso8601Roundtrip();
    void iso8601Date_data();
    void iso8601Date();
    void iso8601Years_data();
    void iso8601Years();
    void iso8601Months_data();
    void iso8601Months();

    void japaneseCalendar();
    void japaneseRoundtrip();
    void japaneseDate_data();
    void japaneseDate();
    void japaneseYears_data();
    void japaneseYears();
    void japaneseMonths_data();
    void japaneseMonths();

    void julianCalendar();
    void julianRoundtrip();
    void julianDate_data();
    void julianDate();
    void julianYears_data();
    void julianYears();
    void julianMonths_data();
    void julianMonths();

    void rocCalendar();
    void rocRoundtrip();
    void rocDate_data();
    void rocDate();
    void rocYears_data();
    void rocYears();
    void rocMonths_data();
    void rocMonths();

    void thaiCalendar();
    void thaiRoundtrip();
    void thaiDate_data();
    void thaiDate();
    void thaiYears_data();
    void thaiYears();
    void thaiMonths_data();
    void thaiMonths();
};

Q_DECLARE_METATYPE(QDate)

tst_QCalendarSystem::tst_QCalendarSystem()
{
}

tst_QCalendarSystem::~tst_QCalendarSystem()
{
}

void tst_QCalendarSystem::init()
{
// This will be executed immediately before each test is run.
// TODO: Add initialization code here.
}

void tst_QCalendarSystem::cleanup()
{
// This will be executed immediately after each test is run.
// TODO: Add cleanup code here.
}


// Generic method tests

void tst_QCalendarSystem::ymd()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QDate date = QDate::fromJulianDay(2451545);

    int y, m, d;
    cal.ymd(date, &y, &m, &d);
    QCOMPARE(cal.date(2000, 1, 1), date);
    QCOMPARE(cal.year(date), 2000);
    QCOMPARE(cal.month(date), 1);
    QCOMPARE(cal.day(date), 1);
    QCOMPARE(y, 2000);
    QCOMPARE(m, 1);
    QCOMPARE(d, 1);
}

void tst_QCalendarSystem::ordinal()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QCOMPARE(cal.isValid(2000, 0) , false);
    QCOMPARE(cal.isValid(2000, 1) , true);
    QCOMPARE(cal.isValid(2000, 366) , true);
    QCOMPARE(cal.isValid(2000, 367) , false);
}

void tst_QCalendarSystem::quarter_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("day");
    QTest::addColumn<int>("quarter");

    QTest::newRow("data0")  <<     0 <<  0 <<  0 <<   0;
    QTest::newRow("data1")  <<  2000 <<  1 <<  1 <<   1;
    QTest::newRow("data2")  <<  2000 <<  2 <<  1 <<   1;
    QTest::newRow("data3")  <<  2000 <<  3 << 31 <<   1;
    QTest::newRow("data4")  <<  2000 <<  4 <<  1 <<   2;
    QTest::newRow("data5")  <<  2000 <<  5 <<  1 <<   2;
    QTest::newRow("data6")  <<  2000 <<  6 << 30 <<   2;
    QTest::newRow("data7")  <<  2000 <<  7 <<  1 <<   3;
    QTest::newRow("data8")  <<  2000 <<  8 <<  1 <<   3;
    QTest::newRow("data9")  <<  2000 <<  9 << 30 <<   3;
    QTest::newRow("data10") <<  2000 << 10 <<  1 <<   4;
    QTest::newRow("data11") <<  2000 << 11 <<  1 <<   4;
    QTest::newRow("data12") <<  2000 << 12 << 31 <<   4;
}

void tst_QCalendarSystem::quarter()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, day);
    QFETCH(int, quarter);

    QDate date = cal.date(year, month, day);
    QCOMPARE(cal.quarter(date), quarter);
    QCOMPARE(cal.quarter(year, month, day), quarter);
}

void tst_QCalendarSystem::dayOfYear_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("day");
    QTest::addColumn<int>("dayOfYear");

    QTest::newRow("data0")  <<     0 <<  0 <<  0 <<   0;
    QTest::newRow("data1")  <<  2000 <<  1 <<  1 <<   1;
    QTest::newRow("data2")  <<  2000 <<  1 <<  2 <<   2;
    QTest::newRow("data3")  <<  2000 <<  1 <<  3 <<   3;
    QTest::newRow("data4")  <<  2000 << 12 << 31 << 366;
    QTest::newRow("data5")  <<  2001 << 12 << 31 << 365;
    QTest::newRow("data12") << -4800 <<  1 <<  1 <<   1;
    QTest::newRow("data13") << -4800 << 12 << 31 << 365;
}

void tst_QCalendarSystem::dayOfYear()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, day);
    QFETCH(int, dayOfYear);

    QDate date1 = cal.date(year, month, day);
    QDate date2 = cal.date(year, dayOfYear);
    QCOMPARE(cal.dayOfYear(date1), dayOfYear);
    QCOMPARE(cal.dayOfYear(year, month, day), dayOfYear);
    QCOMPARE(cal.isValid(year, month, day), cal.isValid(year, dayOfYear));
    QCOMPARE(date1, date2);
}

void tst_QCalendarSystem::dayOfWeek_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("day");
    QTest::addColumn<int>("dayOfWeek");

    QTest::newRow("data0")  <<     0 <<  0 <<  0 << 0;

    QTest::newRow("data1")  <<  2006 << 12 << 25 << 1;
    QTest::newRow("data2")  <<  2006 << 12 << 26 << 2;
    QTest::newRow("data3")  <<  2006 << 12 << 27 << 3;
    QTest::newRow("data4")  <<  2006 << 12 << 28 << 4;
    QTest::newRow("data5")  <<  2006 << 12 << 29 << 5;
    QTest::newRow("data6")  <<  2006 << 12 << 30 << 6;
    QTest::newRow("data7")  <<  2006 << 12 << 31 << 7;
    QTest::newRow("data8")  <<  2007 <<  1 <<  1 << 1;
    QTest::newRow("data9")  <<  2007 <<  1 <<  2 << 2;
    QTest::newRow("data10") <<  2007 <<  1 <<  3 << 3;
    QTest::newRow("data11") <<  2007 <<  1 <<  4 << 4;
    QTest::newRow("data12") <<  2007 <<  1 <<  5 << 5;
    QTest::newRow("data13") <<  2007 <<  1 <<  6 << 6;
    QTest::newRow("data14") <<  2007 <<  1 <<  7 << 7;
    QTest::newRow("data15") <<  2007 <<  1 <<  8 << 1;
    QTest::newRow("data16") <<  2007 <<  1 <<  9 << 2;
    QTest::newRow("data17") <<  2007 <<  1 << 10 << 3;
    QTest::newRow("data18") <<  2007 <<  1 << 11 << 4;
    QTest::newRow("data19") <<  2007 <<  1 << 12 << 5;
    QTest::newRow("data20") <<  2007 <<  1 << 13 << 6;
    QTest::newRow("data21") <<  2007 <<  1 << 14 << 7;

    QTest::newRow("data22") << -4714 << 12 <<  1 << 1; // jd =  7
    QTest::newRow("data23") << -4714 << 11 << 30 << 7; // jd =  6
    QTest::newRow("data24") << -4714 << 11 << 29 << 6; // jd =  5
    QTest::newRow("data25") << -4714 << 11 << 28 << 5; // jd =  4
    QTest::newRow("data26") << -4714 << 11 << 27 << 4; // jd =  3
    QTest::newRow("data27") << -4714 << 11 << 26 << 3; // jd =  2
    QTest::newRow("data28") << -4714 << 11 << 25 << 2; // jd =  1
    QTest::newRow("data29") << -4714 << 11 << 24 << 1; // jd =  0
    QTest::newRow("data30") << -4714 << 11 << 23 << 7; // jd = -1
    QTest::newRow("data31") << -4714 << 11 << 22 << 6; // jd = -2
    QTest::newRow("data32") << -4714 << 11 << 21 << 5; // jd = -3
    QTest::newRow("data33") << -4714 << 11 << 20 << 4; // jd = -4
    QTest::newRow("data34") << -4714 << 11 << 19 << 3; // jd = -5
    QTest::newRow("data35") << -4714 << 11 << 18 << 2; // jd = -6
    QTest::newRow("data36") << -4714 << 11 << 17 << 1; // jd = -7
    QTest::newRow("data37") << -4714 << 11 << 16 << 7; // jd = -8
}

void tst_QCalendarSystem::dayOfWeek()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, day);
    QFETCH(int, dayOfWeek);

    QDate date = cal.date(year, month, day);
    QCOMPARE(cal.dayOfWeek(date), dayOfWeek);
    QCOMPARE(cal.dayOfWeek(year,month, day), dayOfWeek);
}

void tst_QCalendarSystem::weekNumber_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("day");
    QTest::addColumn<int>("week");
    QTest::addColumn<int>("weekYear");
    QTest::addColumn<int>("weeksInYear");

    QTest::newRow( "data1" )  <<    0 <<  0 <<  0 <<  0 <<    0 <<  0;
    QTest::newRow( "data2" )  << 2018 <<  1 <<  1 <<  1 << 2018 << 52;
    QTest::newRow( "data3" )  << 2013 <<  1 <<  1 <<  1 << 2013 << 52;
    QTest::newRow( "data4" )  << 2014 <<  1 <<  1 <<  1 << 2014 << 52;
    QTest::newRow( "data5" )  << 2015 <<  1 <<  1 <<  1 << 2015 << 53;
    QTest::newRow( "data6" )  << 2016 <<  1 <<  1 << 53 << 2015 << 52;
    QTest::newRow( "data7" )  << 2011 <<  1 <<  1 << 52 << 2010 << 52;
    QTest::newRow( "data8" )  << 2012 <<  1 <<  1 << 52 << 2011 << 52;
    QTest::newRow( "data9" )  << 2011 << 12 << 31 << 52 << 2011 << 52;
    QTest::newRow( "data10" ) << 2012 << 12 << 31 <<  1 << 2013 << 52;
    QTest::newRow( "data11" ) << 2015 << 12 << 31 << 53 << 2015 << 53;
}

void tst_QCalendarSystem::weekNumber()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, day);
    QFETCH(int, week);
    QFETCH(int, weekYear);
    QFETCH(int, weeksInYear);

    int yearNum;
    QDate date = cal.date(year, month, day);
    QCOMPARE(cal.weekNumber(date, &yearNum), week);
    QCOMPARE(cal.weekNumber(year, month, day, &yearNum), week);
    QCOMPARE(yearNum, weekYear);
    QCOMPARE(cal.weeksInYear(year), weeksInYear);
}

void tst_QCalendarSystem::daysInWeek()
{
    QDateCalculator cal(QLocale::GregorianCalendar);
    QCOMPARE(cal.daysInWeek(), 7);
}

void tst_QCalendarSystem::isLeapYear()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QDate dt;
    for (int y = -1501; y <= -500; ++y)
        QCOMPARE(cal.isValid(y, 2, 29), cal.isLeapYear(y));
    for (int y = -501; y <= 500; ++y)
        QCOMPARE(cal.isValid(y, 2, 29), cal.isLeapYear(y));
    for (int y = 1500; y <= 2500; ++y)
        QCOMPARE(cal.isValid(y, 2, 29), cal.isLeapYear(y));
}

void tst_QCalendarSystem::addYears_data()
{
    QTest::addColumn<int>("y1");
    QTest::addColumn<int>("m1");
    QTest::addColumn<int>("d1");
    QTest::addColumn<int>("add");
    QTest::addColumn<int>("y2");
    QTest::addColumn<int>("m2");
    QTest::addColumn<int>("d2");

    QTest::newRow("data0")  << 2000 <<  1 <<  1 << 1 << 2001 <<  1 <<  1;
    QTest::newRow("data1")  << 2000 <<  1 << 31 << 1 << 2001 <<  1 << 31;
    QTest::newRow("data2")  << 2000 <<  2 << 28 << 1 << 2001 <<  2 << 28;
    QTest::newRow("data3")  << 2000 <<  2 << 29 << 1 << 2001 <<  2 << 28;
    QTest::newRow("data4")  << 2000 << 12 << 31 << 1 << 2001 << 12 << 31;
    QTest::newRow("data5")  << 2001 <<  2 << 28 << 3 << 2004 <<  2 << 28;
    QTest::newRow("data6")  << 2000 <<  2 << 29 << 4 << 2004 <<  2 << 29;

    QTest::newRow("data7")  << 2000 <<  1 << 31 << -1 << 1999 <<  1 << 31;
    QTest::newRow("data9")  << 2000 <<  2 << 29 << -1 << 1999 <<  2 << 28;
    QTest::newRow("data10") << 2000 << 12 << 31 << -1 << 1999 << 12 << 31;
    QTest::newRow("data11") << 2001 <<  2 << 28 << -3 << 1998 <<  2 << 28;
    QTest::newRow("data12") << 2000 <<  2 << 29 << -4 << 1996 <<  2 << 29;
    QTest::newRow("data13") << 2000 <<  2 << 29 << -5 << 1995 <<  2 << 28;

    QTest::newRow("data14") <<  2000 << 1 << 1 << -1999 <<  1 << 1 << 1;
    QTest::newRow("data15") <<  2000 << 1 << 1 << -2000 << -1 << 1 << 1;
    QTest::newRow("data16") <<  2000 << 1 << 1 << -2001 << -2 << 1 << 1;
    QTest::newRow("data17") << -2000 << 1 << 1 <<  1999 << -1 << 1 << 1;
    QTest::newRow("data18") << -2000 << 1 << 1 <<  2000 <<  1 << 1 << 1;
    QTest::newRow("data19") << -2000 << 1 << 1 <<  2001 <<  2 << 1 << 1;
}

void tst_QCalendarSystem::addYears()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QFETCH(int, y1);
    QFETCH(int, m1);
    QFETCH(int, d1);
    QFETCH(int, add);
    QFETCH(int, y2);
    QFETCH(int, m2);
    QFETCH(int, d2);

    QDate dt1 = cal.date(y1, m1, d1);
    QDate dt2 = cal.date(y2, m2, d2);
    QDate dt3 = cal.addYears(dt1, add);
    QCOMPARE(dt3, dt2);
}

void tst_QCalendarSystem::addMonths_data()
{
    QTest::addColumn<int>("y1");
    QTest::addColumn<int>("m1");
    QTest::addColumn<int>("d1");
    QTest::addColumn<int>("add");
    QTest::addColumn<int>("y2");
    QTest::addColumn<int>("m2");
    QTest::addColumn<int>("d2");

    QTest::newRow("data0")  << 2000 <<  1 <<  1 <<   1 << 2000 <<  2 <<  1;
    QTest::newRow("data1")  << 2000 <<  1 << 31 <<   1 << 2000 <<  2 << 29;
    QTest::newRow("data2")  << 2000 <<  2 << 28 <<   1 << 2000 <<  3 << 28;
    QTest::newRow("data3")  << 2000 <<  2 << 29 <<   1 << 2000 <<  3 << 29;
    QTest::newRow("data4")  << 2000 << 12 << 31 <<   1 << 2001 <<  1 << 31;
    QTest::newRow("data5")  << 2001 <<  2 << 28 <<   1 << 2001 <<  3 << 28;
    QTest::newRow("data6")  << 2001 <<  2 << 28 <<  12 << 2002 <<  2 << 28;
    QTest::newRow("data7")  << 2000 <<  2 << 29 <<  12 << 2001 <<  2 << 28;
    QTest::newRow("data8")  << 2000 << 10 << 15 <<   4 << 2001 <<  2 << 15;
    QTest::newRow("data9")  << 2000 <<  1 <<  1 <<  -1 << 1999 << 12 <<  1;
    QTest::newRow("data10") << 2000 <<  1 << 31 <<  -1 << 1999 << 12 << 31;
    QTest::newRow("data11") << 2000 << 12 << 31 <<  -1 << 2000 << 11 << 30;
    QTest::newRow("data12") << 2001 <<  2 << 28 << -12 << 2000 <<  2 << 28;
    QTest::newRow("data13") << 2000 <<  1 << 31 <<  -7 << 1999 <<  6 << 30;
    QTest::newRow("data14") << 2000 <<  2 << 29 << -12 << 1999 <<  2 << 28;
    QTest::newRow("data14") <<    1 <<  1 <<  1 <<  -1 <<   -1 << 12 <<  1;
    QTest::newRow("data15") <<    1 <<  1 <<  1 << -12 <<   -1 <<  1 <<  1;
    QTest::newRow("data16") <<   -1 << 12 <<  1 <<   1 <<    1 <<  1 <<  1;
    QTest::newRow("data17") <<   -1 <<  1 <<  1 <<  12 <<    1 <<  1 <<  1;
}

void tst_QCalendarSystem::addMonths()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QFETCH(int, y1);
    QFETCH(int, m1);
    QFETCH(int, d1);
    QFETCH(int, add);
    QFETCH(int, y2);
    QFETCH(int, m2);
    QFETCH(int, d2);

    QDate dt1 = cal.date(y1, m1, d1);
    QDate dt2 = cal.date(y2, m2, d2);
    QDate dt3 = cal.addMonths(dt1, add);
    QCOMPARE(dt3, dt2);
}

void tst_QCalendarSystem::addDays_data()
{
    QTest::addColumn<int>("y1");
    QTest::addColumn<int>("m1");
    QTest::addColumn<int>("d1");
    QTest::addColumn<int>("add");
    QTest::addColumn<int>("y2");
    QTest::addColumn<int>("m2");
    QTest::addColumn<int>("d2");

    QTest::newRow("data0")  <<  2000 <<  1 <<  1 <<   1 <<  2000 <<  1 <<  2;
    QTest::newRow("data1")  <<  2000 <<  1 << 31 <<   1 <<  2000 <<  2 <<  1;
    QTest::newRow("data2")  <<  2000 <<  2 << 28 <<   1 <<  2000 <<  2 << 29;
    QTest::newRow("data3")  <<  2000 <<  2 << 29 <<   1 <<  2000 <<  3 <<  1;
    QTest::newRow("data4")  <<  2000 << 12 << 31 <<   1 <<  2001 <<  1 <<  1;
    QTest::newRow("data5")  <<  2001 <<  2 << 28 <<   1 <<  2001 <<  3 <<  1;
    QTest::newRow("data6")  <<  2001 <<  2 << 28 <<  30 <<  2001 <<  3 << 30;
    QTest::newRow("data7")  <<  2001 <<  3 << 30 <<   5 <<  2001 <<  4 <<  4;
    QTest::newRow("data8")  <<  2000 <<  1 <<  1 <<  -1 <<  1999 << 12 << 31;
    QTest::newRow("data9")  <<  2000 <<  1 << 31 <<  -1 <<  2000 <<  1 << 30;
    QTest::newRow("data10") <<  2000 <<  2 << 28 <<  -1 <<  2000 <<  2 << 27;
    QTest::newRow("data11") <<  2001 <<  2 << 28 << -30 <<  2001 <<  1 << 29;
}

void tst_QCalendarSystem::addDays()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QFETCH(int, y1);
    QFETCH(int, m1);
    QFETCH(int, d1);
    QFETCH(int, add);
    QFETCH(int, y2);
    QFETCH(int, m2);
    QFETCH(int, d2);

    QDate dt1 = cal.date(y1, m1, d1);
    QDate dt2 = cal.date(y2, m2, d2);
    QDate dt3 = cal.addDays(dt1, add);
    QCOMPARE(dt3, dt2);
}

void tst_QCalendarSystem::yearsDifference_data()
{
    QTest::addColumn<int>("y1");
    QTest::addColumn<int>("m1");
    QTest::addColumn<int>("d1");
    QTest::addColumn<int>("y2");
    QTest::addColumn<int>("m2");
    QTest::addColumn<int>("d2");
    QTest::addColumn<int>("dy");

    QTest::newRow("data0")  << 2010 <<  1 <<  1 << 2010 <<  1 <<  1 << 0;
    QTest::newRow("data1")  << 2010 <<  1 <<  1 << 2010 << 12 << 31 << 0;
    QTest::newRow("data2")  << 2010 <<  6 <<  1 << 2010 <<  7 <<  1 << 0;
    QTest::newRow("data3")  << 2010 <<  6 <<  1 << 2011 <<  5 <<  1 << 0;
    QTest::newRow("data4")  << 2010 <<  6 <<  1 << 2011 <<  7 <<  1 << 1;
    QTest::newRow("data5")  << 2010 <<  6 << 20 << 2012 <<  6 << 20 << 2;
    QTest::newRow("data6")  << 2010 <<  6 << 20 << 2012 <<  6 << 30 << 2;
    QTest::newRow("data7")  << 2010 <<  6 << 20 << 2012 <<  6 << 10 << 1;
    QTest::newRow("data8")  << 2000 <<  2 << 29 << 2001 <<  2 << 28 << 1;
    QTest::newRow("data9")  <<   -1 << 12 << 31 <<    1 <<  1 <<  1 << 0;
    QTest::newRow("data10") <<   -1 <<  1 <<  1 <<    1 <<  1 <<  1 << 1;
    QTest::newRow("data11") << -100 <<  1 <<  1 << -100 <<  1 <<  1 << 0;
    QTest::newRow("data12") << -100 <<  1 <<  1 << -100 << 12 << 31 << 0;
    QTest::newRow("data13") << -100 <<  6 <<  1 << -100 <<  7 <<  1 << 0;
    QTest::newRow("data14") << -100 <<  6 <<  1 <<  -99 <<  5 <<  1 << 0;
    QTest::newRow("data15") << -100 <<  6 <<  1 <<  -99 <<  7 <<  1 << 1;
    QTest::newRow("data16") << -100 <<  6 << 20 <<  -98 <<  6 << 20 << 2;
    QTest::newRow("data17") << -100 <<  6 << 20 <<  -98 <<  6 << 30 << 2;
    QTest::newRow("data18") << -100 <<  6 << 20 <<  -98 <<  6 << 10 << 1;
    QTest::newRow("data19") <<   -5 <<  2 << 29 <<   -4 <<  2 << 28 << 1;
}

void tst_QCalendarSystem::yearsDifference()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QFETCH(int, y1);
    QFETCH(int, m1);
    QFETCH(int, d1);
    QFETCH(int, y2);
    QFETCH(int, m2);
    QFETCH(int, d2);
    QFETCH(int, dy);

    QDate dt1 = cal.date(y1, m1, d1);
    QDate dt2 = cal.date(y2, m2, d2);

    int y;
    y = cal.yearsDifference(dt1, dt2);
    QCOMPARE(y, dy);

    y = cal.yearsDifference(dt2, dt1);
    QCOMPARE(y, -dy);
}

void tst_QCalendarSystem::monthsDifference_data()
{
    QTest::addColumn<int>("y1");
    QTest::addColumn<int>("m1");
    QTest::addColumn<int>("d1");
    QTest::addColumn<int>("y2");
    QTest::addColumn<int>("m2");
    QTest::addColumn<int>("d2");
    QTest::addColumn<int>("dm");

    QTest::newRow("data1")  << 2010 <<  1 <<  1 << 2010 <<  1 <<  1 <<  0;
    QTest::newRow("data2")  << 2010 <<  1 <<  1 << 2010 <<  2 <<  1 <<  1;
    QTest::newRow("data3")  << 2010 <<  1 << 10 << 2010 <<  2 <<  1 <<  0;
    QTest::newRow("data4")  << 2010 <<  1 << 10 << 2010 <<  2 << 20 <<  1;
    QTest::newRow("data5")  << 2010 <<  1 <<  1 << 2011 <<  1 <<  1 << 12;
    QTest::newRow("data6")  << 2010 <<  1 <<  1 << 2011 <<  2 <<  1 << 13;
    QTest::newRow("data7")  << 2010 <<  1 << 10 << 2011 <<  2 <<  1 << 12;
    QTest::newRow("data8")  << 2010 <<  1 << 10 << 2011 <<  2 << 20 << 13;
    QTest::newRow("data9")  << 2000 <<  2 << 29 << 2001 <<  2 << 27 << 11;
    QTest::newRow("data10") << 2000 <<  2 << 29 << 2001 <<  2 << 28 << 12;
    QTest::newRow("data11") << 2000 <<  2 << 27 << 2001 <<  2 << 28 << 12;
    QTest::newRow("data12") << 2000 <<  2 << 29 << 2001 <<  3 <<  1 << 12;
    QTest::newRow("data13") <<   -1 << 12 << 31 <<    1 <<  1 <<  1 <<  0;
    QTest::newRow("data14") <<   -1 << 12 <<  1 <<    1 <<  1 <<  1 <<  1;
    QTest::newRow("data15") << -100 <<  1 <<  1 << -100 <<  1 <<  1 <<  0;
    QTest::newRow("data16") << -100 <<  1 <<  1 << -100 <<  2 <<  1 <<  1;
    QTest::newRow("data17") << -100 <<  1 << 10 << -100 <<  2 <<  1 <<  0;
    QTest::newRow("data18") << -100 <<  1 << 10 << -100 <<  2 << 20 <<  1;
    QTest::newRow("data19") << -100 <<  1 <<  1 <<  -99 <<  1 <<  1 << 12;
    QTest::newRow("data20") << -100 <<  1 <<  1 <<  -99 <<  2 <<  1 << 13;
    QTest::newRow("data21") << -100 <<  1 << 10 <<  -99 <<  2 <<  1 << 12;
    QTest::newRow("data22") << -100 <<  1 << 10 <<  -99 <<  2 << 20 << 13;
    QTest::newRow("data23") <<   -5 <<  2 << 29 <<   -4 <<  2 << 27 << 11;
    QTest::newRow("data24") <<   -5 <<  2 << 29 <<   -4 <<  2 << 28 << 12;
    QTest::newRow("data25") <<   -5 <<  2 << 27 <<   -4 <<  2 << 28 << 12;
    QTest::newRow("data26") <<   -5 <<  2 << 29 <<   -4 <<  3 <<  1 << 12;
}

void tst_QCalendarSystem::monthsDifference()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QFETCH(int, y1);
    QFETCH(int, m1);
    QFETCH(int, d1);
    QFETCH(int, y2);
    QFETCH(int, m2);
    QFETCH(int, d2);
    QFETCH(int, dm);

    QDate dt1 = cal.date(y1, m1, d1);
    QDate dt2 = cal.date(y2, m2, d2);

    int m;
    m = cal.monthsDifference(dt1, dt2);
    QCOMPARE(m, dm);

    m = cal.monthsDifference(dt2, dt1);
    QCOMPARE(m, -dm);
}

void tst_QCalendarSystem::daysDifference_data()
{
    QTest::addColumn<int>("y1");
    QTest::addColumn<int>("m1");
    QTest::addColumn<int>("d1");
    QTest::addColumn<int>("y2");
    QTest::addColumn<int>("m2");
    QTest::addColumn<int>("d2");
    QTest::addColumn<int>("dd");

    QTest::newRow("data1") << 2010 <<  1 <<  1 << 2010 <<  1 <<  1 <<  0;
    QTest::newRow("data2") << 2010 <<  1 <<  1 << 2010 <<  1 <<  2 <<  1;
    QTest::newRow("data3") <<   -1 << 12 << 31 <<    1 <<  1 <<  1 <<  1;
    QTest::newRow("data4") <<   -1 <<  1 <<  1 <<   -1 <<  1 <<  1 <<  0;
    QTest::newRow("data5") <<   -1 <<  1 <<  1 <<   -1 <<  1 <<  2 <<  1;
}

void tst_QCalendarSystem::daysDifference()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QFETCH(int, y1);
    QFETCH(int, m1);
    QFETCH(int, d1);
    QFETCH(int, y2);
    QFETCH(int, m2);
    QFETCH(int, d2);
    QFETCH(int, dd);

    QDate dt1 = cal.date(y1, m1, d1);
    QDate dt2 = cal.date(y2, m2, d2);

    int d;
    d = cal.daysDifference(dt1, dt2);
    QCOMPARE(d, dd);

    d = cal.daysDifference(dt2, dt1);
    QCOMPARE(d, -dd);
}

void tst_QCalendarSystem::dateDifference_data()
{
    QTest::addColumn<int>("y1");
    QTest::addColumn<int>("m1");
    QTest::addColumn<int>("d1");
    QTest::addColumn<int>("y2");
    QTest::addColumn<int>("m2");
    QTest::addColumn<int>("d2");
    QTest::addColumn<int>("dy");
    QTest::addColumn<int>("dm");
    QTest::addColumn<int>("dd");

    QTest::newRow("data0")  << 2010 << 6 <<  1 << 2010 << 6 <<  1 << 0 <<  0 <<  0;
    QTest::newRow("data1")  << 2010 << 6 << 10 << 2010 << 6 << 20 << 0 <<  0 << 10;
    QTest::newRow("data2")  << 2010 << 6 <<  1 << 2010 << 7 <<  1 << 0 <<  1 <<  0;
    QTest::newRow("data3")  << 2010 << 6 << 10 << 2010 << 7 << 20 << 0 <<  1 << 10;
    QTest::newRow("data4")  << 2010 << 6 << 20 << 2010 << 7 << 10 << 0 <<  0 << 20;
    QTest::newRow("data5")  << 2010 << 6 << 30 << 2010 << 7 << 31 << 0 <<  1 <<  1;
    QTest::newRow("data6")  << 2010 << 8 << 31 << 2010 << 9 << 30 << 0 <<  1 <<  0;
    QTest::newRow("data7")  << 2010 << 7 <<  1 << 2011 << 6 <<  1 << 0 << 11 <<  0;
    QTest::newRow("data8")  << 2010 << 7 << 10 << 2011 << 6 << 20 << 0 << 11 << 10;
    QTest::newRow("data9")  << 2010 << 7 << 20 << 2011 << 6 << 10 << 0 << 10 << 21;
    QTest::newRow("data10") << 2010 << 7 << 31 << 2011 << 6 << 30 << 0 << 11 <<  0;
    QTest::newRow("data11") << 2010 << 8 << 30 << 2011 << 7 << 31 << 0 << 11 <<  1;
    QTest::newRow("data12") << 2010 << 6 << 10 << 2011 << 6 << 10 << 1 <<  0 <<  0;
    QTest::newRow("data13") << 2010 << 6 << 10 << 2011 << 6 << 20 << 1 <<  0 << 10;
    QTest::newRow("data14") << 2010 << 6 << 10 << 2011 << 7 << 10 << 1 <<  1 <<  0;
    QTest::newRow("data15") << 2010 << 6 << 10 << 2011 << 7 << 20 << 1 <<  1 << 10;
    QTest::newRow("data16") << 2010 << 6 << 20 << 2011 << 8 << 10 << 1 <<  1 << 21;
    QTest::newRow("data17") << 2010 << 6 << 30 << 2011 << 7 << 31 << 1 <<  1 <<  1;
    QTest::newRow("data18") << 2010 << 8 << 31 << 2011 << 9 << 30 << 1 <<  1 <<  0;
    QTest::newRow("data19") << 2010 << 7 << 31 << 2012 << 6 << 30 << 1 << 11 <<  0;
    QTest::newRow("data20") << 2010 << 8 << 30 << 2012 << 7 << 31 << 1 << 11 <<  1;
    QTest::newRow("data21") << 2000 << 2 << 29 << 2001 << 2 << 28 << 1 <<  0 <<  0;
    QTest::newRow("data22") << 2000 << 2 << 29 << 2001 << 3 <<  1 << 1 <<  0 <<  1;
    QTest::newRow("data23") << 2000 << 2 << 29 << 2001 << 4 <<  1 << 1 <<  1 <<  3;
    QTest::newRow("data24") <<   -1 << 1 <<  1 <<    1 << 1 <<  1 << 1 <<  0 <<  0;
    QTest::newRow("data25") << -101 << 6 <<  1 << -101 << 6 <<  1 << 0 <<  0 <<  0;
    QTest::newRow("data26") << -101 << 6 << 10 << -101 << 6 << 20 << 0 <<  0 << 10;
    QTest::newRow("data27") << -101 << 6 <<  1 << -101 << 7 <<  1 << 0 <<  1 <<  0;
    QTest::newRow("data28") << -101 << 6 << 10 << -101 << 7 << 20 << 0 <<  1 << 10;
    QTest::newRow("data29") << -101 << 6 << 20 << -101 << 7 << 10 << 0 <<  0 << 20;
    QTest::newRow("data30") << -101 << 6 << 30 << -101 << 7 << 31 << 0 <<  1 <<  1;
    QTest::newRow("data31") << -101 << 8 << 31 << -101 << 9 << 30 << 0 <<  1 <<  0;
    QTest::newRow("data32") << -101 << 7 <<  1 << -100 << 6 <<  1 << 0 << 11 <<  0;
    QTest::newRow("data33") << -101 << 7 << 10 << -100 << 6 << 20 << 0 << 11 << 10;
    QTest::newRow("data34") << -101 << 7 << 20 << -100 << 6 << 10 << 0 << 10 << 21;
    QTest::newRow("data35") << -101 << 7 << 31 << -100 << 6 << 30 << 0 << 11 <<  0;
    QTest::newRow("data36") << -101 << 8 << 30 << -100 << 7 << 31 << 0 << 11 <<  1;
    QTest::newRow("data37") << -101 << 6 << 10 << -100 << 6 << 10 << 1 <<  0 <<  0;
    QTest::newRow("data38") << -101 << 6 << 10 << -100 << 6 << 20 << 1 <<  0 << 10;
    QTest::newRow("data39") << -101 << 6 << 10 << -100 << 7 << 10 << 1 <<  1 <<  0;
    QTest::newRow("data40") << -101 << 6 << 10 << -100 << 7 << 20 << 1 <<  1 << 10;
    QTest::newRow("data41") << -101 << 6 << 20 << -100 << 8 << 10 << 1 <<  1 << 21;
    QTest::newRow("data42") << -101 << 6 << 30 << -100 << 7 << 31 << 1 <<  1 <<  1;
    QTest::newRow("data43") << -101 << 8 << 31 << -100 << 9 << 30 << 1 <<  1 <<  0;
    QTest::newRow("data44") << -101 << 7 << 31 <<  -99 << 6 << 30 << 1 << 11 <<  0;
    QTest::newRow("data45") << -101 << 8 << 30 <<  -99 << 7 << 31 << 1 << 11 <<  1;
    QTest::newRow("data46") <<   -5 << 2 << 29 <<   -4 << 2 << 28 << 1 <<  0 <<  0;
    QTest::newRow("data47") <<   -5 << 2 << 29 <<   -4 << 3 <<  1 << 1 <<  0 <<  1;
    QTest::newRow("data48") <<   -5 << 2 << 29 <<   -4 << 4 <<  1 << 1 <<  1 <<  3;
}

void tst_QCalendarSystem::dateDifference()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QFETCH(int, y1);
    QFETCH(int, m1);
    QFETCH(int, d1);
    QFETCH(int, y2);
    QFETCH(int, m2);
    QFETCH(int, d2);
    QFETCH(int, dy);
    QFETCH(int, dm);
    QFETCH(int, dd);

    QDate dt1 = cal.date(y1, m1, d1);
    QDate dt2 = cal.date(y2, m2, d2);
    int sign = (dt1 <= dt2 ? 1 : -1);

    int y, m, d, s;
    cal.dateDifference(dt1, dt2, &y, &m, &d, &s);
    QCOMPARE(y, dy);
    QCOMPARE(m, dm);
    QCOMPARE(d, dd);
    QCOMPARE(s, sign);

    sign = (dt1 >= dt2 ? 1 : -1);
    cal.dateDifference(dt2, dt1, &y, &m, &d, &s);
    QCOMPARE(y, dy);
    QCOMPARE(m, dm);
    QCOMPARE(d, dd);
    QCOMPARE(s, sign);
}

void tst_QCalendarSystem::firstDayOfYear()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QDate testDate = cal.date(2010, 6, 1);
    QDate result = cal.date(2010, 1, 1);
    QCOMPARE(cal.firstDayOfYear(testDate), result);
    QCOMPARE(cal.firstDayOfYear(2010), result);

    testDate = cal.date(2000, 6, 1);
    result = cal.date(2000, 1, 1);
    QCOMPARE(cal.firstDayOfYear(testDate), result);
    QCOMPARE(cal.firstDayOfYear(2000), result);
}

void tst_QCalendarSystem::lastDayOfYear()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QDate testDate = cal.date(2010, 6, 1);
    QDate result = cal.date(2010, 12, 31);
    QCOMPARE(cal.lastDayOfYear(testDate), result);
    QCOMPARE(cal.lastDayOfYear(2010), result);

    testDate = cal.date(2000, 6, 1);
    result = cal.date(2000, 12, 31);
    QCOMPARE(cal.lastDayOfYear(testDate), result);
    QCOMPARE(cal.lastDayOfYear(2000), result);
}

void tst_QCalendarSystem::firstDayOfMonth()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QDate testDate = cal.date(2010, 2, 15);
    QDate result = cal.date(2010, 2, 1);
    QCOMPARE(cal.firstDayOfMonth(testDate), result);
    QCOMPARE(cal.firstDayOfMonth(2010, 2), result);

    testDate = cal.date(2000, 2, 15);
    result = cal.date(2000, 2, 1);
    QCOMPARE(cal.firstDayOfMonth(testDate), result);
    QCOMPARE(cal.firstDayOfMonth(2000, 2), result);
}

void tst_QCalendarSystem::lastDayOfMonth()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QDate testDate = cal.date(2010, 2, 15);
    QDate result = cal.date(2010, 2, 28);
    QCOMPARE(cal.lastDayOfMonth(testDate), result);
    QCOMPARE(cal.lastDayOfMonth(2010, 2), result);

    testDate = cal.date(2000, 2, 15);
    result = cal.date(2000, 2, 29);
    QCOMPARE(cal.lastDayOfMonth(testDate), result);
    QCOMPARE(cal.lastDayOfMonth(2000, 2), result);
}

// Utility Test Methods

void tst_QCalendarSystem::testDate(QLocale::CalendarSystem calSys, int year, int month, int day, qint64 jd)
{
    QDateCalculator cal(calSys);

    QDate date = cal.date(year, month, day);
    QCOMPARE(date, QDate::fromJulianDay(jd));
    int y, m, d;
    cal.ymd(date, &y, &m, &d);
    QCOMPARE(y, year);
    QCOMPARE(m, month);
    QCOMPARE(d, day);
}

void tst_QCalendarSystem::testYears(QLocale::CalendarSystem calSys, int year, int miy, int diy, bool isLeap, bool isValid)
{
    QDateCalculator cal(calSys);

    QCOMPARE(cal.isLeapYear(cal.date(year, 1, 1)), isLeap);
    QCOMPARE(cal.isLeapYear(year), isLeap);
    QCOMPARE(cal.isLeapYear(year), cal.isValid(year, 2, 29));
    QCOMPARE(cal.monthsInYear(year), miy);
    QCOMPARE(cal.daysInYear(year), diy);
    QCOMPARE(cal.isValid(year, 1), isValid);
    QCOMPARE(cal.isValid(year, diy), isValid);
    QCOMPARE(cal.isValid(year, 0), false);
    QCOMPARE(cal.isValid(year, diy + 1), false);
}

void tst_QCalendarSystem::testMonths(QLocale::CalendarSystem calSys, int year, int month, int dim, bool isValid)
{
    QDateCalculator cal(calSys);

    QCOMPARE(cal.daysInMonth(cal.date(year, month, 1)), dim);
    QCOMPARE(cal.daysInMonth(year, month), dim);
    QCOMPARE(cal.isValid(year, month, 1), isValid);
    QCOMPARE(cal.isValid(year, month, dim), isValid);
    QCOMPARE(cal.isValid(year, month, dim + 1), false);
    QCOMPARE(cal.isValid(year, month, 0), false);
}

void tst_QCalendarSystem::roundtrip(QLocale::CalendarSystem calSys, QDate startDate, QDate endDate)
{
    // Test round trip, this exercises date(), ymd(), isValid(), isLeapYear(),
    // julianDayToDate(), and julianDayFromDate() to ensure they are internally
    // consistent (but doesn't guarantee correct)

    QDateCalculator cal(calSys);
    QCOMPARE(cal.isValid(startDate), true);
    QCOMPARE(cal.isValid(endDate), true);
    QDate testDate;
    QDate loopDate = startDate;
    int y, m, d;
    while (loopDate <= endDate) {
        QCOMPARE(cal.isValid(loopDate), true);
        cal.ymd(loopDate, &y, &m, &d);
        testDate = cal.date(y, m, d);
        QCOMPARE(cal.isValid(testDate), true);
        QCOMPARE(loopDate, testDate);
        loopDate = loopDate.addDays(1);
    }
}


// Gregorian Calendar specific tests

void tst_QCalendarSystem::gregorianCalendar()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    QCOMPARE(cal.calendarSystem(), QLocale::GregorianCalendar);
    QCOMPARE(cal.epoch(), QDate::fromJulianDay(1721426));
    QCOMPARE(cal.epoch(), cal.date(1, 1, 1));
    QCOMPARE(cal.earliestValidDate(), QDate::fromJulianDay(-31738));
    QCOMPARE(cal.earliestValidDate(), cal.date(-4800, 1, 1));
    QCOMPARE(cal.latestValidDate(), QDate::fromJulianDay(5373484));
    QCOMPARE(cal.latestValidDate(), cal.date(9999, 12, 31));
    QCOMPARE(cal.maximumMonthsInYear(), 12);
    QCOMPARE(cal.maximumDaysInYear(),  366);
    QCOMPARE(cal.maximumDaysInMonth(),  31);
}

void tst_QCalendarSystem::gregorianRoundtrip()
{
    QDateCalculator cal(QLocale::GregorianCalendar);

    // Test first 200 valid years, and -ve to +ve jd change
    QDate startDate = cal.date(-4800, 1, 1);
    QDate endDate = cal.date(-4600, 12, 31);
    roundtrip(QLocale::GregorianCalendar, startDate, endDate);
    // Test 100 years either side of year 1
    startDate = cal.date(-100, 1, 1);
    endDate = cal.date(100, 12, 31);
    roundtrip(QLocale::GregorianCalendar, startDate, endDate);
    // Test 200 years either side of year 2000
    startDate = cal.date(1800, 1, 1);
    endDate = cal.date(2200, 12, 31);
    roundtrip(QLocale::GregorianCalendar, startDate, endDate);
    // Test last 200 valid years
    startDate = cal.date(9800, 1, 1);
    endDate = cal.date(9999, 12, 31);
    roundtrip(QLocale::GregorianCalendar, startDate, endDate);
}

void tst_QCalendarSystem::gregorianDate_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("day");
    QTest::addColumn<qint64>("jd");

    // Note: When adding new dates, the jd must be obtained from an independent implementation

    // Some key dates:
    QTest::newRow("data0")   << -4800 <<  1 <<  1 << qint64( -31738);
    QTest::newRow("data1")   << -4800 << 12 << 31 << qint64( -31374);
    QTest::newRow("data2")   << -4799 <<  1 <<  1 << qint64( -31373);
    QTest::newRow("data3")   << -4799 << 12 << 31 << qint64( -31009);
    QTest::newRow("data4")   << -4714 << 11 << 23 << qint64(     -1);
    QTest::newRow("data5")   << -4714 << 11 << 24 << qint64(      0);
    QTest::newRow("data6")   << -4714 << 11 << 25 << qint64(      1);
    QTest::newRow("data7")   <<    -1 <<  1 <<  1 << qint64(1721060);
    QTest::newRow("data8")   <<    -1 << 12 << 31 << qint64(1721425);
    QTest::newRow("data9")   <<     1 <<  1 <<  1 << qint64(1721426);
    QTest::newRow("data10")  <<     1 << 12 << 31 << qint64(1721790);
    QTest::newRow("data11")  <<  9998 <<  1 <<  1 << qint64(5372755);
    QTest::newRow("data12")  <<  9998 << 12 << 31 << qint64(5373119);
    QTest::newRow("data13")  <<  9999 <<  1 <<  1 << qint64(5373120);
    QTest::newRow("data14")  <<  9999 << 12 << 31 << qint64(5373484);

    // Key dates in a leap year, poitive jd
    QTest::newRow("data15")  <<  2000 <<  1 <<  1 << qint64(2451545);
    QTest::newRow("data16")  <<  2000 <<  1 << 31 << qint64(2451575);
    QTest::newRow("data17")  <<  2000 <<  2 <<  1 << qint64(2451576);
    QTest::newRow("data18")  <<  2000 <<  2 << 29 << qint64(2451604);
    QTest::newRow("data19")  <<  2000 <<  3 <<  1 << qint64(2451605);
    QTest::newRow("data20")  <<  2000 <<  3 << 31 << qint64(2451635);
    QTest::newRow("data21")  <<  2000 <<  4 <<  1 << qint64(2451636);
    QTest::newRow("data22")  <<  2000 <<  4 << 30 << qint64(2451665);
    QTest::newRow("data23")  <<  2000 <<  5 <<  1 << qint64(2451666);
    QTest::newRow("data24")  <<  2000 <<  5 << 31 << qint64(2451696);
    QTest::newRow("data25")  <<  2000 <<  6 <<  1 << qint64(2451697);
    QTest::newRow("data26")  <<  2000 <<  6 << 30 << qint64(2451726);
    QTest::newRow("data27")  <<  2000 <<  7 <<  1 << qint64(2451727);
    QTest::newRow("data28")  <<  2000 <<  7 << 31 << qint64(2451757);
    QTest::newRow("data29")  <<  2000 <<  8 <<  1 << qint64(2451758);
    QTest::newRow("data30")  <<  2000 <<  8 << 31 << qint64(2451788);
    QTest::newRow("data31")  <<  2000 <<  9 <<  1 << qint64(2451789);
    QTest::newRow("data32")  <<  2000 <<  9 << 30 << qint64(2451818);
    QTest::newRow("data33")  <<  2000 << 10 <<  1 << qint64(2451819);
    QTest::newRow("data34")  <<  2000 << 10 << 31 << qint64(2451849);
    QTest::newRow("data35")  <<  2000 << 11 <<  1 << qint64(2451850);
    QTest::newRow("data36")  <<  2000 << 11 << 30 << qint64(2451879);
    QTest::newRow("data37")  <<  2000 << 12 <<  1 << qint64(2451880);
    QTest::newRow("data38")  <<  2000 << 12 << 31 << qint64(2451910);

    // Key dates in a non leap year, positive jd
    QTest::newRow("data39")  <<  2010 <<  1 <<  1 << qint64(2455198);
    QTest::newRow("data40")  <<  2010 <<  1 << 31 << qint64(2455228);
    QTest::newRow("data41")  <<  2010 <<  2 <<  1 << qint64(2455229);
    QTest::newRow("data42")  <<  2010 <<  2 << 28 << qint64(2455256);
    QTest::newRow("data43")  <<  2010 <<  3 <<  1 << qint64(2455257);
    QTest::newRow("data44")  <<  2010 <<  3 << 31 << qint64(2455287);
    QTest::newRow("data45")  <<  2010 <<  4 <<  1 << qint64(2455288);
    QTest::newRow("data46")  <<  2010 <<  4 << 30 << qint64(2455317);
    QTest::newRow("data47")  <<  2010 <<  5 <<  1 << qint64(2455318);
    QTest::newRow("data48")  <<  2010 <<  5 << 31 << qint64(2455348);
    QTest::newRow("data49")  <<  2010 <<  6 <<  1 << qint64(2455349);
    QTest::newRow("data50")  <<  2010 <<  6 << 30 << qint64(2455378);
    QTest::newRow("data51")  <<  2010 <<  7 <<  1 << qint64(2455379);
    QTest::newRow("data52")  <<  2010 <<  7 << 31 << qint64(2455409);
    QTest::newRow("data53")  <<  2010 <<  8 <<  1 << qint64(2455410);
    QTest::newRow("data54")  <<  2010 <<  8 << 31 << qint64(2455440);
    QTest::newRow("data55")  <<  2010 <<  9 <<  1 << qint64(2455441);
    QTest::newRow("data56")  <<  2010 <<  9 << 30 << qint64(2455470);
    QTest::newRow("data57")  <<  2010 << 10 <<  1 << qint64(2455471);
    QTest::newRow("data58")  <<  2010 << 10 << 31 << qint64(2455501);
    QTest::newRow("data59")  <<  2010 << 11 <<  1 << qint64(2455502);
    QTest::newRow("data60")  <<  2010 << 11 << 30 << qint64(2455531);
    QTest::newRow("data61")  <<  2010 << 12 <<  1 << qint64(2455532);
    QTest::newRow("data62")  <<  2010 << 12 << 31 << qint64(2455562);

    // Key dates in a leap year, negative jd
    QTest::newRow("data63")  << -4781 <<  1 <<  1 << qint64(-24799);
    QTest::newRow("data64")  << -4781 <<  1 << 31 << qint64(-24769);
    QTest::newRow("data65")  << -4781 <<  2 <<  1 << qint64(-24768);
    QTest::newRow("data66")  << -4781 <<  2 << 29 << qint64(-24740);
    QTest::newRow("data67")  << -4781 <<  3 <<  1 << qint64(-24739);
    QTest::newRow("data68")  << -4781 <<  3 << 31 << qint64(-24709);
    QTest::newRow("data69")  << -4781 <<  4 <<  1 << qint64(-24708);
    QTest::newRow("data70")  << -4781 <<  4 << 30 << qint64(-24679);
    QTest::newRow("data71")  << -4781 <<  5 <<  1 << qint64(-24678);
    QTest::newRow("data72")  << -4781 <<  5 << 31 << qint64(-24648);
    QTest::newRow("data73")  << -4781 <<  6 <<  1 << qint64(-24647);
    QTest::newRow("data74")  << -4781 <<  6 << 30 << qint64(-24618);
    QTest::newRow("data75")  << -4781 <<  7 <<  1 << qint64(-24617);
    QTest::newRow("data76")  << -4781 <<  7 << 31 << qint64(-24587);
    QTest::newRow("data77")  << -4781 <<  8 <<  1 << qint64(-24586);
    QTest::newRow("data78")  << -4781 <<  8 << 31 << qint64(-24556);
    QTest::newRow("data79")  << -4781 <<  9 <<  1 << qint64(-24555);
    QTest::newRow("data80")  << -4781 <<  9 << 30 << qint64(-24526);
    QTest::newRow("data81")  << -4781 << 10 <<  1 << qint64(-24525);
    QTest::newRow("data82")  << -4781 << 10 << 31 << qint64(-24495);
    QTest::newRow("data83")  << -4781 << 11 <<  1 << qint64(-24494);
    QTest::newRow("data84")  << -4781 << 11 << 30 << qint64(-24465);
    QTest::newRow("data85")  << -4781 << 12 <<  1 << qint64(-24464);
    QTest::newRow("data86")  << -4781 << 12 << 31 << qint64(-24434);

    // Key dates in a non leap year, negative jd
    QTest::newRow("data87")  << -4780 <<  1 <<  1 << qint64(-24433);
    QTest::newRow("data88")  << -4780 <<  1 << 31 << qint64(-24403);
    QTest::newRow("data89")  << -4780 <<  2 <<  1 << qint64(-24402);
    QTest::newRow("data90")  << -4780 <<  2 << 28 << qint64(-24375);
    QTest::newRow("data91")  << -4780 <<  3 <<  1 << qint64(-24374);
    QTest::newRow("data92")  << -4780 <<  3 << 31 << qint64(-24344);
    QTest::newRow("data93")  << -4780 <<  4 <<  1 << qint64(-24343);
    QTest::newRow("data94")  << -4780 <<  4 << 30 << qint64(-24314);
    QTest::newRow("data95")  << -4780 <<  5 <<  1 << qint64(-24313);
    QTest::newRow("data96")  << -4780 <<  5 << 31 << qint64(-24283);
    QTest::newRow("data97")  << -4780 <<  6 <<  1 << qint64(-24282);
    QTest::newRow("data98")  << -4780 <<  6 << 30 << qint64(-24253);
    QTest::newRow("data99")  << -4780 <<  7 <<  1 << qint64(-24252);
    QTest::newRow("data100") << -4780 <<  7 << 31 << qint64(-24222);
    QTest::newRow("data101") << -4780 <<  8 <<  1 << qint64(-24221);
    QTest::newRow("data102") << -4780 <<  8 << 31 << qint64(-24191);
    QTest::newRow("data103") << -4780 <<  9 <<  1 << qint64(-24190);
    QTest::newRow("data104") << -4780 <<  9 << 30 << qint64(-24161);
    QTest::newRow("data105") << -4780 << 10 <<  1 << qint64(-24160);
    QTest::newRow("data106") << -4780 << 10 << 31 << qint64(-24130);
    QTest::newRow("data107") << -4780 << 11 <<  1 << qint64(-24129);
    QTest::newRow("data108") << -4780 << 11 << 30 << qint64(-24100);
    QTest::newRow("data109") << -4780 << 12 <<  1 << qint64(-24099);
    QTest::newRow("data110") << -4780 << 12 << 31 << qint64(-24069);
}

void tst_QCalendarSystem::gregorianDate()
{
    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, day);
    QFETCH(qint64, jd);

    testDate(QLocale::GregorianCalendar, year, month, day, jd);
}

void tst_QCalendarSystem::gregorianYears_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("miy");
    QTest::addColumn<int>("diy");
    QTest::addColumn<bool>("isLeap");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("data1")  << -2001 << 12 << 366 << true  << true;
    QTest::newRow("data2")  << -2000 << 12 << 365 << false << true;
    QTest::newRow("data3")  << -1901 << 12 << 365 << false << true;
    QTest::newRow("data4")  << -1900 << 12 << 365 << false << true;
    QTest::newRow("data5")  <<    -6 << 12 << 365 << false << true;
    QTest::newRow("data6")  <<    -5 << 12 << 366 << true  << true;
    QTest::newRow("data7")  <<    -4 << 12 << 365 << false << true;
    QTest::newRow("data8")  <<    -3 << 12 << 365 << false << true;
    QTest::newRow("data9")  <<    -2 << 12 << 365 << false << true;
    QTest::newRow("data10") <<    -1 << 12 << 366 << true  << true;
    QTest::newRow("data11") <<     0 <<  0 <<   0 << false << false; // Doesn't exist
    QTest::newRow("data12") <<     1 << 12 << 365 << false << true;
    QTest::newRow("data13") <<     2 << 12 << 365 << false << true;
    QTest::newRow("data14") <<     3 << 12 << 365 << false << true;
    QTest::newRow("data15") <<     4 << 12 << 366 << true  << true;
    QTest::newRow("data16") <<     7 << 12 << 365 << false << true;
    QTest::newRow("data17") <<     8 << 12 << 366 << true  << true;
    QTest::newRow("data18") <<   100 << 12 << 365 << false << true;
    QTest::newRow("data19") <<   400 << 12 << 366 << true  << true;
    QTest::newRow("data20") <<   700 << 12 << 365 << false << true;
    QTest::newRow("data21") <<  1500 << 12 << 365 << false << true;
    QTest::newRow("data22") <<  1600 << 12 << 366 << true  << true;
    QTest::newRow("data23") <<  1700 << 12 << 365 << false << true;
    QTest::newRow("data24") <<  1800 << 12 << 365 << false << true;
    QTest::newRow("data25") <<  1900 << 12 << 365 << false << true;
    QTest::newRow("data26") <<  2000 << 12 << 366 << true  << true;
    QTest::newRow("data27") <<  2100 << 12 << 365 << false << true;
    QTest::newRow("data28") <<  2200 << 12 << 365 << false << true;
    QTest::newRow("data29") <<  2300 << 12 << 365 << false << true;
    QTest::newRow("data30") <<  2400 << 12 << 366 << true  << true;
}

void tst_QCalendarSystem::gregorianYears()
{
    QFETCH(int, year);
    QFETCH(int, miy);
    QFETCH(int, diy);
    QFETCH(bool, isLeap);
    QFETCH(bool, isValid);

    testYears(QLocale::GregorianCalendar, year, miy, diy, isLeap, isValid);
}

void tst_QCalendarSystem::gregorianMonths_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("dim");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("data1")  << 2000 <<  0 <<  0 << false;
    QTest::newRow("data2")  << 2000 <<  1 << 31 << true;
    QTest::newRow("data3")  << 2000 <<  2 << 29 << true;
    QTest::newRow("data4")  << 2001 <<  2 << 28 << true;
    QTest::newRow("data5")  << 2000 <<  3 << 31 << true;
    QTest::newRow("data6")  << 2000 <<  4 << 30 << true;
    QTest::newRow("data7")  << 2000 <<  5 << 31 << true;
    QTest::newRow("data8")  << 2000 <<  6 << 30 << true;
    QTest::newRow("data9")  << 2000 <<  7 << 31 << true;
    QTest::newRow("data10") << 2000 <<  8 << 31 << true;
    QTest::newRow("data11") << 2000 <<  9 << 30 << true;
    QTest::newRow("data12") << 2000 << 10 << 31 << true;
    QTest::newRow("data13") << 2000 << 11 << 30 << true;
    QTest::newRow("data14") << 2000 << 12 << 31 << true;
    QTest::newRow("data15") << 2000 << 13 <<  0 << false;
}

void tst_QCalendarSystem::gregorianMonths()
{
    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, dim);
    QFETCH(bool, isValid);

    testMonths(QLocale::GregorianCalendar, year, month, dim, isValid);
}


// ISO8601 Calendar specific tests
// Is simply Gregorian with year 0 valid and year range of 0000..9999

void tst_QCalendarSystem::iso8601Calendar()
{
    QDateCalculator cal(QLocale::ISO8601Calendar);

    QCOMPARE(cal.calendarSystem(), QLocale::ISO8601Calendar);
    QCOMPARE(cal.epoch(), QDate::fromJulianDay(1721060));
    QCOMPARE(cal.epoch(), cal.date(0, 1, 1));
    QCOMPARE(cal.earliestValidDate(), QDate::fromJulianDay(1721060));
    QCOMPARE(cal.earliestValidDate(), cal.date(0, 1, 1));
    QCOMPARE(cal.latestValidDate(), QDate::fromJulianDay(5373484));
    QCOMPARE(cal.latestValidDate(), cal.date(9999, 12, 31));
    QCOMPARE(cal.maximumMonthsInYear(), 12);
    QCOMPARE(cal.maximumDaysInYear(),  366);
    QCOMPARE(cal.maximumDaysInMonth(),  31);
}

void tst_QCalendarSystem::iso8601Roundtrip()
{
    QDateCalculator cal(QLocale::ISO8601Calendar);

    // Test first 200 valid years
    QDate startDate = cal.date(0, 1, 1);
    QDate endDate = cal.date(100, 12, 31);
    roundtrip(QLocale::ISO8601Calendar, startDate, endDate);
    // Test 200 years either side of year 2000
    startDate = cal.date(1800, 1, 1);
    endDate = cal.date(2200, 12, 31);
    roundtrip(QLocale::ISO8601Calendar, startDate, endDate);
    // Test last 200 valid years
    startDate = cal.date(9800, 1, 1);
    endDate = cal.date(9999, 12, 31);
    roundtrip(QLocale::ISO8601Calendar, startDate, endDate);
}

void tst_QCalendarSystem::iso8601Date_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("day");
    QTest::addColumn<qint64>("jd");

    // Note: When adding new dates, the jd must be obtained from an independent implementation

    // Some key dates:
    QTest::newRow("data1") <<     0 <<  1 <<  1 << qint64(1721060);
    QTest::newRow("data2") <<     0 << 12 << 31 << qint64(1721425);
    QTest::newRow("data3") <<     1 <<  1 <<  1 << qint64(1721426);
    QTest::newRow("data4") <<     1 << 12 << 31 << qint64(1721790);
    QTest::newRow("data5") <<  9998 <<  1 <<  1 << qint64(5372755);
    QTest::newRow("data6") <<  9998 << 12 << 31 << qint64(5373119);
    QTest::newRow("data7") <<  9999 <<  1 <<  1 << qint64(5373120);
    QTest::newRow("data8") <<  9999 << 12 << 31 << qint64(5373484);
}

void tst_QCalendarSystem::iso8601Date()
{
    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, day);
    QFETCH(qint64, jd);

    testDate(QLocale::ISO8601Calendar, year, month, day, jd);
}

void tst_QCalendarSystem::iso8601Years_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("miy");
    QTest::addColumn<int>("diy");
    QTest::addColumn<bool>("isLeap");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("data1") <<    -1 <<  0 <<   0 << false << false;
    QTest::newRow("data2") <<     0 << 12 << 366 << true << true;
    QTest::newRow("data3") <<     1 << 12 << 365 << false << true;
    QTest::newRow("data4") <<     2 << 12 << 365 << false << true;
    QTest::newRow("data5") <<     3 << 12 << 365 << false << true;
    QTest::newRow("data6") <<     4 << 12 << 366 << true  << true;
    QTest::newRow("data7") <<  2000 << 12 << 366 << true  << true;
    QTest::newRow("data8") <<  2100 << 12 << 365 << false << true;
}

void tst_QCalendarSystem::iso8601Years()
{
    QFETCH(int, year);
    QFETCH(int, miy);
    QFETCH(int, diy);
    QFETCH(bool, isLeap);
    QFETCH(bool, isValid);

    testYears(QLocale::ISO8601Calendar, year, miy, diy, isLeap, isValid);
}

void tst_QCalendarSystem::iso8601Months_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("dim");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("data1")  <<    0 <<  0 <<  0 << false;
    QTest::newRow("data2")  <<    0 <<  1 << 31 << true;
    QTest::newRow("data3")  <<    0 <<  2 << 29 << true;
    QTest::newRow("data5")  <<    0 <<  3 << 31 << true;
    QTest::newRow("data6")  <<    0 <<  4 << 30 << true;
    QTest::newRow("data7")  <<    0 <<  5 << 31 << true;
    QTest::newRow("data8")  <<    0 <<  6 << 30 << true;
    QTest::newRow("data9")  <<    0 <<  7 << 31 << true;
    QTest::newRow("data10") <<    0 <<  8 << 31 << true;
    QTest::newRow("data11") <<    0 <<  9 << 30 << true;
    QTest::newRow("data12") <<    0 << 10 << 31 << true;
    QTest::newRow("data13") <<    0 << 11 << 30 << true;
    QTest::newRow("data14") <<    0 << 12 << 31 << true;
    QTest::newRow("data15") <<    0 << 13 <<  0 << false;
}

void tst_QCalendarSystem::iso8601Months()
{
    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, dim);
    QFETCH(bool, isValid);

    testMonths(QLocale::ISO8601Calendar, year, month, dim, isValid);
}


// Japanese Calendar specific tests
// Is Gregorian with Era Year system

void tst_QCalendarSystem::japaneseCalendar()
{
    QDateCalculator cal(QLocale::JapaneseCalendar);

    QCOMPARE(cal.calendarSystem(), QLocale::JapaneseCalendar);
    QCOMPARE(cal.epoch(), QDate::fromJulianDay(1721426));
    QCOMPARE(cal.epoch(), cal.date(1, 1, 1));
    QCOMPARE(cal.earliestValidDate(), QDate::fromJulianDay(-31738));
    QCOMPARE(cal.earliestValidDate(), cal.date(-4800, 1, 1));
    QCOMPARE(cal.latestValidDate(), QDate::fromJulianDay(5373484));
    QCOMPARE(cal.latestValidDate(), cal.date(9999, 12, 31));
    QCOMPARE(cal.maximumMonthsInYear(), 12);
    QCOMPARE(cal.maximumDaysInYear(),  366);
    QCOMPARE(cal.maximumDaysInMonth(),  31);
}

void tst_QCalendarSystem::japaneseRoundtrip()
{
    QDateCalculator cal(QLocale::JapaneseCalendar);

    // Test first 200 valid years, and -ve to +ve jd change
    QDate startDate = cal.date(-4800, 1, 1);
    QDate endDate = cal.date(-4600, 12, 31);
    roundtrip(QLocale::JapaneseCalendar, startDate, endDate);
    // Test 100 years either side of year 1
    startDate = cal.date(-100, 1, 1);
    endDate = cal.date(100, 12, 31);
    roundtrip(QLocale::JapaneseCalendar, startDate, endDate);
    // Test 200 years either side of year 2000
    startDate = cal.date(1800, 1, 1);
    endDate = cal.date(2200, 12, 31);
    roundtrip(QLocale::JapaneseCalendar, startDate, endDate);
    // Test last 200 valid years
    startDate = cal.date(9800, 1, 1);
    endDate = cal.date(9999, 12, 31);
    roundtrip(QLocale::JapaneseCalendar, startDate, endDate);
}

void tst_QCalendarSystem::japaneseDate_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("day");
    QTest::addColumn<qint64>("jd");

    // Note: When adding new dates, the jd must be obtained from an independent implementation

    // Some key dates:
    QTest::newRow("data0")   << -4800 <<  1 <<  1 << qint64( -31738);
    QTest::newRow("data1")   << -4800 << 12 << 31 << qint64( -31374);
    QTest::newRow("data2")   << -4799 <<  1 <<  1 << qint64( -31373);
    QTest::newRow("data3")   << -4799 << 12 << 31 << qint64( -31009);
    QTest::newRow("data4")   << -4714 << 11 << 23 << qint64(     -1);
    QTest::newRow("data5")   << -4714 << 11 << 24 << qint64(      0);
    QTest::newRow("data6")   << -4714 << 11 << 25 << qint64(      1);
    QTest::newRow("data7")   <<    -1 <<  1 <<  1 << qint64(1721060);
    QTest::newRow("data8")   <<    -1 << 12 << 31 << qint64(1721425);
    QTest::newRow("data9")   <<     1 <<  1 <<  1 << qint64(1721426);
    QTest::newRow("data10")  <<     1 << 12 << 31 << qint64(1721790);
    QTest::newRow("data11")  <<  9998 <<  1 <<  1 << qint64(5372755);
    QTest::newRow("data12")  <<  9998 << 12 << 31 << qint64(5373119);
    QTest::newRow("data13")  <<  9999 <<  1 <<  1 << qint64(5373120);
    QTest::newRow("data14")  <<  9999 << 12 << 31 << qint64(5373484);
}

void tst_QCalendarSystem::japaneseDate()
{
    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, day);
    QFETCH(qint64, jd);

    testDate(QLocale::JapaneseCalendar, year, month, day, jd);
}

void tst_QCalendarSystem::japaneseYears_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("miy");
    QTest::addColumn<int>("diy");
    QTest::addColumn<bool>("isLeap");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("data1") << -2000 << 12 << 365 << false << true;
    QTest::newRow("data2") << -1900 << 12 << 365 << false << true;
    QTest::newRow("data3") <<    -4 << 12 << 365 << false << true;
    QTest::newRow("data4") <<    -1 << 12 << 366 << true  << true;
    QTest::newRow("data5") <<     0 <<  0 <<   0 << false << false; // Doesn't exist
    QTest::newRow("data6") <<     1 << 12 << 365 << false << true;
    QTest::newRow("data7") <<     4 << 12 << 366 << true  << true;
    QTest::newRow("data8") <<  1900 << 12 << 365 << false << true;
    QTest::newRow("data9") <<  2000 << 12 << 366 << true  << true;
}

void tst_QCalendarSystem::japaneseYears()
{
    QFETCH(int, year);
    QFETCH(int, miy);
    QFETCH(int, diy);
    QFETCH(bool, isLeap);
    QFETCH(bool, isValid);

    testYears(QLocale::JapaneseCalendar, year, miy, diy, isLeap, isValid);
}

void tst_QCalendarSystem::japaneseMonths_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("dim");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("data1")  << 2000 <<  0 <<  0 << false;
    QTest::newRow("data2")  << 2000 <<  1 << 31 << true;
    QTest::newRow("data3")  << 2000 <<  2 << 29 << true;
    QTest::newRow("data5")  << 2000 <<  3 << 31 << true;
    QTest::newRow("data6")  << 2000 <<  4 << 30 << true;
    QTest::newRow("data7")  << 2000 <<  5 << 31 << true;
    QTest::newRow("data8")  << 2000 <<  6 << 30 << true;
    QTest::newRow("data9")  << 2000 <<  7 << 31 << true;
    QTest::newRow("data10") << 2000 <<  8 << 31 << true;
    QTest::newRow("data11") << 2000 <<  9 << 30 << true;
    QTest::newRow("data12") << 2000 << 10 << 31 << true;
    QTest::newRow("data13") << 2000 << 11 << 30 << true;
    QTest::newRow("data14") << 2000 << 12 << 31 << true;
    QTest::newRow("data15") << 2000 << 13 <<  0 << false;
}

void tst_QCalendarSystem::japaneseMonths()
{
    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, dim);
    QFETCH(bool, isValid);

    testMonths(QLocale::JapaneseCalendar, year, month, dim, isValid);
}


// Julian Calendar specific tests

void tst_QCalendarSystem::julianCalendar()
{
    QDateCalculator cal(QLocale::JulianCalendar);

    QCOMPARE(cal.calendarSystem(), QLocale::JulianCalendar);
    QCOMPARE(cal.epoch(), QDate::fromJulianDay(1721424));
    QCOMPARE(cal.epoch(), cal.date(1, 1, 1));
    QCOMPARE(cal.earliestValidDate(), QDate::fromJulianDay(-31776));
    QCOMPARE(cal.earliestValidDate(), cal.date(-4800, 1, 1));
    QCOMPARE(cal.latestValidDate(), QDate::fromJulianDay(5373557));
    QCOMPARE(cal.latestValidDate(), cal.date(9999, 12, 31));
    QCOMPARE(cal.maximumMonthsInYear(), 12);
    QCOMPARE(cal.maximumDaysInYear(),  366);
    QCOMPARE(cal.maximumDaysInMonth(),  31);
}

void tst_QCalendarSystem::julianRoundtrip()
{
    QDateCalculator cal(QLocale::JulianCalendar);

    // Test first 200 valid years, and -ve to +ve jd change
    QDate startDate = cal.date(-4800, 1, 1);
    QDate endDate = cal.date(-4600, 12, 31);
    roundtrip(QLocale::JulianCalendar, startDate, endDate);
    // Test 100 years either side of year 1
    startDate = cal.date(-100, 1, 1);
    endDate = cal.date(100, 12, 31);
    roundtrip(QLocale::JulianCalendar, startDate, endDate);
    // Test 200 years either side of year 2000
    startDate = cal.date(1800, 1, 1);
    endDate = cal.date(2200, 12, 31);
    roundtrip(QLocale::JulianCalendar, startDate, endDate);
    // Test last 200 valid years
    startDate = cal.date(9800, 1, 1);
    endDate = cal.date(9999, 12, 31);
    roundtrip(QLocale::JulianCalendar, startDate, endDate);
}

void tst_QCalendarSystem::julianDate_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("day");
    QTest::addColumn<qint64>("jd");

    // Note: When adding new dates, the jd must be obtained from an independent implementation

    // Some key dates:
    QTest::newRow("data0")   << -4800 <<  1 <<  1 << qint64( -31776);
    QTest::newRow("data1")   << -4800 << 12 << 31 << qint64( -31412);
    QTest::newRow("data2")   << -4799 <<  1 <<  1 << qint64( -31411);
    QTest::newRow("data3")   << -4799 << 12 << 31 << qint64( -31047);
    QTest::newRow("data4")   << -4714 << 12 << 31 << qint64(     -1);
    QTest::newRow("data5")   << -4713 <<  1 <<  1 << qint64(      0);
    QTest::newRow("data6")   << -4713 <<  1 <<  2 << qint64(      1);
    QTest::newRow("data7")   <<    -1 <<  1 <<  1 << qint64(1721058);
    QTest::newRow("data8")   <<    -1 << 12 << 31 << qint64(1721423);
    QTest::newRow("data9")   <<     1 <<  1 <<  1 << qint64(1721424);
    QTest::newRow("data10")  <<     1 << 12 << 31 << qint64(1721788);
    QTest::newRow("data11")  <<  9998 <<  1 <<  1 << qint64(5372828);
    QTest::newRow("data12")  <<  9998 << 12 << 31 << qint64(5373192);
    QTest::newRow("data13")  <<  9999 <<  1 <<  1 << qint64(5373193);
    QTest::newRow("data14")  <<  9999 << 12 << 31 << qint64(5373557);

    // Key dates in a leap year, poitive jd
    QTest::newRow("data15")  <<  2012 <<  1 <<  1 << qint64(2455941);
    QTest::newRow("data16")  <<  2012 <<  1 << 31 << qint64(2455971);
    QTest::newRow("data17")  <<  2012 <<  2 <<  1 << qint64(2455972);
    QTest::newRow("data18")  <<  2012 <<  2 << 29 << qint64(2456000);
    QTest::newRow("data19")  <<  2012 <<  3 <<  1 << qint64(2456001);
    QTest::newRow("data20")  <<  2012 <<  3 << 31 << qint64(2456031);
    QTest::newRow("data21")  <<  2012 <<  4 <<  1 << qint64(2456032);
    QTest::newRow("data22")  <<  2012 <<  4 << 30 << qint64(2456061);
    QTest::newRow("data23")  <<  2012 <<  5 <<  1 << qint64(2456062);
    QTest::newRow("data24")  <<  2012 <<  5 << 31 << qint64(2456092);
    QTest::newRow("data25")  <<  2012 <<  6 <<  1 << qint64(2456093);
    QTest::newRow("data26")  <<  2012 <<  6 << 30 << qint64(2456122);
    QTest::newRow("data27")  <<  2012 <<  7 <<  1 << qint64(2456123);
    QTest::newRow("data28")  <<  2012 <<  7 << 31 << qint64(2456153);
    QTest::newRow("data29")  <<  2012 <<  8 <<  1 << qint64(2456154);
    QTest::newRow("data30")  <<  2012 <<  8 << 31 << qint64(2456184);
    QTest::newRow("data31")  <<  2012 <<  9 <<  1 << qint64(2456185);
    QTest::newRow("data32")  <<  2012 <<  9 << 30 << qint64(2456214);
    QTest::newRow("data33")  <<  2012 << 10 <<  1 << qint64(2456215);
    QTest::newRow("data34")  <<  2012 << 10 << 31 << qint64(2456245);
    QTest::newRow("data35")  <<  2012 << 11 <<  1 << qint64(2456246);
    QTest::newRow("data36")  <<  2012 << 11 << 30 << qint64(2456275);
    QTest::newRow("data37")  <<  2012 << 12 <<  1 << qint64(2456276);
    QTest::newRow("data38")  <<  2012 << 12 << 31 << qint64(2456306);

    // Key dates in a non leap year, positive jd
    QTest::newRow("data39")  <<  2010 <<  1 <<  1 << qint64(2455211);
    QTest::newRow("data40")  <<  2010 <<  1 << 31 << qint64(2455241);
    QTest::newRow("data41")  <<  2010 <<  2 <<  1 << qint64(2455242);
    QTest::newRow("data42")  <<  2010 <<  2 << 28 << qint64(2455269);
    QTest::newRow("data43")  <<  2010 <<  3 <<  1 << qint64(2455270);
    QTest::newRow("data44")  <<  2010 <<  3 << 31 << qint64(2455300);
    QTest::newRow("data45")  <<  2010 <<  4 <<  1 << qint64(2455301);
    QTest::newRow("data46")  <<  2010 <<  4 << 30 << qint64(2455330);
    QTest::newRow("data47")  <<  2010 <<  5 <<  1 << qint64(2455331);
    QTest::newRow("data48")  <<  2010 <<  5 << 31 << qint64(2455361);
    QTest::newRow("data49")  <<  2010 <<  6 <<  1 << qint64(2455362);
    QTest::newRow("data50")  <<  2010 <<  6 << 30 << qint64(2455391);
    QTest::newRow("data51")  <<  2010 <<  7 <<  1 << qint64(2455392);
    QTest::newRow("data52")  <<  2010 <<  7 << 31 << qint64(2455422);
    QTest::newRow("data53")  <<  2010 <<  8 <<  1 << qint64(2455423);
    QTest::newRow("data54")  <<  2010 <<  8 << 31 << qint64(2455453);
    QTest::newRow("data55")  <<  2010 <<  9 <<  1 << qint64(2455454);
    QTest::newRow("data56")  <<  2010 <<  9 << 30 << qint64(2455483);
    QTest::newRow("data57")  <<  2010 << 10 <<  1 << qint64(2455484);
    QTest::newRow("data58")  <<  2010 << 10 << 31 << qint64(2455514);
    QTest::newRow("data59")  <<  2010 << 11 <<  1 << qint64(2455515);
    QTest::newRow("data60")  <<  2010 << 11 << 30 << qint64(2455544);
    QTest::newRow("data61")  <<  2010 << 12 <<  1 << qint64(2455545);
    QTest::newRow("data62")  <<  2010 << 12 << 31 << qint64(2455575);

    // Key dates in a leap year, negative jd
    QTest::newRow("data63")  << -4781 <<  1 <<  1 << qint64(-24837);
    QTest::newRow("data64")  << -4781 <<  1 << 31 << qint64(-24807);
    QTest::newRow("data65")  << -4781 <<  2 <<  1 << qint64(-24806);
    QTest::newRow("data66")  << -4781 <<  2 << 29 << qint64(-24778);
    QTest::newRow("data67")  << -4781 <<  3 <<  1 << qint64(-24777);
    QTest::newRow("data68")  << -4781 <<  3 << 31 << qint64(-24747);
    QTest::newRow("data69")  << -4781 <<  4 <<  1 << qint64(-24746);
    QTest::newRow("data70")  << -4781 <<  4 << 30 << qint64(-24717);
    QTest::newRow("data71")  << -4781 <<  5 <<  1 << qint64(-24716);
    QTest::newRow("data72")  << -4781 <<  5 << 31 << qint64(-24686);
    QTest::newRow("data73")  << -4781 <<  6 <<  1 << qint64(-24685);
    QTest::newRow("data74")  << -4781 <<  6 << 30 << qint64(-24656);
    QTest::newRow("data75")  << -4781 <<  7 <<  1 << qint64(-24655);
    QTest::newRow("data76")  << -4781 <<  7 << 31 << qint64(-24625);
    QTest::newRow("data77")  << -4781 <<  8 <<  1 << qint64(-24624);
    QTest::newRow("data78")  << -4781 <<  8 << 31 << qint64(-24594);
    QTest::newRow("data79")  << -4781 <<  9 <<  1 << qint64(-24593);
    QTest::newRow("data80")  << -4781 <<  9 << 30 << qint64(-24564);
    QTest::newRow("data81")  << -4781 << 10 <<  1 << qint64(-24563);
    QTest::newRow("data82")  << -4781 << 10 << 31 << qint64(-24533);
    QTest::newRow("data83")  << -4781 << 11 <<  1 << qint64(-24532);
    QTest::newRow("data84")  << -4781 << 11 << 30 << qint64(-24503);
    QTest::newRow("data85")  << -4781 << 12 <<  1 << qint64(-24502);
    QTest::newRow("data86")  << -4781 << 12 << 31 << qint64(-24472);

    // Key dates in a non leap year, negative jd
    QTest::newRow("data87")  << -4780 <<  1 <<  1 << qint64(-24471);
    QTest::newRow("data88")  << -4780 <<  1 << 31 << qint64(-24441);
    QTest::newRow("data89")  << -4780 <<  2 <<  1 << qint64(-24440);
    QTest::newRow("data90")  << -4780 <<  2 << 28 << qint64(-24413);
    QTest::newRow("data91")  << -4780 <<  3 <<  1 << qint64(-24412);
    QTest::newRow("data92")  << -4780 <<  3 << 31 << qint64(-24382);
    QTest::newRow("data93")  << -4780 <<  4 <<  1 << qint64(-24381);
    QTest::newRow("data94")  << -4780 <<  4 << 30 << qint64(-24352);
    QTest::newRow("data95")  << -4780 <<  5 <<  1 << qint64(-24351);
    QTest::newRow("data96")  << -4780 <<  5 << 31 << qint64(-24321);
    QTest::newRow("data97")  << -4780 <<  6 <<  1 << qint64(-24320);
    QTest::newRow("data98")  << -4780 <<  6 << 30 << qint64(-24291);
    QTest::newRow("data99")  << -4780 <<  7 <<  1 << qint64(-24290);
    QTest::newRow("data100") << -4780 <<  7 << 31 << qint64(-24260);
    QTest::newRow("data101") << -4780 <<  8 <<  1 << qint64(-24259);
    QTest::newRow("data102") << -4780 <<  8 << 31 << qint64(-24229);
    QTest::newRow("data103") << -4780 <<  9 <<  1 << qint64(-24228);
    QTest::newRow("data104") << -4780 <<  9 << 30 << qint64(-24199);
    QTest::newRow("data105") << -4780 << 10 <<  1 << qint64(-24198);
    QTest::newRow("data106") << -4780 << 10 << 31 << qint64(-24168);
    QTest::newRow("data107") << -4780 << 11 <<  1 << qint64(-24167);
    QTest::newRow("data108") << -4780 << 11 << 30 << qint64(-24138);
    QTest::newRow("data109") << -4780 << 12 <<  1 << qint64(-24137);
    QTest::newRow("data110") << -4780 << 12 << 31 << qint64(-24107);
}

void tst_QCalendarSystem::julianDate()
{
    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, day);
    QFETCH(qint64, jd);

    testDate(QLocale::JulianCalendar, year, month, day, jd);
}

void tst_QCalendarSystem::julianYears_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("miy");
    QTest::addColumn<int>("diy");
    QTest::addColumn<bool>("isLeap");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("data1")  << -2001 << 12 << 366 << true  << true;
    QTest::newRow("data2")  << -2000 << 12 << 365 << false << true;
    QTest::newRow("data3")  <<  -101 << 12 << 366 << true  << true;
    QTest::newRow("data4")  <<  -100 << 12 << 365 << false << true;
    QTest::newRow("data5")  <<    -6 << 12 << 365 << false << true;
    QTest::newRow("data6")  <<    -5 << 12 << 366 << true  << true;
    QTest::newRow("data7")  <<    -4 << 12 << 365 << false << true;
    QTest::newRow("data8")  <<    -3 << 12 << 365 << false << true;
    QTest::newRow("data9")  <<    -2 << 12 << 365 << false << true;
    QTest::newRow("data10") <<    -1 << 12 << 366 << true  << true;
    QTest::newRow("data11") <<     0 <<  0 <<   0 << false << false; // Doesn't exist
    QTest::newRow("data12") <<     1 << 12 << 365 << false << true;
    QTest::newRow("data13") <<     2 << 12 << 365 << false << true;
    QTest::newRow("data14") <<     3 << 12 << 365 << false << true;
    QTest::newRow("data15") <<     4 << 12 << 366 << true  << true;
    QTest::newRow("data16") <<     7 << 12 << 365 << false << true;
    QTest::newRow("data17") <<     8 << 12 << 366 << true  << true;
    QTest::newRow("data18") <<   100 << 12 << 366 << true  << true;
    QTest::newRow("data19") <<   400 << 12 << 366 << true  << true;
    QTest::newRow("data20") <<  1900 << 12 << 366 << true  << true;
    QTest::newRow("data21") <<  2000 << 12 << 366 << true  << true;
    QTest::newRow("data22") <<  2100 << 12 << 366 << true  << true;
}

void tst_QCalendarSystem::julianYears()
{
    QFETCH(int, year);
    QFETCH(int, miy);
    QFETCH(int, diy);
    QFETCH(bool, isLeap);
    QFETCH(bool, isValid);

    testYears(QLocale::JulianCalendar, year, miy, diy, isLeap, isValid);
}

void tst_QCalendarSystem::julianMonths_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("dim");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("data1")  << 2000 <<  0 <<  0 << false;
    QTest::newRow("data2")  << 2000 <<  1 << 31 << true;
    QTest::newRow("data3")  << 2000 <<  2 << 29 << true;
    QTest::newRow("data4")  << 2001 <<  2 << 28 << true;
    QTest::newRow("data5")  << 2000 <<  3 << 31 << true;
    QTest::newRow("data6")  << 2000 <<  4 << 30 << true;
    QTest::newRow("data7")  << 2000 <<  5 << 31 << true;
    QTest::newRow("data8")  << 2000 <<  6 << 30 << true;
    QTest::newRow("data9")  << 2000 <<  7 << 31 << true;
    QTest::newRow("data10") << 2000 <<  8 << 31 << true;
    QTest::newRow("data11") << 2000 <<  9 << 30 << true;
    QTest::newRow("data12") << 2000 << 10 << 31 << true;
    QTest::newRow("data13") << 2000 << 11 << 30 << true;
    QTest::newRow("data14") << 2000 << 12 << 31 << true;
    QTest::newRow("data15") << 2000 << 13 <<  0 << false;
}

void tst_QCalendarSystem::julianMonths()
{
    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, dim);
    QFETCH(bool, isValid);

    testMonths(QLocale::JulianCalendar, year, month, dim, isValid);
}


// ROC Calendar specific tests
// Is Gregorian but with years counted from Gregorian 1912

void tst_QCalendarSystem::rocCalendar()
{
    QDateCalculator cal(QLocale::ROCCalendar);

    QCOMPARE(cal.calendarSystem(), QLocale::ROCCalendar);
    QCOMPARE(cal.epoch(), QDate::fromJulianDay(2419403));
    QCOMPARE(cal.epoch(), cal.date(1, 1, 1));
    QCOMPARE(cal.earliestValidDate(), QDate::fromJulianDay(2419403));
    QCOMPARE(cal.earliestValidDate(), cal.date(1, 1, 1));
    QCOMPARE(cal.latestValidDate(), QDate::fromJulianDay(6071462));
    QCOMPARE(cal.latestValidDate(), cal.date(9999, 12, 31));
    QCOMPARE(cal.maximumMonthsInYear(), 12);
    QCOMPARE(cal.maximumDaysInYear(),  366);
    QCOMPARE(cal.maximumDaysInMonth(),  31);
}

void tst_QCalendarSystem::rocRoundtrip()
{
    QDateCalculator cal(QLocale::ROCCalendar);

    // Test first 200 valid years
    QDate startDate = cal.date(1, 1, 1);
    QDate endDate = cal.date(200, 12, 31);
    roundtrip(QLocale::ROCCalendar, startDate, endDate);
    // Test middle 200 valid years
    startDate = cal.date(4900, 1, 1);
    endDate = cal.date(5100, 12, 31);
    roundtrip(QLocale::ROCCalendar, startDate, endDate);
    // Test last 200 valid years
    startDate = cal.date(9800, 1, 1);
    endDate = cal.date(9999, 12, 31);
    roundtrip(QLocale::ROCCalendar, startDate, endDate);
}

void tst_QCalendarSystem::rocDate_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("day");
    QTest::addColumn<qint64>("jd");

    // Note: When adding new dates, the jd must be obtained from an independent implementation

    // Some key dates:
    QTest::newRow("data1")  <<     1 <<  1 <<  1 << qint64(2419403); // Gregorian  1912-01-01
    QTest::newRow("data2")  <<     1 << 12 << 31 << qint64(2419768); // Gregorian  1912-12-31
    QTest::newRow("data3")  <<     2 <<  1 <<  1 << qint64(2419769); // Gregorian  1913-01-01
    QTest::newRow("data4")  <<     2 << 12 << 31 << qint64(2420133); // Gregorian  1913-12-31
    QTest::newRow("data5")  <<   100 <<  1 <<  1 << qint64(2455563); // Gregorian  2011-01-01
    QTest::newRow("data6")  <<   100 << 12 << 31 << qint64(2455927); // Gregorian  2011-12-31
    QTest::newRow("data7")  <<  1000 <<  1 <<  1 << qint64(2784281); // Gregorian  2911-01-01
    QTest::newRow("data8")  <<  1000 << 12 << 31 << qint64(2784645); // Gregorian  2911-12-31
    QTest::newRow("data9")  <<  9998 <<  1 <<  1 << qint64(6070733); // Gregorian 11909-01-01
    QTest::newRow("data10") <<  9998 << 12 << 31 << qint64(6071097); // Gregorian 11909-12-31
    QTest::newRow("data11") <<  9999 <<  1 <<  1 << qint64(6071098); // Gregorian 11910-01-01
    QTest::newRow("data12") <<  9999 << 12 << 31 << qint64(6071462); // Gregorian 11910-12-31
}

void tst_QCalendarSystem::rocDate()
{
    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, day);
    QFETCH(qint64, jd);

    testDate(QLocale::ROCCalendar, year, month, day, jd);
}

void tst_QCalendarSystem::rocYears_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("miy");
    QTest::addColumn<int>("diy");
    QTest::addColumn<bool>("isLeap");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("data1") <<     0 <<  0 <<   0 << false << false; // Doesn't exist
    QTest::newRow("data2") <<     1 << 12 << 366 << true  << true;  // Gregorian 1912
    QTest::newRow("data3") <<     2 << 12 << 365 << false << true;  // Gregorian 1913
    QTest::newRow("data4") <<    89 << 12 << 366 << true  << true;  // Gregorian 2000
    QTest::newRow("data5") <<    90 << 12 << 365 << false << true;  // Gregorian 2001
    QTest::newRow("data6") <<   189 << 12 << 365 << false << true;  // Gregorian 2100
}

void tst_QCalendarSystem::rocYears()
{
    QFETCH(int, year);
    QFETCH(int, miy);
    QFETCH(int, diy);
    QFETCH(bool, isLeap);
    QFETCH(bool, isValid);

    testYears(QLocale::ROCCalendar, year, miy, diy, isLeap, isValid);
}

void tst_QCalendarSystem::rocMonths_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("dim");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("data1")  <<   89 <<  0 <<  0 << false;
    QTest::newRow("data2")  <<   89 <<  1 << 31 << true;
    QTest::newRow("data3")  <<   89 <<  2 << 29 << true;
    QTest::newRow("data5")  <<   89 <<  3 << 31 << true;
    QTest::newRow("data6")  <<   89 <<  4 << 30 << true;
    QTest::newRow("data7")  <<   89 <<  5 << 31 << true;
    QTest::newRow("data8")  <<   89 <<  6 << 30 << true;
    QTest::newRow("data9")  <<   89 <<  7 << 31 << true;
    QTest::newRow("data10") <<   89 <<  8 << 31 << true;
    QTest::newRow("data11") <<   89 <<  9 << 30 << true;
    QTest::newRow("data12") <<   89 << 10 << 31 << true;
    QTest::newRow("data13") <<   89 << 11 << 30 << true;
    QTest::newRow("data14") <<   89 << 12 << 31 << true;
    QTest::newRow("data15") <<   89 << 13 <<  0 << false;
}

void tst_QCalendarSystem::rocMonths()
{
    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, dim);
    QFETCH(bool, isValid);

    testMonths(QLocale::ROCCalendar, year, month, dim, isValid);
}


// Thai Calendar specific tests
// Is Gregorian but with years counted from Gregorian -544

void tst_QCalendarSystem::thaiCalendar()
{
    QDateCalculator cal(QLocale::ThaiCalendar);

    QCOMPARE(cal.calendarSystem(), QLocale::ThaiCalendar);
    QCOMPARE(cal.epoch(), QDate::fromJulianDay(1522734));
    QCOMPARE(cal.epoch(), cal.date(0, 1, 1));
    QCOMPARE(cal.earliestValidDate(), QDate::fromJulianDay(1522734));
    QCOMPARE(cal.earliestValidDate(), cal.date(0, 1, 1));
    QCOMPARE(cal.latestValidDate(), QDate::fromJulianDay(5175158));
    QCOMPARE(cal.latestValidDate(), cal.date(9999, 12, 31));
    QCOMPARE(cal.maximumMonthsInYear(), 12);
    QCOMPARE(cal.maximumDaysInYear(),  366);
    QCOMPARE(cal.maximumDaysInMonth(),  31);
}

void tst_QCalendarSystem::thaiRoundtrip()
{
    QDateCalculator cal(QLocale::ThaiCalendar);

    // Test first 200 valid years
    QDate startDate = cal.date(0, 1, 1);
    QDate endDate = cal.date(200, 12, 31);
    roundtrip(QLocale::ThaiCalendar, startDate, endDate);
    // Test middle 200 valid years
    startDate = cal.date(4900, 1, 1);
    endDate = cal.date(5100, 12, 31);
    roundtrip(QLocale::ThaiCalendar, startDate, endDate);
    // Test last 200 valid years
    startDate = cal.date(9800, 1, 1);
    endDate = cal.date(9999, 12, 31);
    roundtrip(QLocale::ThaiCalendar, startDate, endDate);
}

void tst_QCalendarSystem::thaiDate_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("day");
    QTest::addColumn<qint64>("jd");

    // Note: When adding new dates, the jd must be obtained from an independent implementation

    // Some key dates:
    QTest::newRow("data1")  <<     0 <<  1 <<  1 << qint64(1522734); // Gregorian -0544-01-01
    QTest::newRow("data2")  <<     0 << 12 << 31 << qint64(1523098); // Gregorian -0544-12-31
    QTest::newRow("data3")  <<     1 <<  1 <<  1 << qint64(1523099); // Gregorian -0543-01-01
    QTest::newRow("data4")  <<     1 << 12 << 31 << qint64(1523463); // Gregorian -0543-12-31
    QTest::newRow("data5")  <<  1000 <<  1 <<  1 << qint64(1887977); // Gregorian  0457-01-01
    QTest::newRow("data6")  <<  1000 << 12 << 31 << qint64(1888341); // Gregorian  0457-12-31
    QTest::newRow("data7")  <<  2000 <<  1 <<  1 << qint64(2253219); // Gregorian  1457-01-01
    QTest::newRow("data8")  <<  2000 << 12 << 31 << qint64(2253583); // Gregorian  1457-12-31
    QTest::newRow("data9")  <<  3000 <<  1 <<  1 << qint64(2618462); // Gregorian  2457-01-01
    QTest::newRow("data10") <<  3000 << 12 << 31 << qint64(2618826); // Gregorian  2457-12-31
    QTest::newRow("data11") <<  9998 <<  1 <<  1 << qint64(5174428); // Gregorian  9455-01-01
    QTest::newRow("data12") <<  9998 << 12 << 31 << qint64(5174792); // Gregorian  9455-12-31
    QTest::newRow("data13") <<  9999 <<  1 <<  1 << qint64(5174793); // Gregorian  9456-01-01
    QTest::newRow("data14") <<  9999 << 12 << 31 << qint64(5175158); // Gregorian  9456-12-31
}

void tst_QCalendarSystem::thaiDate()
{
    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, day);
    QFETCH(qint64, jd);

    testDate(QLocale::ThaiCalendar, year, month, day, jd);
}

void tst_QCalendarSystem::thaiYears_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("miy");
    QTest::addColumn<int>("diy");
    QTest::addColumn<bool>("isLeap");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("data1") <<    -1 <<  0 <<   0 << false << false; // Doesn't exist
    QTest::newRow("data2") <<     0 << 12 << 365 << false << true;  // Gregorian -544
    QTest::newRow("data3") <<     1 << 12 << 365 << false << true;  // Gregorian -543
    QTest::newRow("data4") <<  2543 << 12 << 366 << true  << true;  // Gregorian 2000
    QTest::newRow("data5") <<  2544 << 12 << 365 << false << true;  // Gregorian 2001
    QTest::newRow("data6") <<  2643 << 12 << 365 << false << true;  // Gregorian 2100
}

void tst_QCalendarSystem::thaiYears()
{
    QFETCH(int, year);
    QFETCH(int, miy);
    QFETCH(int, diy);
    QFETCH(bool, isLeap);
    QFETCH(bool, isValid);

    testYears(QLocale::ThaiCalendar, year, miy, diy, isLeap, isValid);
}

void tst_QCalendarSystem::thaiMonths_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("dim");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("data1")  << 2543 <<  0 <<  0 << false;
    QTest::newRow("data2")  << 2543 <<  1 << 31 << true;
    QTest::newRow("data3")  << 2543 <<  2 << 29 << true;
    QTest::newRow("data5")  << 2543 <<  3 << 31 << true;
    QTest::newRow("data6")  << 2543 <<  4 << 30 << true;
    QTest::newRow("data7")  << 2543 <<  5 << 31 << true;
    QTest::newRow("data8")  << 2543 <<  6 << 30 << true;
    QTest::newRow("data9")  << 2543 <<  7 << 31 << true;
    QTest::newRow("data10") << 2543 <<  8 << 31 << true;
    QTest::newRow("data11") << 2543 <<  9 << 30 << true;
    QTest::newRow("data12") << 2543 << 10 << 31 << true;
    QTest::newRow("data13") << 2543 << 11 << 30 << true;
    QTest::newRow("data14") << 2543 << 12 << 31 << true;
    QTest::newRow("data15") << 2543 << 13 <<  0 << false;
}

void tst_QCalendarSystem::thaiMonths()
{
    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, dim);
    QFETCH(bool, isValid);

    testMonths(QLocale::ThaiCalendar, year, month, dim, isValid);
}

QTEST_APPLESS_MAIN(tst_QCalendarSystem)

#include "tst_qcalendarsystem.moc"
