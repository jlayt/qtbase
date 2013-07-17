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

#if !defined(QWS) && defined(Q_OS_MAC)
#   include "private/qcore_mac_p.h"
#   include <CoreFoundation/CoreFoundation.h>
#endif

#include "qglobal.h"

#include "qplatformdefs.h"

#include "qdatastream.h"
#include "qdebug.h"
#include "qstring.h"
#include "qlocale.h"
#include "qlocale_p.h"
#include "qlocale_tools_p.h"
#include "qdatetime_p.h"
#include "qnamespace.h"
#include "qdatetime.h"
#include "qstringlist.h"
#include "qvariant.h"
#include "qstringbuilder.h"
#include "private/qnumeric_p.h"
#include "private/qsystemlibrary_p.h"
#ifdef Q_OS_WIN
#   include <qt_windows.h>
#   include <time.h>
#endif

QT_BEGIN_NAMESPACE

/******************************************************************************
** Helpers for accessing Qt locale database
*/

QT_BEGIN_INCLUDE_NAMESPACE
#include "qlocale_data_p.h"
QT_END_INCLUDE_NAMESPACE

// http://www.unicode.org/reports/tr35/#Likely_Subtags
static bool addLikelySubtags(QLocaleId &localeId)
{
    // ### optimize with bsearch
    const int likely_subtags_count = sizeof(likely_subtags) / sizeof(likely_subtags[0]);
    const QLocaleId *p = likely_subtags;
    const QLocaleId *const e = p + likely_subtags_count;
    for ( ; p < e; p += 2) {
        if (localeId == p[0]) {
            localeId = p[1];
            return true;
        }
    }
    return false;
}

QLocaleId QLocaleId::withLikelySubtagsAdded() const
{
    // language_script_region
    if (language_id || script_id || country_id) {
        QLocaleId id = QLocaleId::fromIds(language_id, script_id, country_id);
        if (addLikelySubtags(id))
            return id;
    }
    // language_script
    if (country_id) {
        QLocaleId id = QLocaleId::fromIds(language_id, script_id, 0);
        if (addLikelySubtags(id)) {
            id.country_id = country_id;
            return id;
        }
    }
    // language_region
    if (script_id) {
        QLocaleId id = QLocaleId::fromIds(language_id, 0, country_id);
        if (addLikelySubtags(id)) {
            id.script_id = script_id;
            return id;
        }
    }
    // language
    if (script_id && country_id) {
        QLocaleId id = QLocaleId::fromIds(language_id, 0, 0);
        if (addLikelySubtags(id)) {
            id.script_id = script_id;
            id.country_id = country_id;
            return id;
        }
    }
    return *this;
}

QLocaleId QLocaleId::withLikelySubtagsRemoved() const
{
    QLocaleId max = withLikelySubtagsAdded();
    // language
    {
        QLocaleId id = QLocaleId::fromIds(language_id, 0, 0);
        if (id.withLikelySubtagsAdded() == max)
            return id;
    }
    // language_region
    if (country_id) {
        QLocaleId id = QLocaleId::fromIds(language_id, 0, country_id);
        if (id.withLikelySubtagsAdded() == max)
            return id;
    }
    // language_script
    if (script_id) {
        QLocaleId id = QLocaleId::fromIds(language_id, script_id, 0);
        if (id.withLikelySubtagsAdded() == max)
            return id;
    }
    return max;
}

QByteArray QLocaleId::name(char separator) const
{
    if (language_id == QLocale::AnyLanguage)
        return QByteArray();
    if (language_id == QLocale::C)
        return QByteArrayLiteral("C");

    const unsigned char *lang = language_code_list + 3 * language_id;
    const unsigned char *script =
            (script_id != QLocale::AnyScript ? script_code_list + 4 * script_id : 0);
    const unsigned char *country =
            (country_id != QLocale::AnyCountry ? country_code_list + 3 * country_id : 0);
    char len = (lang[2] != 0 ? 3 : 2) + (script ? 4+1 : 0) + (country ? (country[2] != 0 ? 3 : 2)+1 : 0);
    QByteArray name(len, Qt::Uninitialized);
    char *uc = name.data();
    *uc++ = lang[0];
    *uc++ = lang[1];
    if (lang[2] != 0)
        *uc++ = lang[2];
    if (script) {
        *uc++ = separator;
        *uc++ = script[0];
        *uc++ = script[1];
        *uc++ = script[2];
        *uc++ = script[3];
    }
    if (country) {
        *uc++ = separator;
        *uc++ = country[0];
        *uc++ = country[1];
        if (country[2] != 0)
            *uc++ = country[2];
    }
    return name;
}

static QLocalePrivate *localePrivateByName(const QString &name)
{
    if (name == QLatin1String("C"))
        return &c_private;
    return QLocalePrivate::create(findLocaleData(name));
}

static QLocalePrivate *findLocalePrivate(QLocale::Language language, QLocale::Script script,
                                         QLocale::Country country)
{
    if (language == QLocale::C)
        return &c_private;

    const QLocaleData *data = QLocaleData::findLocaleData(language, script, country);

    int numberOptions = 0;

    // If not found, should default to system
    if (data->m_language_id == QLocale::C && language != QLocale::C) {
        numberOptions = default_number_options;
        data = defaultData();
    }
    return QLocalePrivate::create(data, numberOptions);
}

QLocalePrivate::QLocalePrivate(const QString &name)
    : d(localePrivateByName(name))
{
}

