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

#include "qlocale.h"
#include "qlocale_p.h"

#include "qglobal.h"
#include "qdatastream.h"
#include "qdebug.h"
#include "qstring.h"
#include "qdatetime.h"
#include "qstringlist.h"
#include "qstringbuilder.h"

QT_BEGIN_NAMESPACE

// Singleton class to hold global default Decimal Number Formatter for efficiency
class QLocaleSingleton
{
public:
    QLocaleSingleton() {
        default = QLocalePrivate();
#ifndef QT_NO_SYSTEMLOCALE
        system = default;
#endif
    }

#ifndef QT_NO_SYSTEMLOCALE
    QSharedDataPointer<QLocalePrivate> system;
#endif
    QSharedDataPointer<QLocalePrivate> default;
};

Q_GLOBAL_STATIC(QLocaleSingleton, global_locale);

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &ds, const QLocale &l)
{
    ds << l.name();
    return ds;
}

QDataStream &operator>>(QDataStream &ds, QLocale &l)
{
    QString s;
    ds >> s;
    l = QLocale(s);
    return ds;
}
#endif // QT_NO_DATASTREAM

/*!
 \internal
*/
QLocale::QLocale(QLocalePrivate &dd)
    : d(&dd)
{}


/*!
    Constructs a QLocale object with the specified \a name,
    which has the format
    "language[_script][_country][.codeset][@modifier]" or "C", where:

    \list
    \li language is a lowercase, two-letter, ISO 639 language code (also some three-letter codes),
    \li script is a titlecase, four-letter, ISO 15924 script code,
    \li country is an uppercase, two-letter, ISO 3166 country code (also "419" as defined by United Nations),
    \li and codeset and modifier are ignored.
    \endlist

    The separator can be either underscore or a minus sign.

    If the string violates the locale format, or language is not
    a valid ISO 639 code, the "C" locale is used instead. If country
    is not present, or is not a valid ISO 3166 code, the most
    appropriate country is chosen for the specified language.

    The language, script and country codes are converted to their respective
    \c Language, \c Script and \c Country enums. After this conversion is
    performed, the constructor behaves exactly like QLocale(Country, Script,
    Language).

    This constructor is much slower than QLocale(Country, Script, Language).

    \sa bcp47Name()
*/

QLocale::QLocale(const QString &name)
    : d(name)
{
}

/*!
    Constructs a QLocale object initialized with the default locale. If
    no default locale was set using setDefault(), this locale will
    be the same as the one returned by system().

    \sa setDefault()
*/

QLocale::QLocale()
    : d(global_locale.default)
{
}

/*!
    Constructs a QLocale object with the specified \a language and \a
    country.

    \list
    \li If the language/country pair is found in the database, it is used.
    \li If the language is found but the country is not, or if the country
       is \c AnyCountry, the language is used with the most
       appropriate available country (for example, Germany for German),
    \li If neither the language nor the country are found, QLocale
       defaults to the default locale (see setDefault()).
    \endlist

    The language and country that are actually used can be queried
    using language() and country().

    \sa setDefault(), language(), country()
*/

QLocale::QLocale(Language language, Country country)
    : d(language, QLocale::AnyScript, country)
{
}

/*!
    \since 4.8

    Constructs a QLocale object with the specified \a language, \a script and
    \a country.

    \list
    \li If the language/script/country is found in the database, it is used.
    \li If both \a script is AnyScript and \a country is AnyCountry, the
       language is used with the most appropriate available script and country
       (for example, Germany for German),
    \li If either \a script is AnyScript or \a country is AnyCountry, the
       language is used with the first locale that matches the given \a script
       and \a country.
    \li If neither the language nor the country are found, QLocale
       defaults to the default locale (see setDefault()).
    \endlist

    The language, script and country that are actually used can be queried
    using language(), script() and country().

    \sa setDefault(), language(), script(), country()
*/

QLocale::QLocale(Language language, Script script, Country country)
    : d(language, script, country)
{
}

/*!
    Constructs a QLocale object as a copy of \a other.
*/

QLocale::QLocale(const QLocale &other)
{
    d = other.d;
}

/*!
    Destructor
*/

QLocale::~QLocale()
{
}

/*!
    Assigns \a other to this QLocale object and returns a reference
    to this QLocale object.
*/

QLocale &QLocale::operator=(const QLocale &other)
{
    d = other.d;
    return *this;
}

bool QLocale::operator==(const QLocale &other) const
{
    return d == other.d;
}

