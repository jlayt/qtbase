/****************************************************************************
**
** Copyright (C) 2012 John Layt jlayt@kde.org
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
#include <private/qnumberformatter_p.h>

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
    qDebug() << "USA";
    QNumberFormatter enus(QNumberFormatter::DecimalStyle, "en_US");
    qDebug() << "en_US isValid()" << enus.isValid();
    qDebug() << "en_US isInvalid()" << enus.isInvalid();
    qDebug() << "en_US localeCode()" << enus.localeCode();
    qDebug() << "en_US localeCode(EffectiveLocaleCode)" << enus.localeCode(QLocale::EffectiveLocaleCode);
    qDebug() << "en_US numberStyle()" << enus.numberStyle();
    qDebug() << "en_US currencyCode()" << enus.currencyCode();
    qDebug() << "en_US numberSymbol(PercentSymbol)" << enus.numberSymbol(QNumberFormatter::PercentSymbol);
    qDebug() << "en_US numberAttribute(PrimaryGroupingSize)" << enus.numberAttribute(QNumberFormatter::PrimaryGroupingSize);
    qDebug() << "en_US usesSignificantDigits()" << enus.usesSignificantDigits();
    qDebug() << "en_US roundingMode()" << enus.roundingMode();
    qDebug() << "en_US roundingIncrement()" << enus.roundingIncrement();
    qDebug() << "en_US paddingWidth()" << enus.paddingWidth();
    qDebug() << "en_US padding()" << enus.padding();
    qDebug() << "en_US paddingPosition()" << enus.paddingPosition();
    qDebug() << "en_US parsingMode()" << enus.parsingMode();
    qDebug() << "";
    qDebug() << "en_US toString()" << enus.toString(12345);
    qDebug() << "en_US toString()" << enus.toString(2147483647);
    qDebug() << "en_US toString()" << enus.toString(-2147483647);
    qDebug() << "en_US toString()" << enus.toString((qint64)9223372036854775807);
    qDebug() << "en_US toString()" << enus.toString((qint64)-9223372036854775807);
    qDebug() << "en_US toString()" << enus.toString(12.345);
    qDebug() << "en_US toString()" << enus.toString(1.123456789);
    qDebug() << "en_US toString()" << enus.toString(123456789.123456789);
    qDebug() << "";
    qDebug() << "en_US toString()" << enus.toString(12345, 10);
    qDebug() << "en_US toString()" << enus.toString(-12345, 10);
    qDebug() << "en_US toString()" << enus.toString(12345, 10, QString(" "), QNumberFormatter::PadBeforePrefix);
    qDebug() << "en_US toString()" << enus.toString(-12345, 10, QString(" "), QNumberFormatter::PadBeforePrefix);
    qDebug() << "en_US toString()" << enus.toString(12345, 10, QString("p"), QNumberFormatter::PadAfterPrefix);
    qDebug() << "en_US toString()" << enus.toString(-12345, 10, QString("p"), QNumberFormatter::PadAfterPrefix);
    qDebug() << "";
    qDebug() << "en_US toString()" << enus.toString(123456789.123456789, 3, 3, 3, 3);
    qDebug() << "en_US toString()" << enus.toString(123456789.123456789, 3, 3, 3, 3, 10, QString("p"), QNumberFormatter::PadAfterPrefix);
    qDebug() << "en_US toString()" << enus.toString(123456789.123456789, 3, 3, 3, 3, 10, QString("p"), QNumberFormatter::PadAfterPrefix, QNumberFormatter::RoundDown, 0.01);
    qDebug() << "en_US toString()" << enus.toString(123456789.123456789, 3, 3);
    qDebug() << "en_US toString()" << enus.toString(123456789.123456789, 3, 3, 30, QString("p"), QNumberFormatter::PadAfterPrefix);
    qDebug() << "en_US toString()" << enus.toString(123456789.123456789, 3, 3, 30, QString("p"), QNumberFormatter::PadAfterPrefix, QNumberFormatter::RoundDown, 0.01);
    qDebug() << "";
    qDebug() << "en_US toInt32() " << enus.toInt32("12,345");
    qDebug() << "en_US toInt32() " << enus.toInt32("12.345");
    qDebug() << "en_US toInt32() " << enus.toInt32("12345");
    qDebug() << "en_US toInt64() " << enus.toInt64("12,345");
    qDebug() << "en_US toInt64() " << enus.toInt64("12.345");
    qDebug() << "en_US toInt64() " << enus.toInt64("12345");
    qDebug() << "en_US toDouble()" << enus.toDouble("12,345");
    qDebug() << "en_US toDouble()" << enus.toDouble("12.345");
    qDebug() << "en_US toDouble()" << enus.toDouble("12345");
    qDebug() << "";
    qDebug() << "en_US toInt32() " << enus.toInt32("12,345", 0, QLocale::StrictParsing);
    qDebug() << "en_US toInt32() " << enus.toInt32("12.345", 0, QLocale::StrictParsing);
    qDebug() << "en_US toInt32() " << enus.toInt32("12345", 0, QLocale::StrictParsing);
    qDebug() << "en_US toInt64() " << enus.toInt64("12,345", 0, QLocale::StrictParsing);
    qDebug() << "en_US toInt64() " << enus.toInt64("12.345", 0, QLocale::StrictParsing);
    qDebug() << "en_US toInt64() " << enus.toInt64("12345", 0, QLocale::StrictParsing);
    qDebug() << "en_US toDouble()" << enus.toDouble("12,345", 0, QLocale::StrictParsing);
    qDebug() << "en_US toDouble()" << enus.toDouble("12.345", 0, QLocale::StrictParsing);
    qDebug() << "en_US toDouble()" << enus.toDouble("12345", 0, QLocale::StrictParsing);
    qDebug() << "";
    qDebug() << "en_US toAnyCurrency(USD)" << enus.toAnyCurrency(12.34, "USD");
    qDebug() << "en_US toAnyCurrency(GBP)" << enus.toAnyCurrency(12.34, "GBP");
    QString code;
    double amt;
    amt = enus.fromAnyCurrency("USD 12.34", &code);
    qDebug() << "en_US fromAnyCurrency(USD)" << amt << code;
    code.clear();
    amt = enus.fromAnyCurrency("GBP 12.34", &code);
    qDebug() << "en_US fromAnyCurrency(GBP)" << amt << code;
    code.clear();
    amt = enus.fromAnyCurrency("$ 12.34", &code);
    qDebug() << "en_US fromAnyCurrency($)" << amt << code;
    code.clear();
    amt = enus.fromAnyCurrency("£ 12.34", &code);
    qDebug() << "en_US fromAnyCurrency(£)" << amt << code;

    qDebug() << "";
    qDebug() << "Germany";
    QNumberFormatter dede(QNumberFormatter::DecimalStyle, "de_DE");
    qDebug() << "de_DE isValid()" << dede.isValid();
    qDebug() << "de_DE isInvalid()" << dede.isInvalid();
    qDebug() << "de_DE localeCode()" << dede.localeCode();
    qDebug() << "de_DE localeCode(EffectiveLocaleCode)" << dede.localeCode(QLocale::EffectiveLocaleCode);
    qDebug() << "de_DE numberStyle()" << dede.numberStyle();
    qDebug() << "de_DE toString()" << dede.toString(12345);
    qDebug() << "de_DE toInt32()" << dede.toInt32("12,345");
    qDebug() << "de_DE toInt32()" << dede.toInt32("12.345");
    qDebug() << "de_DE toInt32()" << dede.toInt32("12345");

    qDebug() << "";
    qDebug() << "CurrencyStyle";
    QNumberFormatter enusCurr("en_US", QNumberFormatter::CurrencyStyle);
    qDebug() << "en_US isValid()" << enusCurr.isValid();
    qDebug() << "en_US currencyCode()" << enusCurr.currencyCode();
    qDebug() << "en_US toString()" << enusCurr.toString(123.45);
    qDebug() << "en_US toDouble()" << enusCurr.toDouble("$123.45");
    qDebug() << "en_US toAnyCurrency(USD)" << enusCurr.toAnyCurrency(12.34, "USD");
    qDebug() << "en_US toAnyCurrency(GBP)" << enusCurr.toAnyCurrency(12.34, "GBP");
    code.clear();
    amt = enusCurr.fromAnyCurrency("USD 12.34", &code);
    qDebug() << "en_US fromAnyCurrency(USD)" << amt << code;
    code.clear();
    amt = enusCurr.fromAnyCurrency("GBP 12.34", &code);
    qDebug() << "en_US fromAnyCurrency(GBP)" << amt << code;
    code.clear();
    amt = enusCurr.fromAnyCurrency("$ 12.34", &code);
    qDebug() << "en_US fromAnyCurrency($)" << amt << code;
    code.clear();
    amt = enusCurr.fromAnyCurrency("£ 12.34", &code);
    qDebug() << "en_US fromAnyCurrency(£)" << amt << code;

    qDebug() << "";
    qDebug() << "CurrencyCodeStyle";
    QNumberFormatter enusCurrCode(QNumberFormatter::CurrencyCodeStyle, "en_US");
    qDebug() << "en_US isValid()" << enusCurrCode.isValid();
    qDebug() << "en_US currencyCode()" << enusCurrCode.currencyCode();
    qDebug() << "en_US toString()" << enusCurrCode.toString(123.45);
    qDebug() << "en_US toDouble()" << enusCurrCode.toDouble("USD 123.45");
    qDebug() << "en_US toAnyCurrency(USD)" << enusCurrCode.toAnyCurrency(12.34, "USD");
    qDebug() << "en_US toAnyCurrency(GBP)" << enusCurrCode.toAnyCurrency(12.34, "GBP");
    code.clear();
    amt = enusCurrCode.fromAnyCurrency("USD 12.34", &code);
    qDebug() << "en_US fromAnyCurrency(USD)" << amt << code;
    code.clear();
    amt = enusCurrCode.fromAnyCurrency("GBP 12.34", &code);
    qDebug() << "en_US fromAnyCurrency(GBP)" << amt << code;
    code.clear();
    amt = enusCurrCode.fromAnyCurrency("$ 12.34", &code);
    qDebug() << "en_US fromAnyCurrency($)" << amt << code;
    code.clear();
    amt = enusCurrCode.fromAnyCurrency("£ 12.34", &code);
    qDebug() << "en_US fromAnyCurrency(£)" << amt << code;

    qDebug() << "";
    qDebug() << "CurrencyNameStyle";
    QNumberFormatter enusCurrName(QNumberFormatter::CurrencyNameStyle, "en_US");
    qDebug() << "en_US isValid()" << enusCurrName.isValid();
    qDebug() << "en_US currencyCode()" << enusCurrName.currencyCode();
    qDebug() << "en_US toString()" << enusCurrName.toString(123.45);
    qDebug() << "en_US toDouble()" << enusCurrName.toDouble("123.45 US dollars");
    qDebug() << "en_US toAnyCurrency(USD)" << enusCurrName.toAnyCurrency(12.34, "USD");
    qDebug() << "en_US toAnyCurrency(GBP)" << enusCurrName.toAnyCurrency(12.34, "GBP");
    code.clear();
    amt = enusCurrName.fromAnyCurrency("USD 12.34", &code);
    qDebug() << "en_US fromAnyCurrency(USD)" << amt << code;
    code.clear();
    amt = enusCurrName.fromAnyCurrency("GBP 12.34", &code);
    qDebug() << "en_US fromAnyCurrency(GBP)" << amt << code;
    code.clear();
    amt = enusCurrName.fromAnyCurrency("$ 12.34", &code);
    qDebug() << "en_US fromAnyCurrency($)" << amt << code;
    code.clear();
    amt = enusCurrName.fromAnyCurrency("£ 12.34", &code);
    qDebug() << "en_US fromAnyCurrency(£)" << amt << code;

    qDebug() << "";
    qDebug() << "PercentageStyle";
    QNumberFormatter enusPerc(QNumberFormatter::PercentageStyle, "en_US");
    qDebug() << "en_US isValid()" << enusPerc.isValid();
    qDebug() << "en_US toString()" << enusPerc.toString(1.2345);
    qDebug() << "en_US toDouble()" << enusPerc.toDouble("123.45%");

    qDebug() << "";
    qDebug() << "ScientificStyle";
    QNumberFormatter enusSci(QNumberFormatter::ScientificStyle, "en_US");
    qDebug() << "en_US isValid()" << enusSci.isValid();
    qDebug() << "en_US toString()" << enusSci.toString(12345);
    qDebug() << "en_US toDouble()" << enusSci.toDouble("1.2345E4");

    qDebug() << "";
    qDebug() << "OrdinalStyle";
    QNumberFormatter enusOrd(QNumberFormatter::OrdinalStyle, "en_US");
    qDebug() << "en_US isValid()" << enusOrd.isValid();
    qDebug() << "en_US toString()" << enusOrd.toString(12345);
    qDebug() << "en_US toInt32()" << enusOrd.toInt32("12,345th");

    qDebug() << "";
    qDebug() << "DurationStyle";
    QNumberFormatter enusDur(QNumberFormatter::DurationStyle, "en_US");
    qDebug() << "en_US isValid()" << enusDur.isValid();
    qDebug() << "en_US toString()" << enusDur.toString(12345);
    qDebug() << "en_US toInt32()" << enusDur.toInt32("3:25:45");

    qDebug() << "";
    qDebug() << "SpelloutStyle";
    QNumberFormatter enusSpell(QNumberFormatter::SpelloutStyle, "en_US");
    qDebug() << "en_US isValid()" << enusSpell.isValid();
    qDebug() << "en_US toString()" << enusSpell.toString(12345);
    qDebug() << "en_US toInt32()" << enusSpell.toInt32("twelve thousand three hundred forty-five");
}

QTEST_MAIN(tst_QNumberFormatter)

#include "tst_qnumberformatter.moc"