QLocalePrivate::QLocalePrivate(QLocale::Language language, QLocale::Script script, QLocale::Country country)
    : d(findLocalePrivate(language, script, country))
{
}

QLocalePrivate::QLocalePrivate(const QLocalePrivate &other)
{
    d = other.d;
}

QLocalePrivate::~QLocalePrivate()
{
}

QLocalePrivate &QLocalePrivate::operator=(const QLocalePrivate &other)
{
    d = other.d;
    return *this;
}

bool QLocalePrivate::operator==(const QLocale &other) const
{
    return d->m_data == other.d->m_data && d->m_numberOptions == other.d->m_numberOptions;
}

QLocale::Language QLocalePrivate::codeToLanguage(const QString &code)
{
    int len = code.length();
    if (len != 2 && len != 3)
        return QLocale::C;
    ushort uc1 = len-- > 0 ? code[0].toLower().unicode() : 0;
    ushort uc2 = len-- > 0 ? code[1].toLower().unicode() : 0;
    ushort uc3 = len-- > 0 ? code[2].toLower().unicode() : 0;

    const unsigned char *c = language_code_list;
    for (; *c != 0; c += 3) {
        if (uc1 == c[0] && uc2 == c[1] && uc3 == c[2])
            return QLocale::Language((c - language_code_list)/3);
    }

    // legacy codes
    if (uc1 == 'n' && uc2 == 'o' && uc3 == 0) { // no -> nb
        Q_STATIC_ASSERT(QLocale::Norwegian == QLocale::NorwegianBokmal);
        return QLocale::Norwegian;
    }
    if (uc1 == 't' && uc2 == 'l' && uc3 == 0) { // tl -> fil
        Q_STATIC_ASSERT(QLocale::Tagalog == QLocale::Filipino);
        return QLocale::Tagalog;
    }
    if (uc1 == 's' && uc2 == 'h' && uc3 == 0) { // sh -> sr[_Latn]
        Q_STATIC_ASSERT(QLocale::SerboCroatian == QLocale::Serbian);
        return QLocale::SerboCroatian;
    }
    if (uc1 == 'm' && uc2 == 'o' && uc3 == 0) { // mo -> ro
        Q_STATIC_ASSERT(QLocale::Moldavian == QLocale::Romanian);
        return QLocale::Moldavian;
    }

    return QLocale::C;
}

QLocale::Script QLocalePrivate::codeToScript(const QString &code)
{
    int len = code.length();
    if (len != 4)
        return QLocale::AnyScript;

    // script is titlecased in our data
    unsigned char c0 = code.at(0).toUpper().toLatin1();
    unsigned char c1 = code.at(1).toLower().toLatin1();
    unsigned char c2 = code.at(2).toLower().toLatin1();
    unsigned char c3 = code.at(3).toLower().toLatin1();

    const unsigned char *c = script_code_list;
    for (int i = 0; i < QLocale::LastScript; ++i, c += 4) {
        if (c0 == c[0] && c1 == c[1] && c2 == c[2] && c3 == c[3])
            return QLocale::Script(i);
    }
    return QLocale::AnyScript;
}

QLocale::Country QLocalePrivate::codeToCountry(const QString &code)
{
    int len = code.length();
    if (len != 2 && len != 3)
        return QLocale::AnyCountry;
    ushort uc1 = len-- > 0 ? code[0].toUpper().unicode() : 0;
    ushort uc2 = len-- > 0 ? code[1].toUpper().unicode() : 0;
    ushort uc3 = len-- > 0 ? code[2].toUpper().unicode() : 0;

    const unsigned char *c = country_code_list;
    for (; *c != 0; c += 3) {
        if (uc1 == c[0] && uc2 == c[1] && uc3 == c[2])
            return QLocale::Country((c - country_code_list)/3);
    }

    return QLocale::AnyCountry;
}

QString QLocalePrivate::languageToCode(QLocale::Language language)
{
    if (language == QLocale::AnyLanguage)
        return QString();
    if (language == QLocale::C)
        return QLatin1String("C");

    const unsigned char *c = language_code_list + 3*(uint(language));

    QString code(c[2] == 0 ? 2 : 3, Qt::Uninitialized);

    code[0] = ushort(c[0]);
    code[1] = ushort(c[1]);
    if (c[2] != 0)
        code[2] = ushort(c[2]);

    return code;
}

QString QLocalePrivate::scriptToCode(QLocale::Script script)
{
    if (script == QLocale::AnyScript || script > QLocale::LastScript)
        return QString();
    const unsigned char *c = script_code_list + 4*(uint(script));
    return QString::fromLatin1((const char *)c, 4);
}

QString QLocalePrivate::countryToCode(QLocale::Country country)
{
    if (country == QLocale::AnyCountry)
        return QString();

    const unsigned char *c = country_code_list + 3*(uint(country));

    QString code(c[2] == 0 ? 2 : 3, Qt::Uninitialized);

    code[0] = ushort(c[0]);
    code[1] = ushort(c[1]);
    if (c[2] != 0)
        code[2] = ushort(c[2]);

    return code;
}

QByteArray QLocalePrivate::bcp47Name(char separator) const
{
    if (m_data->m_language_id == QLocale::AnyLanguage)
        return QByteArray();
    if (m_data->m_language_id == QLocale::C)
        return QByteArrayLiteral("C");

    QLocaleId localeId = QLocaleId::fromIds(m_data->m_language_id, m_data->m_script_id, m_data->m_country_id);
    return localeId.withLikelySubtagsRemoved().name(separator);
}