/*!
    \since 4.2

    Sets the \a options related to number conversions for this
    QLocale instance.
*/
void QLocale::setNumberOptions(NumberOptions options)
{
    d->m_numberOptions = options;
}

/*!
    \since 4.2

    Returns the options related to number conversions for this
    QLocale instance.

    By default, no options are set for the standard locales.
*/
QLocale::NumberOptions QLocale::numberOptions() const
{
    return static_cast<NumberOption>(d->m_numberOptions);
}

/*!
    \since 4.8

    Returns \a str quoted according to the current locale using the given
    quotation \a style.
*/
QString QLocale::quoteString(const QString &str, QuotationStyle style) const
{
    return d->m_backend->quoteString(&str, style);
}

/*!
    \since 4.8

    \overload
*/
QString QLocale::quoteString(const QStringRef &str, QuotationStyle style) const
{
    return d->m_backend->quoteString(str, style);
}

/*!
    \since 4.8

    Returns a string that represents a join of a given \a list of strings with
    a separator defined by the locale.
*/
QString QLocale::createSeparatedList(const QStringList &list) const
{
    return d->m_backend->createSeparatedList(list);
}

/*!
    \nonreentrant

    Sets the global default locale to \a locale. These
    values are used when a QLocale object is constructed with
    no arguments. If this function is not called, the system's
    locale is used.

    \warning In a multithreaded application, the default locale
    should be set at application startup, before any non-GUI threads
    are created.

    \sa system(), c()
*/

void QLocale::setDefault(const QLocale &locale)
{
    global_locale.default = locale.d;
}

/*!
    Returns the language of this locale.

    \sa script(), country(), languageToString(), bcp47Name()
*/
QLocale::Language QLocale::language() const
{
    return Language(d->languageId());
}

/*!
    \since 4.8

    Returns the script of this locale.

    \sa language(), country(), languageToString(), scriptToString(), bcp47Name()
*/
QLocale::Script QLocale::script() const
{
    return Script(d->scriptId());
}

/*!
    Returns the country of this locale.

    \sa language(), script(), countryToString(), bcp47Name()
*/
QLocale::Country QLocale::country() const
{
    return Country(d->countryId());
}

/*!
    Returns the language and country of this locale as a
    string of the form "language_country", where
    language is a lowercase, two-letter ISO 639 language code,
    and country is an uppercase, two- or three-letter ISO 3166 country code.

    Note that even if QLocale object was constructed with an explicit script,
    name() will not contain it for compatibility reasons. Use bcp47Name() instead
    if you need a full locale name.

    \sa QLocale(), language(), script(), country(), bcp47Name()
*/

QString QLocale::name() const
{
    Language l = language();

    QString result = d->languageCode();

    if (l == C)
        return result;

    Country c = country();
    if (c == AnyCountry)
        return result;

    result.append(QLatin1Char('_'));
    result.append(d->countryCode());

    return result;
}

/*!
    \since 4.8

    Returns the dash-separated language, script and country (and possibly other BCP47 fields)
    of this locale as a string.

    Unlike the uiLanguages() the returned value of the bcp47Name() represents
    the locale name of the QLocale data but not the language the user-interface
    should be in.

    This function tries to conform the locale name to BCP47.

    \sa language(), country(), script(), uiLanguages()
*/
QString QLocale::bcp47Name() const
{
    return QString::fromLatin1(d->bcp47Name());
}

/*!
    Returns a QString containing the name of \a language.

    \sa countryToString(), scriptToString(), bcp47Name()
*/

QString QLocale::languageToString(Language language)
{
    return d->m_backend->languageToString(language);
}

/*!
    Returns a QString containing the name of \a country.

    \sa languageToString(), scriptToString(), country(), bcp47Name()
*/

QString QLocale::countryToString(Country country)
{
    returnd->m_backend->countryToString(country);
}

/*!
    \since 4.8

    Returns a QString containing the name of \a script.

    \sa languageToString(), countryToString(), script(), bcp47Name()
*/
QString QLocale::scriptToString(QLocale::Script script)
{
    return d->m_backend->scriptToString(script);
}

/*!
    Returns the short int represented by the localized string \a s.

    If the conversion fails the function returns 0.

    If \a ok is not 0, failure is reported by setting *ok to false, and
    success by setting *ok to true.

    This function ignores leading and trailing whitespace.

    \sa toUShort(), toString()
*/

short QLocale::toShort(const QString &s, bool *ok) const
{
    qlonglong i = toLongLong(s, ok);
    if (i < SHRT_MIN || i > SHRT_MAX) {
        if (ok != 0)
            *ok = false;
        return 0;
    }
    return short(i);
}

