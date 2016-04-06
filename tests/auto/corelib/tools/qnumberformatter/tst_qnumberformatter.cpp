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

#include <QtTest/QtTest>
#include <math.h>
#include <float.h>

#include <qglobal.h>
#include <qlocale.h>
#include <qnumeric.h>
#include <qnumberformatter.h>
#include <qcurrencyformatter.h>
#include <private/qnumberformatterprivate_p.h>

class tst_QNumberFormatter : public QObject
{
    Q_OBJECT

public:
    tst_QNumberFormatter();

private slots:
    void initTestCase();
};

tst_QNumberFormatter::tst_QNumberFormatter()
{
}

void tst_QNumberFormatter::initTestCase()
{
    qDebug() << "";
    //qDebug() << "QNumberFormatter::availableStyles() = " << QNumberFormatter::availableStyles();
    qDebug() << "";
    qDebug() << "USA";
    QCustomNumberFormatter enus(QLocale::DecimalStyle, QLocaleCode("en_US"));
    qDebug() << "en_US isValid() = " << enus.isValid();
    qDebug() << "en_US locale()  = " << enus.locale();
    qDebug() << "en_US style()   = " << enus.style();
    qDebug() << "";
    qDebug() << "en_US symbol(DecimalSeparatorSymbol)   = " << enus.symbol(QLocale::DecimalSeparatorSymbol);
    qDebug() << "en_US symbol(GroupingSeparatorSymbol)  = " << enus.symbol(QLocale::GroupingSeparatorSymbol);
    qDebug() << "en_US symbol(PositivePrefix)           = " << enus.symbol(QLocale::PositivePrefix);
    qDebug() << "";
    qDebug() << "en_US attribute(PrimaryGroupingSize)   = " << enus.attribute(QLocale::PrimaryGroupingSize);
    qDebug() << "en_US attribute(SecondaryGroupingSize) = " << enus.attribute(QLocale::SecondaryGroupingSize);
    qDebug() << "";
    qDebug() << "en_US flag(ParseLenient)          = " << enus.flag(QLocale::ParseLenient);
    qDebug() << "en_US flag(FormatIntegerWithDecimalSeparator) = " << enus.flag(QLocale::FormatIntegerWithDecimalSeparator);
    qDebug() << "en_US flags() = " << enus.flags();
    qDebug() << "";
    qDebug() << "en_US decimalFormatMode() = " << enus.decimalFormatMode();
    qDebug() << "";
    qDebug() << "en_US roundingMode()      = " << enus.roundingMode();
    qDebug() << "en_US roundingIncrement() = " << enus.roundingIncrement();
    qDebug() << "";
    qDebug() << "en_US paddingWidth()    = " << enus.paddingWidth();
    qDebug() << "en_US padding()         = " << enus.padding();
    qDebug() << "en_US paddingPosition() = " << enus.paddingPosition();
    qDebug() << "";
    qDebug() << "en_US toString(12345)                = " << enus.toString((qint64)12345);
    qDebug() << "en_US toString(2147483647)           = " << enus.toString((qint64)2147483647);
    qDebug() << "en_US toString(-2147483647)          = " << enus.toString((qint64)-2147483647);
    qDebug() << "en_US toString(9223372036854775807)  = " << enus.toString((qint64)9223372036854775807);
    qDebug() << "en_US toString(-9223372036854775807) = " << enus.toString((qint64)-9223372036854775807);
    qDebug() << "en_US toString(12.345)               = " << enus.toString(12.345);
    qDebug() << "en_US toString(1.123456789)          = " << enus.toString(1.123456789);
    qDebug() << "en_US toString(123456789.123456789)  = " << enus.toString(123456789.123456789);
    qDebug() << "";
    qDebug() << "en_US toInt64(12,345)  = " << enus.toInt64("12,345");
    qDebug() << "en_US toInt64(12.345)  = " << enus.toInt64("12.345");
    qDebug() << "en_US toInt64(12345)   = " << enus.toInt64("12345");
    qDebug() << "en_US toInt64(12,345)  = " << enus.toUInt64("12,345");
    qDebug() << "en_US toInt64(12.345)  = " << enus.toUInt64("12.345");
    qDebug() << "en_US toInt64(12345)   = " << enus.toUInt64("12345");
    qDebug() << "en_US toDouble(12,345) = " << enus.toDouble("12,345");
    qDebug() << "en_US toDouble(12.345) = " << enus.toDouble("12.345");
    qDebug() << "en_US toDouble(12345)  = " << enus.toDouble("12345");
    qDebug() << "";
    enus.setFlag(QLocale::ParseLenient);
    qDebug() << "en_US toInt64(12,345)  = " << enus.toInt64("12,345");
    qDebug() << "en_US toInt64(12.345)  = " << enus.toInt64("12.345");
    qDebug() << "en_US toInt64(12345)   = " << enus.toInt64("12345");
    qDebug() << "en_US toInt64(12,345)  = " << enus.toUInt64("12,345");
    qDebug() << "en_US toInt64(12.345)  = " << enus.toUInt64("12.345");
    qDebug() << "en_US toInt64(12345)   = " << enus.toUInt64("12345");
    qDebug() << "en_US toDouble(12,345) = " << enus.toDouble("12,345");
    qDebug() << "en_US toDouble(12.345) = " << enus.toDouble("12.345");
    qDebug() << "en_US toDouble(12345)  = " << enus.toDouble("12345");

    qDebug() << "";
    qDebug() << "Germany";
    QCustomNumberFormatter dede(QLocale::DecimalStyle, QLocaleCode("de_DE"));
    qDebug() << "de_DE isValid() = " << dede.isValid();
    qDebug() << "de_DE locale() = " << dede.locale();
    qDebug() << "de_DE style() = " << dede.style();
    qDebug() << "de_DE toString() = " << dede.toString((qint64)12345);
    qDebug() << "de_DE toInt64() = " << dede.toInt64("12,345");
    qDebug() << "de_DE toInt64() = " << dede.toInt64("12.345");
    qDebug() << "de_DE toInt64() = " << dede.toInt64("12345");

    qDebug() << "";
    qDebug() << "CurrencyStyle";
    QCustomCurrencyFormatter enusCurr(QLocale::CurrencySymbolStyle, QLocaleCode("en_US"));
    qDebug() << "en_US isValid()" << enusCurr.isValid();
    qDebug() << "en_US currencyCode()" << enusCurr.currencyCode();
    qDebug() << "en_US toString()" << enusCurr.toString(123.45);
    qDebug() << "en_US toDouble()" << enusCurr.toDouble("$123.45");
    qDebug() << "en_US toString(USD)" << enusCurr.toString(12.34, "USD");
    qDebug() << "en_US toString(GBP)" << enusCurr.toString(12.34, "GBP");
    QString code;
    double amt = enusCurr.toDouble("USD 12.34", &code);
    qDebug() << "en_US toDouble(USD)" << amt << code;
    code.clear();
    amt = enusCurr.toDouble("GBP 12.34", &code);
    qDebug() << "en_US toDouble(GBP)" << amt << code;
    code.clear();
    amt = enusCurr.toDouble("$ 12.34", &code);
    qDebug() << "en_US toDouble($)" << amt << code;
    code.clear();
    amt = enusCurr.toDouble("£ 12.34", &code);
    qDebug() << "en_US toDouble(£)" << amt << code;

    qDebug() << "";
    qDebug() << "CurrencyCodeStyle";
    QCustomCurrencyFormatter enusCurrCode(QLocale::CurrencyCodeStyle, QLocaleCode("en_US"));
    qDebug() << "en_US isValid()" << enusCurrCode.isValid();
    qDebug() << "en_US currencyCode()" << enusCurrCode.currencyCode();
    qDebug() << "en_US toString()" << enusCurrCode.toString(123.45);
    qDebug() << "en_US toDouble()" << enusCurrCode.toDouble("USD 123.45");
    qDebug() << "en_US toString(USD)" << enusCurrCode.toString(12.34, "USD");
    qDebug() << "en_US toString(GBP)" << enusCurrCode.toString(12.34, "GBP");
    code.clear();
    amt = enusCurrCode.toDouble("USD 12.34", &code);
    qDebug() << "en_US toDouble(USD)" << amt << code;
    code.clear();
    amt = enusCurrCode.toDouble("GBP 12.34", &code);
    qDebug() << "en_US toDouble(GBP)" << amt << code;
    code.clear();
    amt = enusCurrCode.toDouble("$ 12.34", &code);
    qDebug() << "en_US toDouble($)" << amt << code;
    code.clear();
    amt = enusCurrCode.toDouble("£ 12.34", &code);
    qDebug() << "en_US toDouble(£)" << amt << code;

    qDebug() << "";
    qDebug() << "CurrencyNameStyle";
    QCustomCurrencyFormatter enusCurrName(QLocale::CurrencyNameStyle, QLocaleCode("en_US"));
    qDebug() << "en_US isValid()" << enusCurrName.isValid();
    qDebug() << "en_US currencyCode()" << enusCurrName.currencyCode();
    qDebug() << "en_US toString()" << enusCurrName.toString(123.45);
    qDebug() << "en_US toDouble()" << enusCurrName.toDouble("123.45 US dollars");
    qDebug() << "en_US toString(USD)" << enusCurrName.toString(12.34, "USD");
    qDebug() << "en_US toString(GBP)" << enusCurrName.toString(12.34, "GBP");
    code.clear();
    amt = enusCurrName.toDouble("USD 12.34", &code);
    qDebug() << "en_US toDouble(USD)" << amt << code;
    code.clear();
    amt = enusCurrName.toDouble("GBP 12.34", &code);
    qDebug() << "en_US toDouble(GBP)" << amt << code;
    code.clear();
    amt = enusCurrName.toDouble("$ 12.34", &code);
    qDebug() << "en_US toDouble($)" << amt << code;
    code.clear();
    amt = enusCurrName.toDouble("£ 12.34", &code);
    qDebug() << "en_US toDouble(£)" << amt << code;

    qDebug() << "";
    qDebug() << "PercentageStyle";
    QCustomNumberFormatter enusPerc(QLocale::PercentageStyle, QLocaleCode("en_US"));
    qDebug() << "en_US isValid() = " << enusPerc.isValid();
    qDebug() << "en_US style()   = " << enusPerc.style();
    qDebug() << "en_US toString(1.2345)  = " << enusPerc.toString(1.2345);
    qDebug() << "en_US toDouble(123.45%) = " << enusPerc.toDouble("123.45%");

    qDebug() << "";
    qDebug() << "ScientificStyle";
    QCustomNumberFormatter enusSci(QLocale::ScientificStyle, QLocaleCode("en_US"));
    qDebug() << "en_US isValid() = " << enusSci.isValid();
    qDebug() << "en_US style()   = " << enusSci.style();
    qDebug() << "en_US toString(12345)    = " << enusSci.toString((qint64)12345);
    qDebug() << "en_US toString(12.345)   = " << enusSci.toString((qint64)12.345);
    qDebug() << "en_US toDouble(1.2345E4) = " << enusSci.toDouble("1.2345E4");

    qDebug() << "";
    qDebug() << "OrdinalStyle";
    QCustomNumberFormatter enusOrd(QLocale::OrdinalStyle, QLocaleCode("en_US"));
    qDebug() << "en_US isValid() = " << enusOrd.isValid();
    qDebug() << "en_US style()   = " << enusOrd.style();
    qDebug() << "en_US toString(12345)   = " << enusOrd.toString((qint64)12345);
    qDebug() << "en_US toInt64(12,345th) = " << enusOrd.toInt64("12,345th");

    qDebug() << "";
    qDebug() << "DurationStyle";
    QCustomNumberFormatter enusDur(QLocale::DurationStyle, QLocaleCode("en_US"));
    qDebug() << "en_US isValid() = " << enusDur.isValid();
    qDebug() << "en_US style()   = " << enusDur.style();
    qDebug() << "en_US toString(12345)  = " << enusDur.toString((qint64)12345);
    qDebug() << "en_US toInt64(3:25:45) = " << enusDur.toInt64("3:25:45");

    qDebug() << "";
    qDebug() << "SpelloutStyle";
    QCustomNumberFormatter enusSpell(QLocale::SpelloutStyle, QLocaleCode("en_US"));
    qDebug() << "en_US isValid() = " << enusSpell.isValid();
    qDebug() << "en_US style()   = " << enusSpell.style();
    qDebug() << "en_US toString(12345) = " << enusSpell.toString((qint64)12345);
    qDebug() << "en_US toInt64()  = " << enusSpell.toInt64("twelve thousand three hundred forty-five");
}

QTEST_MAIN(tst_QNumberFormatter)

#include "tst_qnumberformatter.moc"