static bool parse_locale_tag(const QString &input, int &i, QString *result, const QString &separators)
{
    *result = QString(8, Qt::Uninitialized); // worst case according to BCP47
    QChar *pch = result->data();
    const QChar *uc = input.data() + i;
    const int l = input.length();
    int size = 0;
    for (; i < l && size < 8; ++i, ++size) {
        if (separators.contains(*uc))
            break;
        if (! ((uc->unicode() >= 'a' && uc->unicode() <= 'z') ||
               (uc->unicode() >= 'A' && uc->unicode() <= 'Z') ||
               (uc->unicode() >= '0' && uc->unicode() <= '9')) ) // latin only
            return false;
        *pch++ = *uc++;
    }
    result->truncate(size);
    return true;
}

bool qt_splitLocaleName(const QString &name, QString &lang, QString &script, QString &cntry)
{
    const int length = name.length();

    lang = script = cntry = QString();

    const QString separators = QStringLiteral("_-.@");
    enum ParserState { NoState, LangState, ScriptState, CountryState };
    ParserState state = LangState;
    for (int i = 0; i < length && state != NoState; ) {
        QString value;
        if (!parse_locale_tag(name, i, &value, separators) ||value.isEmpty())
            break;
        QChar sep = i < length ? name.at(i) : QChar();
        switch (state) {
        case LangState:
            if (!sep.isNull() && !separators.contains(sep)) {
                state = NoState;
                break;
            }
            lang = value;
            if (i == length) {
                // just language was specified
                state = NoState;
                break;
            }
            state = ScriptState;
            break;
        case ScriptState: {
            QString scripts = QString::fromLatin1((const char *)script_code_list, sizeof(script_code_list));
            if (value.length() == 4 && scripts.indexOf(value) % 4 == 0) {
                // script name is always 4 characters
                script = value;
                state = CountryState;
            } else {
                // it wasn't a script, maybe it is a country then?
                cntry = value;
                state = NoState;
            }
            break;
        }
        case CountryState:
            cntry = value;
            state = NoState;
            break;
        case NoState:
            // shouldn't happen
            qWarning("QLocale: This should never happen");
            break;
        }
        ++i;
    }
    return lang.length() == 2 || lang.length() == 3;
}

void QLocalePrivate::getLangAndCountry(const QString &name, QLocale::Language &lang,
                                       QLocale::Script &script, QLocale::Country &cntry)
{
    lang = QLocale::C;
    script = QLocale::AnyScript;
    cntry = QLocale::AnyCountry;

    QString lang_code;
    QString script_code;
    QString cntry_code;
    if (!qt_splitLocaleName(name, lang_code, script_code, cntry_code))
        return;

    lang = QLocalePrivate::codeToLanguage(lang_code);
    if (lang == QLocale::C)
        return;
    script = QLocalePrivate::codeToScript(script_code);
    cntry = QLocalePrivate::codeToCountry(cntry_code);
}

QString qt_readEscapedFormatString(const QString &format, int *idx)
{
    int &i = *idx;

    Q_ASSERT(format.at(i) == QLatin1Char('\''));
    ++i;
    if (i == format.size())
        return QString();
    if (format.at(i).unicode() == '\'') { // "''" outside of a quoted stirng
        ++i;
        return QLatin1String("'");
    }

    QString result;

    while (i < format.size()) {
        if (format.at(i).unicode() == '\'') {
            if (i + 1 < format.size() && format.at(i + 1).unicode() == '\'') {
                // "''" inside of a quoted string
                result.append(QLatin1Char('\''));
                i += 2;
            } else {
                break;
            }
        } else {
            result.append(format.at(i++));
        }
    }
    if (i < format.size())
        ++i;

    return result;
}

int qt_repeatCount(const QString &s, int i)
{
    QChar c = s.at(i);
    int j = i + 1;
    while (j < s.size() && s.at(j) == c)
        ++j;
    return j - i;
}

static bool timeFormatContainsAP(const QString &format)
{
    int i = 0;
    while (i < format.size()) {
        if (format.at(i).unicode() == '\'') {
            qt_readEscapedFormatString(format, &i);
            continue;
        }

        if (format.at(i).toLower().unicode() == 'a')
            return true;

        ++i;
    }
    return false;
}

static QString timeZone()
{
#if defined(Q_OS_WINCE)
    TIME_ZONE_INFORMATION info;
    DWORD res = GetTimeZoneInformation(&info);
    if (res == TIME_ZONE_ID_UNKNOWN)
        return QString();
    return QString::fromWCharArray(info.StandardName);
#elif defined(Q_OS_WIN)
    _tzset();
# if defined(_MSC_VER) && _MSC_VER >= 1400
    size_t returnSize = 0;
    char timeZoneName[512];
    if (_get_tzname(&returnSize, timeZoneName, 512, 1))
        return QString();
    return QString::fromLocal8Bit(timeZoneName);
# else
    return QString::fromLocal8Bit(_tzname[1]);
# endif
#elif defined(Q_OS_VXWORKS)
    return QString();
#else
    tzset();
    return QString::fromLocal8Bit(tzname[1]);
#endif
}