/*!
    Returns the unsigned short int represented by the localized string \a s.

    If the conversion fails the function returns 0.

    If \a ok is not 0, failure is reported by setting *ok to false, and
    success by setting *ok to true.

    This function ignores leading and trailing whitespace.

    \sa toShort(), toString()
*/

ushort QLocale::toUShort(const QString &s, bool *ok) const
{
    qulonglong i = toULongLong(s, ok);
    if (i > USHRT_MAX) {
        if (ok != 0)
            *ok = false;
        return 0;
    }
    return ushort(i);
}

/*!
    Returns the int represented by the localized string \a s.

    If the conversion fails the function returns 0.

    If \a ok is not 0, failure is reported by setting *ok to false, and
    success by setting *ok to true.

    This function ignores leading and trailing whitespace.

    \sa toUInt(), toString()
*/

int QLocale::toInt(const QString &s, bool *ok) const
{
    qlonglong i = toLongLong(s, ok);
    if (i < INT_MIN || i > INT_MAX) {
        if (ok != 0)
            *ok = false;
        return 0;
    }
    return int(i);
}

/*!
    Returns the unsigned int represented by the localized string \a s.

    If the conversion fails the function returns 0.

    If \a ok is not 0, failure is reported by setting *ok to false, and
    success by setting *ok to true.

    This function ignores leading and trailing whitespace.

    \sa toInt(), toString()
*/

uint QLocale::toUInt(const QString &s, bool *ok) const
{
    qulonglong i = toULongLong(s, ok);
    if (i > UINT_MAX) {
        if (ok != 0)
            *ok = false;
        return 0;
    }
    return uint(i);
}

/*!
    Returns the long long int represented by the localized string \a s.

    If the conversion fails the function returns 0.

    If \a ok is not 0, failure is reported by setting *ok to false, and
    success by setting *ok to true.

    This function ignores leading and trailing whitespace.

    \sa toInt(), toULongLong(), toDouble(), toString()
*/


qlonglong QLocale::toLongLong(const QString &s, bool *ok) const
{
    QLocalePrivate::GroupSeparatorMode mode = d->m_numberOptions & RejectGroupSeparator
                                            ? QLocalePrivate::FailOnGroupSeparators
                                            : QLocalePrivate::ParseGroupSeparators;

    return d->m_backend->toInt64(s, ok, mode);
}

/*!
    Returns the unsigned long long int represented by the localized
    string \a s.

    If the conversion fails the function returns 0.

    If \a ok is not 0, failure is reported by setting *ok to false, and
    success by setting *ok to true.

    This function ignores leading and trailing whitespace.

    \sa toLongLong(), toInt(), toDouble(), toString()
*/

qulonglong QLocale::toULongLong(const QString &s, bool *ok) const
{
    QLocalePrivate::GroupSeparatorMode mode = d->m_numberOptions & RejectGroupSeparator
                                            ? QLocalePrivate::FailOnGroupSeparators
                                            : QLocalePrivate::ParseGroupSeparators;

    return d->m_backend->toUint64(s, ok, mode);
}

/*!
    Returns the float represented by the localized string \a s, or 0.0
    if the conversion failed.

    If \a ok is not 0, reports failure by setting
    *ok to false and success by setting *ok to true.

    This function ignores leading and trailing whitespace.

    \sa toDouble(), toInt(), toString()
*/

#define QT_MAX_FLOAT 3.4028234663852886e+38

float QLocale::toFloat(const QString &s, bool *ok) const
{
    bool myOk;
    double d = toDouble(s, &myOk);
    if (!myOk || d > QT_MAX_FLOAT || d < -QT_MAX_FLOAT) {
        if (ok != 0)
            *ok = false;
        return 0.0;
    }
    if (ok != 0)
        *ok = true;
    return float(d);
}

/*!
    Returns the double represented by the localized string \a s, or
    0.0 if the conversion failed.

    If \a ok is not 0, reports failure by setting
    *ok to false and success by setting *ok to true.

    Unlike QString::toDouble(), this function does not fall back to
    the "C" locale if the string cannot be interpreted in this
    locale.

    \snippet code/src_corelib_tools_qlocale.cpp 3

    Notice that the last conversion returns 1234.0, because '.' is the
    thousands group separator in the German locale.

    This function ignores leading and trailing whitespace.

    \sa toFloat(), toInt(), toString()
*/

double QLocale::toDouble(const QString &s, bool *ok) const
{
    QLocalePrivate::GroupSeparatorMode mode = d->m_numberOptions & RejectGroupSeparator
                                            ? QLocalePrivate::FailOnGroupSeparators
                                            : QLocalePrivate::ParseGroupSeparators;

    return d->m_backend->toDouble(s, ok, mode);
}

