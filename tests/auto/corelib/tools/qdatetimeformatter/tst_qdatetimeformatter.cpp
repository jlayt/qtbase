/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>
#include <math.h>
#include <float.h>

#include <qglobal.h>
#include <qlocale.h>
#include <qnumeric.h>
#include <private/qdatetimeformatter_p.h>

class tst_QDateTimeFormatter : public QObject
{
    Q_OBJECT

public:
    tst_QDateTimeFormatter();

private slots:
    void initTestCase();
    void testBasic();
    void testRelative();
    void testStandards();
    void testCustom();
};

tst_QDateTimeFormatter::tst_QDateTimeFormatter()
{
}

void tst_QDateTimeFormatter::initTestCase()
{
}

void tst_QDateTimeFormatter::testBasic()
{
    qDebug() << "";
    qDebug() << "USA";
    QDateTimeFormatter full(QDateTimeFormatter::FullStyle, "en_US");
    qDebug() << "en_US isValid()                       = " << full.isValid();
    qDebug() << "en_US isInvalid()                     = " << full.isInvalid();
    qDebug() << "en_US localeCode()                    = " << full.localeCode();
    qDebug() << "en_US localeCode(EffectiveLocaleCode) = " << full.localeCode(QLocale::EffectiveLocaleCode);
    qDebug() << "en_US style()                         = " << full.style();
    qDebug() << "en_US datePattern()                   = " << full.datePattern();
    qDebug() << "en_US timePattern()                   = " << full.timePattern();
    qDebug() << "en_US dateTimePattern()               = " << full.dateTimePattern();
    qDebug() << "en_US shortYearWindowStart()          = " << full.shortYearWindowStart();
    qDebug() << "";
    qDebug() << "Default Format = Thursday, February 2, 2012 3:02:01 AM Greenwich Mean Time";
    qDebug() << "en_US toString() = " << full.toString(QDate(2012, 1, 1));
    qDebug() << "en_US toString() = " << full.toString(QTime(1, 2, 3));
    qDebug() << "en_US toString() = " << full.toString(QDateTime(QDate(2012, 2, 2), QTime(3, 2, 1)));
    qDebug() << "";
    qDebug() << "Default Parse (Lenient) = Thursday, February 2, 2012 3:02:01 AM Greenwich Mean Time";
    qDebug() << "en_US toDate()     = " << full.toDate("Sunday, January 1, 2012");
    qDebug() << "en_US toTime()     = " << full.toTime("3:02:01 AM Greenwich Mean Time");
    qDebug() << "en_US toDateTime() = " << full.toDateTime("Thursday, February 2, 2012 3:02:01 AM Greenwich Mean Time");
    qDebug() << "";
    qDebug() << "Lenient Parse = Sun Jan 1 2012 3:02:01 AM GMT";
    qDebug() << "en_US toDate()     = " << full.toDate("Sun Jan 1 2012", QLocale::LenientParsing);
    qDebug() << "en_US toTime()     = " << full.toTime("3:02:01 AM GMT", QLocale::LenientParsing);
    qDebug() << "en_US toDateTime() = " << full.toDateTime("Sun Jan 1 2012 3:02:01 AM GMT", QLocale::LenientParsing);
    qDebug() << "";
    qDebug() << "Strict Parse = Sun Jan 1 2012 3:02:01 AM GMT";
    qDebug() << "en_US toDate()     = " << full.toDate("Sun Jan 1 2012", QLocale::StrictParsing);
    qDebug() << "en_US toTime()     = " << full.toTime("3:02:01 AM GMT", QLocale::StrictParsing);
    qDebug() << "en_US toDateTime() = " << full.toDateTime("Sun Jan 1 2012 3:02:01 AM GMT", QLocale::StrictParsing);
    qDebug() << "";
    qDebug() << "Override pattern format";
    qDebug() << "en_US toString() = " << full.toString(QDate(2012, 1, 1), "yyyy-MM-dd");
    qDebug() << "en_US toString() = " << full.toString(QTime(3, 2, 1), "HH:mm:ss");
    qDebug() << "en_US toString() = " << full.toString(QDateTime(QDate(2012, 1, 1), QTime(3, 2, 1)), "yyyy-MM-dd HH:mm:ss");
    qDebug() << "";
    qDebug() << "Override pattern parse = 2012-01-01 3:02:00";
    qDebug() << "en_US toDate()     = " << full.toDate("2012-01-01", "yyyy-MM-dd");
    qDebug() << "en_US toTime()     = " << full.toTime("3:02:01", "HH:mm:ss");
    qDebug() << "en_US toDateTime() = " << full.toDateTime("2012-01-01 3:02:00", "yyyy-MM-dd HH:mm:ss");
    qDebug() << "";
    qDebug() << "eraName Long   = " << full.eraName(1, QDateTimeFormatter::LongName);
    qDebug() << "eraName Short  = " << full.eraName(1, QDateTimeFormatter::ShortName);
    qDebug() << "eraName Narrow = " << full.eraName(1, QDateTimeFormatter::NarrowName);
    qDebug() << "";
    qDebug() << "quarterName Long   = " << full.quarterName(1, QDateTimeFormatter::LongName);
    qDebug() << "quarterName Short  = " << full.quarterName(1, QDateTimeFormatter::ShortName);
    qDebug() << "quarterName Narrow = " << full.quarterName(1, QDateTimeFormatter::NarrowName);
    qDebug() << "";
    qDebug() << "monthName Long   = " << full.monthName(1, QDateTimeFormatter::LongName);
    qDebug() << "monthName Short  = " << full.monthName(1, QDateTimeFormatter::ShortName);
    qDebug() << "monthName Narrow = " << full.monthName(1, QDateTimeFormatter::NarrowName);
    qDebug() << "";
    qDebug() << "weekdayName Long   = " << full.weekdayName(1, QDateTimeFormatter::LongName);
    qDebug() << "weekdayName Short  = " << full.weekdayName(1, QDateTimeFormatter::ShortName);
    qDebug() << "weekdayName Narrow = " << full.weekdayName(1, QDateTimeFormatter::NarrowName);
    qDebug() << "";
    qDebug() << "dayPeriodName Long   = " << full.dayPeriodName(QTime(0,0,0), QDateTimeFormatter::LongName);
    qDebug() << "dayPeriodName Short  = " << full.dayPeriodName(QTime(0,0,0), QDateTimeFormatter::ShortName);
    qDebug() << "dayPeriodName Narrow = " << full.dayPeriodName(QTime(0,0,0), QDateTimeFormatter::NarrowName);
}