QString QLocalePrivate::dateTimeToString(const QString &format, const QDate *date, const QTime *time,
                                         const QLocale *q) const
{
    Q_ASSERT(date || time);
    if ((date && !date->isValid()) || (time && !time->isValid()))
        return QString();
    const bool format_am_pm = time && timeFormatContainsAP(format);

    enum { AM, PM } am_pm = AM;
    int hour12 = time ? time->hour() : -1;
    if (time) {
        if (hour12 == 0) {
            am_pm = AM;
            hour12 = 12;
        } else if (hour12 < 12) {
            am_pm = AM;
        } else if (hour12 == 12) {
            am_pm = PM;
        } else {
            am_pm = PM;
            hour12 -= 12;
        }
    }

    QString result;

    int i = 0;
    while (i < format.size()) {
        if (format.at(i).unicode() == '\'') {
            result.append(qt_readEscapedFormatString(format, &i));
            continue;
        }

        const QChar c = format.at(i);
        int repeat = qt_repeatCount(format, i);
        bool used = false;
        if (date) {
            switch (c.unicode()) {
            case 'y':
                used = true;
                if (repeat >= 4)
                    repeat = 4;
                else if (repeat >= 2)
                    repeat = 2;

                switch (repeat) {
                case 4:
                    result.append(longLongToString(date->year(), -1, 10, 4, QLocalePrivate::ZeroPadded));
                    break;
                case 2:
                    result.append(longLongToString(date->year() % 100, -1, 10, 2,
                                                   QLocalePrivate::ZeroPadded));
                    break;
                default:
                    repeat = 1;
                    result.append(c);
                    break;
                }
                break;

            case 'M':
                used = true;
                repeat = qMin(repeat, 4);
                switch (repeat) {
                case 1:
                    result.append(longLongToString(date->month()));
                    break;
                case 2:
                    result.append(longLongToString(date->month(), -1, 10, 2, QLocalePrivate::ZeroPadded));
                    break;
                case 3:
                    result.append(q->monthName(date->month(), QLocale::ShortFormat));
                    break;
                case 4:
                    result.append(q->monthName(date->month(), QLocale::LongFormat));
                    break;
                }
                break;

            case 'd':
                used = true;
                repeat = qMin(repeat, 4);
                switch (repeat) {
                case 1:
                    result.append(longLongToString(date->day()));
                    break;
                case 2:
                    result.append(longLongToString(date->day(), -1, 10, 2, QLocalePrivate::ZeroPadded));
                    break;
                case 3:
                    result.append(q->dayName(date->dayOfWeek(), QLocale::ShortFormat));
                    break;
                case 4:
                    result.append(q->dayName(date->dayOfWeek(), QLocale::LongFormat));
                    break;
                }
                break;

            default:
                break;
            }
        }
        if (!used && time) {
            switch (c.unicode()) {
            case 'h': {
                used = true;
                repeat = qMin(repeat, 2);
                const int hour = format_am_pm ? hour12 : time->hour();

                switch (repeat) {
                case 1:
                    result.append(longLongToString(hour));
                    break;
                case 2:
                    result.append(longLongToString(hour, -1, 10, 2, QLocalePrivate::ZeroPadded));
                    break;
                }
                break;
            }
            case 'H':
                used = true;
                repeat = qMin(repeat, 2);
                switch (repeat) {
                case 1:
                    result.append(longLongToString(time->hour()));
                    break;
                case 2:
                    result.append(longLongToString(time->hour(), -1, 10, 2, QLocalePrivate::ZeroPadded));
                    break;
                }
                break;

            case 'm':
                used = true;
                repeat = qMin(repeat, 2);
                switch (repeat) {
                case 1:
                    result.append(longLongToString(time->minute()));
                    break;
                case 2:
                    result.append(longLongToString(time->minute(), -1, 10, 2, QLocalePrivate::ZeroPadded));
                    break;
                }
                break;

            case 's':
                used = true;
                repeat = qMin(repeat, 2);
                switch (repeat) {
                case 1:
                    result.append(longLongToString(time->second()));
                    break;
                case 2:
                    result.append(longLongToString(time->second(), -1, 10, 2, QLocalePrivate::ZeroPadded));
                    break;
                }
                break;

            case 'a':
                used = true;
                if (i + 1 < format.length() && format.at(i + 1).unicode() == 'p') {
                    repeat = 2;
                } else {
                    repeat = 1;
                }
                result.append(am_pm == AM ? q->amText().toLower() : q->pmText().toLower());
                break;

            case 'A':
                used = true;
                if (i + 1 < format.length() && format.at(i + 1).unicode() == 'P') {
                    repeat = 2;
                } else {
                    repeat = 1;
                }
                result.append(am_pm == AM ? q->amText().toUpper() : q->pmText().toUpper());
                break;

            case 'z':
                used = true;
                if (repeat >= 3) {
                    repeat = 3;
                } else {
                    repeat = 1;
                }
                switch (repeat) {
                case 1:
                    result.append(longLongToString(time->msec()));
                    break;
                case 3:
                    result.append(longLongToString(time->msec(), -1, 10, 3, QLocalePrivate::ZeroPadded));
                    break;
                }
                break;

            case 't':
                used = true;
                repeat = 1;
                result.append(timeZone());
                break;
            default:
                break;
            }
        }
        if (!used) {
            result.append(QString(repeat, c));
        }
        i += repeat;
    }

    return result;
}

QString QLocalePrivate::doubleToString(double d,
                                       int precision,
                                       DoubleForm form,
                                       int width,
                                       unsigned flags) const
{
    return QLocalePrivate::doubleToString(zero(), plus(), minus(), exponential(),
                                          group(), decimal(),
                                          d, precision, form, width, flags);
}