/*!
    Returns the short int represented by the localized string \a s.

    If the conversion fails the function returns 0.

    If \a ok is not null, failure is reported by setting *ok to false, and
    success by setting *ok to true.

    This function ignores leading and trailing whitespace.

    \sa toUShort(), toString()

    \since 5.1
*/

short QLocale::toShort(const QStringRef &s, bool *ok) const
{
    qlonglong i = toLongLong(s, ok);
    if (i < SHRT_MIN || i > SHRT_MAX) {
        if (ok)
            *ok = false;
        return 0;
    }
    return short(i);
}

/*!
    Returns the unsigned short int represented by the localized string \a s.

    If the conversion fails the function returns 0.

    If \a ok is not null, failure is reported by setting *ok to false, and
    success by setting *ok to true.

    This function ignores leading and trailing whitespace.

    \sa toShort(), toString()

    \since 5.1
*/

ushort QLocale::toUShort(const QStringRef &s, bool *ok) const
{
    qulonglong i = toULongLong(s, ok);
    if (i > USHRT_MAX) {
        if (ok)
            *ok = false;
        return 0;
    }
    return ushort(i);
}

/*!
    Returns the int represented by the localized string \a s.

    If the conversion fails the function returns 0.

    If \a ok is not null, failure is reported by setting *ok to false, and
    success by setting *ok to true.

    This function ignores leading and trailing whitespace.

    \sa toUInt(), toString()

    \since 5.1
*/

int QLocale::toInt(const QStringRef &s, bool *ok) const
{
    qlonglong i = toLongLong(s, ok);
    if (i < INT_MIN || i > INT_MAX) {
        if (ok)
            *ok = false;
        return 0;
    }
    return int(i);
}

/*!
    Returns the unsigned int represented by the localized string \a s.

    If the conversion fails the function returns 0.

    If \a ok is not null, failure is reported by setting *ok to false, and
    success by setting *ok to true.

    This function ignores leading and trailing whitespace.

    \sa toInt(), toString()

    \since 5.1
*/

uint QLocale::toUInt(const QStringRef &s, bool *ok) const
{
    qulonglong i = toULongLong(s, ok);
    if (i > UINT_MAX) {
        if (ok)
            *ok = false;
        return 0;
    }
    return uint(i);
}

/*!
    Returns the long long int represented by the localized string \a s.

    If the conversion fails the function returns 0.

    If \a ok is not null, failure is reported by setting *ok to false, and
    success by setting *ok to true.

    This function ignores leading and trailing whitespace.

    \sa toInt(), toULongLong(), toDouble(), toString()

    \since 5.1
*/


qlonglong QLocale::toLongLong(const QStringRef &s, bool *ok) const
{
    QLocalePrivate::GroupSeparatorMode mode = d->m_numberOptions & RejectGroupSeparator
                                            ? QLocalePrivate::FailOnGroupSeparators
                                            : QLocalePrivate::ParseGroupSeparators;

    return d->m_backend->toInt64(s, ok, mode);
}

/*!
    Returns the unsigned long long int represented by the localized
    string \a s.

    If the conversion fails the function returns 0.

    If \a ok is not null, failure is reported by setting *ok to false, and
    success by setting *ok to true.

    This function ignores leading and trailing whitespace.

    \sa toLongLong(), toInt(), toDouble(), toString()

    \since 5.1
*/

qulonglong QLocale::toULongLong(const QStringRef &s, bool *ok) const
{
    QLocalePrivate::GroupSeparatorMode mode = d->m_numberOptions & RejectGroupSeparator
                                            ? QLocalePrivate::FailOnGroupSeparators
                                            : QLocalePrivate::ParseGroupSeparators;

    return d->m_backend->toUint64(s, ok, mode);
}

/*!
    Returns the float represented by the localized string \a s, or 0.0
    if the conversion failed.

    If \a ok is not null, reports failure by setting
    *ok to false and success by setting *ok to true.

    This function ignores leading and trailing whitespace.

    \sa toDouble(), toInt(), toString()

    \since 5.1
*/

float QLocale::toFloat(const QStringRef &s, bool *ok) const
{
    bool myOk;
    double d = toDouble(s, &myOk);
    if (!myOk || d > QT_MAX_FLOAT || d < -QT_MAX_FLOAT) {
        if (ok)
            *ok = false;
        return 0.0;
    }
    if (ok)
        *ok = true;
    return float(d);
}