void tst_QDateTimeFormatter::testRelative()
{
    qDebug() << "Relative Style";
    QDateTimeFormatter rel(QDateTimeFormatter::MediumRelativeStyle, "en_US");
    qDebug() << "en_US toString(2012-01-01) = " << rel.toString(QDate(2012, 1, 1));
    qDebug() << "en_US toString(Today)      = " << rel.toString(QDate::currentDate());
    qDebug() << "en_US toDate(2012-01-01)   = " << rel.toDate("Jan 1, 2012");
    qDebug() << "en_US toDate(Today)        = " << rel.toDate("Today");
}

void tst_QDateTimeFormatter::testStandards()
{
    qDebug() << "ISO Style = 2012-01-01 01:02:03";
    QDateTimeFormatter iso(QDateTimeFormatter::IsoStyle, "en_US");
    qDebug() << "en_US toString()   = " << iso.toString(QDate(2012, 1, 1));
    qDebug() << "en_US toString()   = " << iso.toString(QTime(1, 2, 3));
    qDebug() << "en_US toString()   = " << iso.toString(QDateTime(QDate(2012, 1, 1), QTime(1, 2, 3)));
    qDebug() << "en_US toDate()     = " << iso.toDate("2012-01-01");
    qDebug() << "en_US toTime()     = " << iso.toTime("01:02:03");
    qDebug() << "en_US toDateTime() = " << iso.toDateTime("2012-01-01T01:02:03.000Z");
}

void tst_QDateTimeFormatter::testCustom()
{
    qDebug() << "Custom Style";
    QCustomDateTimeFormatter cus(QDateTimeFormatter::MediumStyle, "en_US");
    qDebug() << "en_US toString() = " << cus.toString(QDate(2012, 2, 2));
    cus.setDatePattern("yyyy");
    qDebug() << "en_US toString() = " << cus.toString(QDate(2012, 2, 2));
}

QTEST_MAIN(tst_QDateTimeFormatter)

#include "tst_qdatetimeformatter.moc"