QString QLocalePrivate::doubleToString(const QChar _zero, const QChar plus, const QChar minus,
                                       const QChar exponential, const QChar group, const QChar decimal,
                                       double d,
                                       int precision,
                                       DoubleForm form,
                                       int width,
                                       unsigned flags)
{
    if (precision == -1)
        precision = 6;
    if (width == -1)
        width = 0;

    bool negative = false;
    bool special_number = false; // nan, +/-inf
    QString num_str;

    // Detect special numbers (nan, +/-inf)
    if (qt_is_inf(d)) {
        num_str = QString::fromLatin1("inf");
        special_number = true;
        negative = d < 0;
    } else if (qt_is_nan(d)) {
        num_str = QString::fromLatin1("nan");
        special_number = true;
    }

    // Handle normal numbers
    if (!special_number) {
        int decpt, sign;
        QString digits;

#ifdef QT_QLOCALE_USES_FCVT
        // NOT thread safe!
        if (form == DFDecimal) {
            digits = QLatin1String(fcvt(d, precision, &decpt, &sign));
        } else {
            int pr = precision;
            if (form == DFExponent)
                ++pr;
            else if (form == DFSignificantDigits && pr == 0)
                pr = 1;
            digits = QLatin1String(ecvt(d, pr, &decpt, &sign));

            // Chop trailing zeros
            if (digits.length() > 0) {
                int last_nonzero_idx = digits.length() - 1;
                while (last_nonzero_idx > 0
                       && digits.unicode()[last_nonzero_idx] == QLatin1Char('0'))
                    --last_nonzero_idx;
                digits.truncate(last_nonzero_idx + 1);
            }

        }

#else
        int mode;
        if (form == DFDecimal)
            mode = 3;
        else
            mode = 2;

        /* This next bit is a bit quirky. In DFExponent form, the precision
           is the number of digits after decpt. So that would suggest using
           mode=3 for qdtoa. But qdtoa behaves strangely when mode=3 and
           precision=0. So we get around this by using mode=2 and reasoning
           that we want precision+1 significant digits, since the decimal
           point in this mode is always after the first digit. */
        int pr = precision;
        if (form == DFExponent)
            ++pr;

        char *rve = 0;
        char *buff = 0;
        QT_TRY {
            digits = QLatin1String(qdtoa(d, mode, pr, &decpt, &sign, &rve, &buff));
        } QT_CATCH(...) {
            if (buff != 0)
                free(buff);
            QT_RETHROW;
        }
        if (buff != 0)
            free(buff);
#endif // QT_QLOCALE_USES_FCVT

        if (_zero.unicode() != '0') {
            ushort z = _zero.unicode() - '0';
            for (int i = 0; i < digits.length(); ++i)
                reinterpret_cast<ushort *>(digits.data())[i] += z;
        }

        bool always_show_decpt = (flags & Alternate || flags & ForcePoint);
        switch (form) {
            case DFExponent: {
                num_str = exponentForm(_zero, decimal, exponential, group, plus, minus,
                                       digits, decpt, precision, PMDecimalDigits,
                                       always_show_decpt);
                break;
            }
            case DFDecimal: {
                num_str = decimalForm(_zero, decimal, group,
                                      digits, decpt, precision, PMDecimalDigits,
                                      always_show_decpt, flags & ThousandsGroup);
                break;
            }
            case DFSignificantDigits: {
                PrecisionMode mode = (flags & Alternate) ?
                            PMSignificantDigits : PMChopTrailingZeros;

                if (decpt != digits.length() && (decpt <= -4 || decpt > precision))
                    num_str = exponentForm(_zero, decimal, exponential, group, plus, minus,
                                           digits, decpt, precision, mode,
                                           always_show_decpt);
                else
                    num_str = decimalForm(_zero, decimal, group,
                                          digits, decpt, precision, mode,
                                          always_show_decpt, flags & ThousandsGroup);
                break;
            }
        }

        negative = sign != 0 && !isZero(d);
    }

    // pad with zeros. LeftAdjusted overrides this flag). Also, we don't
    // pad special numbers
    if (flags & QLocalePrivate::ZeroPadded
            && !(flags & QLocalePrivate::LeftAdjusted)
            && !special_number) {
        int num_pad_chars = width - num_str.length();
        // leave space for the sign
        if (negative
                || flags & QLocalePrivate::AlwaysShowSign
                || flags & QLocalePrivate::BlankBeforePositive)
            --num_pad_chars;

        for (int i = 0; i < num_pad_chars; ++i)
            num_str.prepend(_zero);
    }

    // add sign
    if (negative)
        num_str.prepend(minus);
    else if (flags & QLocalePrivate::AlwaysShowSign)
        num_str.prepend(plus);
    else if (flags & QLocalePrivate::BlankBeforePositive)
        num_str.prepend(QLatin1Char(' '));

    if (flags & QLocalePrivate::CapitalEorX)
        num_str = num_str.toUpper();

    return num_str;
}

QString QLocalePrivate::longLongToString(qlonglong l, int precision,
                                            int base, int width,
                                            unsigned flags) const
{
    return QLocalePrivate::longLongToString(zero(), group(), plus(), minus(),
                                            l, precision, base, width, flags);
}