/*!
    Returns the double represented by the localized string \a s, or
    0.0 if the conversion failed.

    If \a ok is not null, reports failure by setting
    *ok to false and success by setting *ok to true.

    Unlike QString::toDouble(), this function does not fall back to
    the "C" locale if the string cannot be interpreted in this
    locale.

    \snippet code/src_corelib_tools_qlocale.cpp 3

    Notice that the last conversion returns 1234.0, because '.' is the
    thousands group separator in the German locale.

    This function ignores leading and trailing whitespace.

    \sa toFloat(), toInt(), toString()

    \since 5.1
*/

double QLocale::toDouble(const QStringRef &s, bool *ok) const
{
    QLocalePrivate::GroupSeparatorMode mode = d->m_numberOptions & RejectGroupSeparator
                                            ? QLocalePrivate::FailOnGroupSeparators
                                            : QLocalePrivate::ParseGroupSeparators;

    return d->m_backend->toDouble(s, ok, mode);
}


/*!
    Returns a localized string representation of \a i.

    \sa toLongLong()
*/

QString QLocale::toString(qlonglong i) const
{
    QLocalePrivate::Flags flags = d->m_numberOptions & OmitGroupSeparator
                                ? QLocalePrivate::NoFlags
                                : QLocalePrivate::ThousandsGroup;

    return d->m_backend->toString(i, flags);
}

/*!
    \overload

    \sa toULongLong()
*/

QString QLocale::toString(qulonglong i) const
{
    QLocalePrivate::Flags flags = d->m_numberOptions & OmitGroupSeparator
                                ? QLocalePrivate::NoFlags
                                : QLocalePrivate::ThousandsGroup;

    return d->m_backend->toString(i, flags);
}

/*!
    \overload

    \a f and \a prec have the same meaning as in QString::number(double, char, int).

    \sa toDouble()
*/

QString QLocale::toString(double i, char f, int prec) const
{
    QLocalePrivate::DoubleForm form = QLocalePrivate::DFDecimal;
    QLocalePrivate::Flags flags = QLocalePrivate::NoFlags;

    if (f >= 'A' && f <= 'Z') {
        flags = QLocalePrivate::CapitalEorX;
        f = f - 'A' + 'a';
    }

    switch (f) {
        case 'f':
            form = QLocalePrivate::DFDecimal;
            break;
        case 'e':
            form = QLocalePrivate::DFExponent;
            break;
        case 'g':
            form = QLocalePrivate::DFSignificantDigits;
            break;
        default:
            break;
    }

    if (!(d->m_numberOptions & OmitGroupSeparator))
        flags |= QLocalePrivate::ThousandsGroup;

    return d->m_backend->doubleToString(i, prec, form, flags);
}

/*!
    Returns a localized string representation of the given \a date in the
    specified \a format.
    If \a format is an empty string, an empty string is returned.
*/

QString QLocale::toString(const QDate &date, const QString &format) const
{
    if (!date.isValid())
        return QString();

    return d->m_backend->toString(date, format);
}

/*!
    Returns a localized string representation of the given \a date according
    to the specified \a format.
*/

QString QLocale::toString(const QDate &date, FormatType format) const
{
    if (!date.isValid())
        return QString();

    return d->m_backend->toString(date, format);
}

/*!
    Returns a localized string representation of the given \a time according
    to the specified \a format.
    If \a format is an empty string, an empty string is returned.
*/
QString QLocale::toString(const QTime &time, const QString &format) const
{
    if (!time.isValid())
        return QString();

    return d->m_backend->toString(time, format);
}

/*!
    Returns a localized string representation of the given \a time in the
    specified \a format.
*/

QString QLocale::toString(const QTime &time, FormatType format) const
{
    if (!time.isValid())
        return QString();

    return d->m_backend->toString(time, format);
}

/*!
    \since 4.4

    Returns a localized string representation of the given \a dateTime according
    to the specified \a format.
    If \a format is an empty string, an empty string is returned.
*/

QString QLocale::toString(const QDateTime &dateTime, const QString &format) const
{
    if (!dateTime.isValid())
        return QString();

    return d->m_backend->toString(dateTime, format);
}

/*!
    \since 4.4

    Returns a localized string representation of the given \a dateTime according
    to the specified \a format.
*/

QString QLocale::toString(const QDateTime &dateTime, FormatType format) const
{
    if (!dateTime.isValid())
        return QString();

    return d->m_backend->toString(dateTime, format);
}

