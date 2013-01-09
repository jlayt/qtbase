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

#ifndef QLOCALECODE_H_P
#define QLOCALECODE_H_P

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

#include "qlocale.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QLocaleCodePrivate;

// Internal for now, make public later
// Implement full ICU support, parts may not be made public later

class Q_CORE_EXPORT QLocaleCode
{
public:
    // TODO Move these enums to Qt or QLocale

    // ICU ULocDataLocaleType
    enum LocaleCodeType {
        RequestedLocaleCode = 1, // The locale code the user requested
        EffectiveLocaleCode      // The locale code effectively being used
    };

    enum IsoCodeFormat {
        IsoAlpha2Code,
        IsoAlpha3Code,
        IsoNumericCode
    }

    // ICU UCurrCurrencyType
    enum CurrencyType {
        ActiveCurrency         = 0x01,
        ObsoleteCurrency       = 0x02,
        CirculatingCurrency    = 0x04,  // i.e. real money
        NonCirculatingCurrency = 0x08   // i.e metals, units, etc
    }
    Q_DECLARE_FLAGS(CurrencyTypes, CurrencyType);

    QLocaleCode();
    QLocaleCode(QLocale::Language language, QLocale::Country country);
    QLocaleCode(const QString &languageCode, const QString &countryCode, const QString &variant);
    QLocaleCode(const QString &locale);
    ~QLocaleCode();

    QLocaleCode(const QLocaleCode &other);
    QLocaleCode &operator=(const QLocaleCode &other);

    QString locale() const;
    QString localeBase() const;
    qint32 localeWindowsId();
    QString localeName(const QString &inLocale = QString()) const;

    bool isValid() const;
    bool isInvalid() const;

    QLocale::Language language() const;
    QString languageCode(QLocaleCode::IsoCodeFormat format = QLocaleCode::IsoAlpha2Code) const;
    QString languageName(const QString &inLocale = QString()) const;

    QLocale::Country country(const QString &locale) const;
    QString countryCode(QLocaleCode::IsoCodeFormat format = QLocaleCode::IsoAlpha2Code) const;
    QString countryName(const QString &inLocale = QString()) const;

    QLocale::Script script(const QString &locale) const;
    QString scriptCode(QLocaleCode::IsoCodeFormat format = QLocaleCode::IsoAlpha2Code) const;
    QString scriptName(const QString &inLocale = QString()) const;

    QString variant() const;
    QString variantName(const QString &inLocale = QString()) const;

    // keywords

    QStringList currencyCode(const QDate & date = QDate(),
                             QLocaleCode::IsoCodeFormat format = QLocaleCode::IsoAlpha3Code) const;
    QString currencySymbol(const QDate & date = QDate(), const QString &inLocale = QString()) const;
    QString currencyName(const QDate & date = QDate(), const QString &inLocale = QString(),
                         quint32 pluralCount = 1) const;

    Qt::LayoutDirection characterOrientation() const;
    Qt::LayoutDirection lineOrientation() const;

    static QString defaultLocale();
    static QString setDefaultLocale(const QString &locale);

    static QString systemLocale();

    static QStringList availableLocales();

    static QStringList languageCodes();
    static QStringList countryCodes();
    static QStringList scriptCodes();
    static QStringList currencyCodes(CurrencyTypes types
                                     = QLocaleCode::ActiveCurrency | QLocaleCode::CirculatingCurrency);

    static QString languageName(QLocale::Language language, const QString &inLocale);
    static QString languageName(const QString &languageCode, const QString &inLocale);

    static QString countryName(QLocale::Country country, const QString &inLocale);
    static QString countryName(const QString &countryCode, const QString &inLocale);

    static QString scriptName(QLocale::Script script, const QString &inLocale);
    static QString scriptName(const QString &scriptCode, const QString &inLocale);

    QStringList currencyCode(const QString &locale, const QDate & date = QDate(),
                             QLocaleCode::IsoCodeFormat format = QLocaleCode::IsoAlpha3Code);
    QString currencySymbol(const QString &currencyCode, const QString &inLocale);
    QString currencyName(const QString &currencyCode, const QString &inLocale, quint32 pluralCount = 1);
    qint32 currencyFractionDigits(const QString &currencyCode);
    double currencyRoundingIncrement(const QString &currencyCode);

    static QLocale::Language isoCodeToLanguage(const QString &isoCode);
    static QString languageToIsoCode(QLocale::Language language,
                                     QLocaleCode::IsoCodeFormat format = QLocaleCode::IsoAlpha2Code);

    static QLocale::Country isoCodeToCountry(const QString &isoCode);
    static QString countryToIsoCode(QLocale::Country country,
                                    QLocaleCode::IsoCodeFormat format = QLocaleCode::IsoAlpha2Code);

    static QLocale::Script isoCodeToScript(const QString &isoCode);
    static QString scriptToIsoCode(QLocale::Script script,
                                   QLocaleCode::IsoCodeFormat format = QLocaleCode::IsoAlpha2Code);

    static qint32 windowsLocaleId(const QString &locale);
    static QString fromWindowsLocaleId(qint32 localeId);

private:
    // Create locale with backend
    QLocaleCode(QLocaleCodePrivate &dd);
    friend class QLocaleCodePrivate;
    QSharedDataPointer<QLocaleCodePrivate> d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QLOCALECODE_H_P