QString QLocalePrivate::longLongToString(const QChar zero, const QChar group,
                                         const QChar plus, const QChar minus,
                                         qlonglong l, int precision,
                                         int base, int width,
                                         unsigned flags)
{
    bool precision_not_specified = false;
    if (precision == -1) {
        precision_not_specified = true;
        precision = 1;
    }

    bool negative = l < 0;
    if (base != 10) {
        // these are not supported by sprintf for octal and hex
        flags &= ~AlwaysShowSign;
        flags &= ~BlankBeforePositive;
        negative = false; // neither are negative numbers
    }

    QString num_str;
    if (base == 10)
        num_str = qlltoa(l, base, zero);
    else
        num_str = qulltoa(l, base, zero);

    uint cnt_thousand_sep = 0;
    if (flags & ThousandsGroup && base == 10) {
        for (int i = num_str.length() - 3; i > 0; i -= 3) {
            num_str.insert(i, group);
            ++cnt_thousand_sep;
        }
    }

    for (int i = num_str.length()/* - cnt_thousand_sep*/; i < precision; ++i)
        num_str.prepend(base == 10 ? zero : QChar::fromLatin1('0'));

    if ((flags & Alternate || flags & ShowBase)
            && base == 8
            && (num_str.isEmpty() || num_str[0].unicode() != QLatin1Char('0')))
        num_str.prepend(QLatin1Char('0'));

    // LeftAdjusted overrides this flag ZeroPadded. sprintf only padds
    // when precision is not specified in the format string
    bool zero_padded = flags & ZeroPadded
                        && !(flags & LeftAdjusted)
                        && precision_not_specified;

    if (zero_padded) {
        int num_pad_chars = width - num_str.length();

        // leave space for the sign
        if (negative
                || flags & AlwaysShowSign
                || flags & BlankBeforePositive)
            --num_pad_chars;

        // leave space for optional '0x' in hex form
        if (base == 16 && (flags & Alternate || flags & ShowBase))
            num_pad_chars -= 2;
        // leave space for optional '0b' in binary form
        else if (base == 2 && (flags & Alternate || flags & ShowBase))
            num_pad_chars -= 2;

        for (int i = 0; i < num_pad_chars; ++i)
            num_str.prepend(base == 10 ? zero : QChar::fromLatin1('0'));
    }

    if (flags & CapitalEorX)
        num_str = num_str.toUpper();

    if (base == 16 && (flags & Alternate || flags & ShowBase))
        num_str.prepend(QLatin1String(flags & UppercaseBase ? "0X" : "0x"));
    if (base == 2 && (flags & Alternate || flags & ShowBase))
        num_str.prepend(QLatin1String(flags & UppercaseBase ? "0B" : "0b"));

    // add sign
    if (negative)
        num_str.prepend(minus);
    else if (flags & AlwaysShowSign)
        num_str.prepend(plus);
    else if (flags & BlankBeforePositive)
        num_str.prepend(QLatin1Char(' '));

    return num_str;
}

QString QLocalePrivate::unsLongLongToString(qulonglong l, int precision,
                                            int base, int width,
                                            unsigned flags) const
{
    return QLocalePrivate::unsLongLongToString(zero(), group(), plus(),
                                               l, precision, base, width, flags);
}

QString QLocalePrivate::unsLongLongToString(const QChar zero, const QChar group,
                                            const QChar plus,
                                            qulonglong l, int precision,
                                            int base, int width,
                                            unsigned flags)
{
    bool precision_not_specified = false;
    if (precision == -1) {
        precision_not_specified = true;
        precision = 1;
    }

    QString num_str = qulltoa(l, base, zero);

    uint cnt_thousand_sep = 0;
    if (flags & ThousandsGroup && base == 10) {
        for (int i = num_str.length() - 3; i > 0; i -=3) {
            num_str.insert(i, group);
            ++cnt_thousand_sep;
        }
    }

    for (int i = num_str.length()/* - cnt_thousand_sep*/; i < precision; ++i)
        num_str.prepend(base == 10 ? zero : QChar::fromLatin1('0'));

    if ((flags & Alternate || flags & ShowBase)
            && base == 8
            && (num_str.isEmpty() || num_str[0].unicode() != QLatin1Char('0')))
        num_str.prepend(QLatin1Char('0'));

    // LeftAdjusted overrides this flag ZeroPadded. sprintf only padds
    // when precision is not specified in the format string
    bool zero_padded = flags & ZeroPadded
                        && !(flags & LeftAdjusted)
                        && precision_not_specified;

    if (zero_padded) {
        int num_pad_chars = width - num_str.length();

        // leave space for optional '0x' in hex form
        if (base == 16 && flags & Alternate)
            num_pad_chars -= 2;
        // leave space for optional '0b' in binary form
        else if (base == 2 && flags & Alternate)
            num_pad_chars -= 2;

        for (int i = 0; i < num_pad_chars; ++i)
            num_str.prepend(base == 10 ? zero : QChar::fromLatin1('0'));
    }

    if (flags & CapitalEorX)
        num_str = num_str.toUpper();

    if (base == 16 && (flags & Alternate || flags & ShowBase))
        num_str.prepend(QLatin1String(flags & UppercaseBase ? "0X" : "0x"));
    else if (base == 2 && (flags & Alternate || flags & ShowBase))
        num_str.prepend(QLatin1String(flags & UppercaseBase ? "0B" : "0b"));

    // add sign
    if (flags & AlwaysShowSign)
        num_str.prepend(plus);
    else if (flags & BlankBeforePositive)
        num_str.prepend(QLatin1Char(' '));

    return num_str;
}