/*!
    \since 4.1

    Returns the date format used for the current locale.

    If \a format is LongFormat the format will be a long version.
    Otherwise it uses a shorter version.

    \sa QDate::toString(), QDate::fromString()
*/

QString QLocale::dateFormat(FormatType format) const
{
    return d->m_backend->dateFormat(format);
}

/*!
    \since 4.1

    Returns the time format used for the current locale.

    If \a format is LongFormat the format will be a long version.
    Otherwise it uses a shorter version.

    \sa QTime::toString(), QTime::fromString()
*/

QString QLocale::timeFormat(FormatType format) const
{
    return d->m_backend->timeFormat(format);
}

/*!
    \since 4.4

    Returns the date time format used for the current locale.

    If \a format is ShortFormat the format will be a short version.
    Otherwise it uses a longer version.

    \sa QDateTime::toString(), QDateTime::fromString()
*/

QString QLocale::dateTimeFormat(FormatType format) const
{
    return d->m_backend->dateTimeFormat(format);
}

/*!
    \since 4.4

    Parses the time string given in \a string and returns the
    time. The format of the time string is chosen according to the
    \a format parameter (see timeFormat()).

    If the time could not be parsed, returns an invalid time.

    \sa timeFormat(), toDate(), toDateTime(), QTime::fromString()
*/
#ifndef QT_NO_DATESTRING
QTime QLocale::toTime(const QString &string, FormatType format) const
{
    return d->m_backend->toTime(string, format);
}
#endif

/*!
    \since 4.4

    Parses the date string given in \a string and returns the
    date. The format of the date string is chosen according to the
    \a format parameter (see dateFormat()).

    If the date could not be parsed, returns an invalid date.

    \sa dateFormat(), toTime(), toDateTime(), QDate::fromString()
*/
#ifndef QT_NO_DATESTRING
QDate QLocale::toDate(const QString &string, FormatType format) const
{
    return d->m_backend->toDate(string, format);
}
#endif

/*!
    \since 4.4

    Parses the date/time string given in \a string and returns the
    time. The format of the date/time string is chosen according to the
    \a format parameter (see dateTimeFormat()).

    If the string could not be parsed, returns an invalid QDateTime.

    \sa dateTimeFormat(), toTime(), toDate(), QDateTime::fromString()
*/

#ifndef QT_NO_DATESTRING
QDateTime QLocale::toDateTime(const QString &string, FormatType format) const
{
    return d->m_backend->toDateTime(string, format);
}
#endif

/*!
    \since 4.4

    Parses the time string given in \a string and returns the
    time. See QTime::fromString() for information on what is a valid
    format string.

    If the time could not be parsed, returns an invalid time.

    \sa timeFormat(), toDate(), toDateTime(), QTime::fromString()
*/
#ifndef QT_NO_DATESTRING
QTime QLocale::toTime(const QString &string, const QString &format) const
{
    return d->m_backend->toTime(string, format);
}
#endif

/*!
    \since 4.4

    Parses the date string given in \a string and returns the
    date. See QDate::fromString() for information on the expressions
    that can be used with this function.

    This function searches month names and the names of the days of
    the week in the current locale.

    If the date could not be parsed, returns an invalid date.

    \sa dateFormat(), toTime(), toDateTime(), QDate::fromString()
*/
#ifndef QT_NO_DATESTRING
QDate QLocale::toDate(const QString &string, const QString &format) const
{
    return d->m_backend->toDate(string, format);
}
#endif

/*!
    \since 4.4

    Parses the date/time string given in \a string and returns the
    time.  See QDateTime::fromString() for information on the expressions
    that can be used with this function.

    \note The month and day names used must be given in the user's local
    language.

    If the string could not be parsed, returns an invalid QDateTime.

    \sa dateTimeFormat(), toTime(), toDate(), QDateTime::fromString()
*/
#ifndef QT_NO_DATESTRING
QDateTime QLocale::toDateTime(const QString &string, const QString &format) const
{
    return d->m_backend->toDateTime(string, format);
}
#endif


/*!
    \since 4.1

    Returns the decimal point character of this locale.
*/
QChar QLocale::decimalPoint() const
{
    return d->m_backend->decimalPoint();
}

/*!
    \since 4.1

    Returns the group separator character of this locale.
*/
QChar QLocale::groupSeparator() const
{
    return d->m_backend->groupSeparator();
}

/*!
    \since 4.1

    Returns the percent character of this locale.
*/
QChar QLocale::percent() const
{
    return d->m_backend->percent();
}

/*!
    \since 4.1

    Returns the zero digit character of this locale.
*/
QChar QLocale::zeroDigit() const
{
    return d->m_backend->zeroDigit();
}

/*!
    \since 4.1

    Returns the negative sign character of this locale.
*/
QChar QLocale::negativeSign() const
{
    return d->m_backend->negativeSign();
}

/*!
    \since 4.5

    Returns the positive sign character of this locale.
*/
QChar QLocale::positiveSign() const
{
    return d->m_backend->positiveSign();
}

/*!
    \since 4.1

    Returns the exponential character of this locale.
*/
QChar QLocale::exponential() const
{
    return d->m_backend->exponential();
}

/*!
    \fn QLocale QLocale::c()

    Returns a QLocale object initialized to the "C" locale.

    \sa system()
*/

/*!
    Returns a QLocale object initialized to the system locale.

    On Windows and Mac, this locale will use the decimal/grouping characters and date/time
    formats specified in the system configuration panel.

    \sa c()
*/

QLocale QLocale::system()
{
#ifndef QT_NO_SYSTEMLOCALE
    return QLocale(global_locale.system);
#else
    return QLocale(global_locale.default);
#endif
}


/*!
    \since 4.8

    Returns a list of valid locale objects that match the given \a language, \a
    script and \a country.

    Getting a list of all locales:
    QList<QLocale> allLocales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);

    Getting a list of locales suitable for Russia:
    QList<QLocale> locales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::Russia);
*/
QList<QLocale> QLocale::matchingLocales(QLocale::Language language,
                                        QLocale::Script script,
                                        QLocale::Country country)
{
    if (uint(language) > QLocale::LastLanguage 
        || uint(script) > QLocale::LastScript
        || uint(country) > QLocale::LastCountry)
        return QList<QLocale>();

    if (language == QLocale::C)
        return QList<QLocale>() << QLocale(QLocale::C);

    return d->m_backend->matchingLocales(language, script, country);
}

/*!
    \obsolete
    \since 4.3

    Returns the list of countries that have entries for \a language in Qt's locale
    database. If the result is an empty list, then \a language is not represented in
    Qt's locale database.

    \sa matchingLocales()
*/
QList<QLocale::Country> QLocale::countriesForLanguage(Language language)
{
    if (language == C)
        return QList<Country>() << AnyCountry;

    return d->m_backend->countriesForLanguage(language);
}

/*!
    \since 4.2

    Returns the localized name of \a month, in the format specified
    by \a type.

    \sa dayName(), standaloneMonthName()
*/
QString QLocale::monthName(int month, FormatType type) const
{
    if (month < 1 || month > 12)
        return QString();

    return d->m_backend->monthName(month, type);
}

/*!
    \since 4.5

    Returns the localized name of \a month that is used as a
    standalone text, in the format specified by \a type.

    If the locale information doesn't specify the standalone month
    name then return value is the same as in monthName().

    \sa monthName(), standaloneDayName()
*/
QString QLocale::standaloneMonthName(int month, FormatType type) const
{
    if (month < 1 || month > 12)
        return QString();

    return d->m_backend->standaloneMonthName(month, type);
}

/*!
    \since 4.2

    Returns the localized name of the \a day (where 1 represents
    Monday, 2 represents Tuesday and so on), in the format specified
    by \a type.

    \sa monthName(), standaloneDayName()
*/
QString QLocale::dayName(int day, FormatType type) const
{
    if (day < 1 || day > 7)
        return QString();

    return d->m_backend->dayName(day, type);
}

/*!
    \since 4.5

    Returns the localized name of the \a day (where 1 represents
    Monday, 2 represents Tuesday and so on) that is used as a
    standalone text, in the format specified by \a type.

    If the locale information does not specify the standalone day
    name then return value is the same as in dayName().

    \sa dayName(), standaloneMonthName()
*/
QString QLocale::standaloneDayName(int day, FormatType type) const
{
    if (day < 1 || day > 7)
        return QString();

    return d->m_backend->standaloneDayName(day, type);
}

/*!
    \since 4.8

    Returns the first day of the week according to the current locale.
*/
Qt::DayOfWeek QLocale::firstDayOfWeek() const
{
    return d->m_backend->firstDayOfWeek();
}

/*!
    \since 4.8

    Returns a list of days that are considered weekdays according to the current locale.
*/
QList<Qt::DayOfWeek> QLocale::weekdays() const
{
    return d->m_backend->weekdays();
}

/*!
    \since 4.4

    Returns the measurement system for the locale.
*/
QLocale::MeasurementSystem QLocale::measurementSystem() const
{
    return d->m_backend->measurementSystem();
}