/*
    Converts a number in locale to its representation in the C locale.
    Only has to guarantee that a string that is a correct representation of
    a number will be converted. If junk is passed in, junk will be passed
    out and the error will be detected during the actual conversion to a
    number. We can't detect junk here, since we don't even know the base
    of the number.
*/
bool QLocalePrivate::numberToCLocale(const QChar *str, int len,
                                            GroupSeparatorMode group_sep_mode,
                                            CharBuff *result) const
{
    const QChar *uc = str;
    int l = len;
    int idx = 0;

    // Skip whitespace
    while (idx < l && uc[idx].isSpace())
        ++idx;
    if (idx == l)
        return false;

    while (idx < l) {
        const QChar in = uc[idx];

        char out = digitToCLocale(in);
        if (out == 0) {
            if (in == list())
                out = ';';
            else if (in == percent())
                out = '%';
            // for handling base-x numbers
            else if (in.unicode() >= 'A' && in.unicode() <= 'Z')
                out = in.toLower().toLatin1();
            else if (in.unicode() >= 'a' && in.unicode() <= 'z')
                out = in.toLatin1();
            else
                break;
        }

        result->append(out);

        ++idx;
    }

    // Check trailing whitespace
    for (; idx < l; ++idx) {
        if (!uc[idx].isSpace())
            return false;
    }

    result->append('\0');

    // Check separators
    if (group_sep_mode == ParseGroupSeparators
            && !removeGroupSeparators(result))
        return false;


    return true;
}

bool QLocalePrivate::validateChars(const QString &str, NumberMode numMode, QByteArray *buff,
                                    int decDigits) const
{
    buff->clear();
    buff->reserve(str.length());

    const bool scientific = numMode == DoubleScientificMode;
    bool lastWasE = false;
    bool lastWasDigit = false;
    int eCnt = 0;
    int decPointCnt = 0;
    bool dec = false;
    int decDigitCnt = 0;

    for (int i = 0; i < str.length(); ++i) {
        char c = digitToCLocale(str.at(i));

        if (c >= '0' && c <= '9') {
            if (numMode != IntegerMode) {
                // If a double has too many digits after decpt, it shall be Invalid.
                if (dec && decDigits != -1 && decDigits < ++decDigitCnt)
                    return false;
            }
            lastWasDigit = true;
        } else {
            switch (c) {
                case '.':
                    if (numMode == IntegerMode) {
                        // If an integer has a decimal point, it shall be Invalid.
                        return false;
                    } else {
                        // If a double has more than one decimal point, it shall be Invalid.
                        if (++decPointCnt > 1)
                            return false;
#if 0
                        // If a double with no decimal digits has a decimal point, it shall be
                        // Invalid.
                        if (decDigits == 0)
                            return false;
#endif                  // On second thoughts, it shall be Valid.

                        dec = true;
                    }
                    break;

                case '+':
                case '-':
                    if (scientific) {
                        // If a scientific has a sign that's not at the beginning or after
                        // an 'e', it shall be Invalid.
                        if (i != 0 && !lastWasE)
                            return false;
                    } else {
                        // If a non-scientific has a sign that's not at the beginning,
                        // it shall be Invalid.
                        if (i != 0)
                            return false;
                    }
                    break;

                case ',':
                    //it can only be placed after a digit which is before the decimal point
                    if (!lastWasDigit || decPointCnt > 0)
                        return false;
                    break;

                case 'e':
                    if (scientific) {
                        // If a scientific has more than one 'e', it shall be Invalid.
                        if (++eCnt > 1)
                            return false;
                        dec = false;
                    } else {
                        // If a non-scientific has an 'e', it shall be Invalid.
                        return false;
                    }
                    break;

                default:
                    // If it's not a valid digit, it shall be Invalid.
                    return false;
            }
            lastWasDigit = false;
        }

        lastWasE = c == 'e';
        if (c != ',')
            buff->append(c);
    }

    return true;
}

double QLocalePrivate::stringToDouble(const QString &number, bool *ok,
                                        GroupSeparatorMode group_sep_mode) const
{
    CharBuff buff;
    // Do not use the ternary operator - triggers msvc2012 bug in optimized builds
    QString trimmedNumber;
    if (group().unicode() == 0xa0)
        trimmedNumber = number.trimmed();
    else
        trimmedNumber = number;
    if (!numberToCLocale(trimmedNumber.unicode(), trimmedNumber.size(),
                         group_sep_mode, &buff)) {
        if (ok != 0)
            *ok = false;
        return 0.0;
    }
    return bytearrayToDouble(buff.constData(), ok);
}

qlonglong QLocalePrivate::stringToLongLong(const QString &number, int base,
                                           bool *ok, GroupSeparatorMode group_sep_mode) const
{
    CharBuff buff;
    // Do not use the ternary operator - triggers msvc2012 bug in optimized builds
    QString trimmedNumber;
    if (group().unicode() == 0xa0)
        trimmedNumber = number.trimmed();
    else
        trimmedNumber = number;
    if (!numberToCLocale(trimmedNumber.unicode(), trimmedNumber.size(),
                         group_sep_mode, &buff)) {
        if (ok != 0)
            *ok = false;
        return 0;
    }

    return bytearrayToLongLong(buff.constData(), base, ok);
}