/*!
  \since 4.7

  Returns the text direction of the language.
*/
Qt::LayoutDirection QLocale::textDirection() const
{
    switch (script()) {
    case QLocale::ArabicScript:
    case QLocale::AvestanScript:
    case QLocale::CypriotScript:
    case QLocale::HebrewScript:
    case QLocale::ImperialAramaicScript:
    case QLocale::InscriptionalPahlaviScript:
    case QLocale::InscriptionalParthianScript:
    case QLocale::KharoshthiScript:
    case QLocale::LydianScript:
    case QLocale::MandaeanScript:
    case QLocale::MeroiticCursiveScript:
    case QLocale::MeroiticScript:
    case QLocale::SamaritanScript:
    case QLocale::SyriacScript:
    case QLocale::ThaanaScript:
    case QLocale::NkoScript:
    case QLocale::OldSouthArabianScript:
    case QLocale::OrkhonScript:
    case QLocale::PhoenicianScript:
        return Qt::RightToLeft;
    default:
        break;
    }
    return Qt::LeftToRight;
}

/*!
  \since 4.8

  Returns an uppercase copy of \a str.
*/
QString QLocale::toUpper(const QString &str) const
{
    return d->m_backend->toUpper(str);
}

/*!
  \since 4.8

  Returns a lowercase copy of \a str.
*/
QString QLocale::toLower(const QString &str) const
{
    return d->m_backend->toLower(str);
}

/*!
    \since 4.5

    Returns the localized name of the "AM" suffix for times specified using
    the conventions of the 12-hour clock.

    \sa pmText()
*/
QString QLocale::amText() const
{
    return d->m_backend->amText();
}

/*!
    \since 4.5

    Returns the localized name of the "PM" suffix for times specified using
    the conventions of the 12-hour clock.

    \sa amText()
*/
QString QLocale::pmText() const
{
    return d->m_backend->amText();
}


/*!
    \since 4.8

    \enum QLocale::CurrencySymbolFormat

    Specifies the format of the currency symbol.

    \value CurrencyIsoCode a ISO-4217 code of the currency.
    \value CurrencySymbol a currency symbol.
    \value CurrencyDisplayName a user readable name of the currency.
*/

/*!
    \since 4.8
    Returns a currency symbol according to the \a format.
*/
QString QLocale::currencySymbol(QLocale::CurrencySymbolFormat format) const
{
    return d->m_backend->currencySymbol(format);
}

/*!
    \since 4.8

    Returns a localized string representation of \a value as a currency.
    If the \a symbol is provided it is used instead of the default currency symbol.

    \sa currencySymbol()
*/
QString QLocale::toCurrencyString(qlonglong value, const QString &symbol) const
{
    return d->m_backend->toCurrencyString(value, symbol);
}

/*!
    \since 4.8
    \overload
*/
QString QLocale::toCurrencyString(qulonglong value, const QString &symbol) const
{
    return d->m_backend->toCurrencyString(value, symbol);
}

/*!
    \since 4.8
    \overload
*/
QString QLocale::toCurrencyString(double value, const QString &symbol) const
{
    return d->m_backend->toCurrencyString(value, symbol);
}

/*!
    \since 4.8

    Returns an ordered list of locale names for translation purposes in
    preference order (like "en", "en-US", "en-Latn-US").

    The return value represents locale names that the user expects to see the
    UI translation in.

    Most like you do not need to use this function directly, but just pass the
    QLocale object to the QTranslator::load() function.

    The first item in the list is the most preferred one.

    \sa QTranslator, bcp47Name()
*/
QStringList QLocale::uiLanguages() const
{
    return d->m_backend->uiLanguages();
}

/*!
    \since 4.8

    Returns a native name of the language for the locale. For example
    "Schwiizertüütsch" for Swiss-German locale.

    \sa nativeCountryName(), languageToString()
*/
QString QLocale::nativeLanguageName() const
{
    return d->m_backend->nativeLanguageName();
}

/*!
    \since 4.8

    Returns a native name of the country for the locale. For example
    "España" for Spanish/Spain locale.

    \sa nativeLanguageName(), countryToString()
*/
QString QLocale::nativeCountryName() const
{
    return d->m_backend->nativeCountryName();
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QLocale &l)
{
    dbg.nospace() << "QLocale(" << qPrintable(QLocale::languageToString(l.language()))
                  << ", " << qPrintable(QLocale::scriptToString(l.script()))
                  << ", " << qPrintable(QLocale::countryToString(l.country())) << ')';
    return dbg.space();
}
#endif
QT_END_NAMESPACE