qulonglong QLocalePrivate::stringToUnsLongLong(const QString &number, int base,
                                               bool *ok, GroupSeparatorMode group_sep_mode) const
{
    CharBuff buff;
    // Do not use the ternary operator - triggers msvc2012 bug in optimized builds
    QString trimmedNumber;
    if (group().unicode() == 0xa0)
        trimmedNumber = number.trimmed();
    else
        trimmedNumber = number;
    if (!numberToCLocale(trimmedNumber.unicode(), trimmedNumber.size(),
                         group_sep_mode, &buff)) {
        if (ok != 0)
            *ok = false;
        return 0;
    }

    return bytearrayToUnsLongLong(buff.constData(), base, ok);
}

double QLocalePrivate::stringToDouble(const QStringRef &number, bool *ok,
                                        GroupSeparatorMode group_sep_mode) const
{
    CharBuff buff;
    QStringRef trimmedNumber;
    // Do not use the ternary operator - triggers msvc2012 bug in optimized builds
    if (group().unicode() == 0xa0)
        trimmedNumber = number.trimmed();
    else
        trimmedNumber = number;
    if (!numberToCLocale(trimmedNumber.unicode(), trimmedNumber.size(),
                         group_sep_mode, &buff)) {
        if (ok != 0)
            *ok = false;
        return 0.0;
    }
    return bytearrayToDouble(buff.constData(), ok);
}

qlonglong QLocalePrivate::stringToLongLong(const QStringRef &number, int base,
                                           bool *ok, GroupSeparatorMode group_sep_mode) const
{
    CharBuff buff;
    QStringRef trimmedNumber;
    // Do not use the ternary operator - triggers msvc2012 bug in optimized builds
    if (group().unicode() == 0xa0)
        trimmedNumber = number.trimmed();
    else
        trimmedNumber = number;
    if (!numberToCLocale(trimmedNumber.unicode(), trimmedNumber.size(),
                         group_sep_mode, &buff)) {
        if (ok != 0)
            *ok = false;
        return 0;
    }

    return bytearrayToLongLong(buff.constData(), base, ok);
}

qulonglong QLocalePrivate::stringToUnsLongLong(const QStringRef &number, int base,
                                               bool *ok, GroupSeparatorMode group_sep_mode) const
{
    CharBuff buff;
    QStringRef trimmedNumber;
    // Do not use the ternary operator - triggers msvc2012 bug in optimized builds
    if (group().unicode() == 0xa0)
        trimmedNumber = number.trimmed();
    else
        trimmedNumber = number;
    if (!numberToCLocale(trimmedNumber.unicode(), trimmedNumber.size(),
                         group_sep_mode, &buff)) {
        if (ok != 0)
            *ok = false;
        return 0;
    }

    return bytearrayToUnsLongLong(buff.constData(), base, ok);
}

double QLocalePrivate::bytearrayToDouble(const char *num, bool *ok, bool *overflow)
{
    if (ok != 0)
        *ok = true;
    if (overflow != 0)
        *overflow = false;

    if (*num == '\0') {
        if (ok != 0)
            *ok = false;
        return 0.0;
    }

    if (qstrcmp(num, "nan") == 0)
        return qt_snan();

    if (qstrcmp(num, "+inf") == 0 || qstrcmp(num, "inf") == 0)
        return qt_inf();

    if (qstrcmp(num, "-inf") == 0)
        return -qt_inf();

    bool _ok;
    const char *endptr;
    double d = qstrtod(num, &endptr, &_ok);

    if (!_ok) {
        // the only way strtod can fail with *endptr != '\0' on a non-empty
        // input string is overflow
        if (ok != 0)
            *ok = false;
        if (overflow != 0)
            *overflow = *endptr != '\0';
        return 0.0;
    }

    if (*endptr != '\0') {
        // we stopped at a non-digit character after converting some digits
        if (ok != 0)
            *ok = false;
        if (overflow != 0)
            *overflow = false;
        return 0.0;
    }

    if (ok != 0)
        *ok = true;
    if (overflow != 0)
        *overflow = false;
    return d;
}

qlonglong QLocalePrivate::bytearrayToLongLong(const char *num, int base, bool *ok, bool *overflow)
{
    bool _ok;
    const char *endptr;

    if (*num == '\0') {
        if (ok != 0)
            *ok = false;
        if (overflow != 0)
            *overflow = false;
        return 0;
    }

    qlonglong l = qstrtoll(num, &endptr, base, &_ok);

    if (!_ok) {
        if (ok != 0)
            *ok = false;
        if (overflow != 0) {
            // the only way qstrtoll can fail with *endptr != '\0' on a non-empty
            // input string is overflow
            *overflow = *endptr != '\0';
        }
        return 0;
    }

    if (*endptr != '\0') {
        // we stopped at a non-digit character after converting some digits
        if (ok != 0)
            *ok = false;
        if (overflow != 0)
            *overflow = false;
        return 0;
    }

    if (ok != 0)
        *ok = true;
    if (overflow != 0)
        *overflow = false;
    return l;
}

qulonglong QLocalePrivate::bytearrayToUnsLongLong(const char *num, int base, bool *ok)
{
    bool _ok;
    const char *endptr;
    qulonglong l = qstrtoull(num, &endptr, base, &_ok);

    if (!_ok || *endptr != '\0') {
        if (ok != 0)
            *ok = false;
        return 0;
    }

    if (ok != 0)
        *ok = true;
    return l;
}

QT_END_NAMESPACE
