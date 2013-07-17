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

QByteArray QLocalePrivate::bcp47Name(char separator) const
{
    if (m_data->m_language_id == QLocale::AnyLanguage)
        return QByteArray();
    if (m_data->m_language_id == QLocale::C)
        return QByteArrayLiteral("C");

    QLocaleId localeId = QLocaleId::fromIds(m_data->m_language_id, m_data->m_script_id, m_data->m_country_id);
    return localeId.withLikelySubtagsRemoved().name(separator);
}

const QLocaleData *QLocaleData::findLocaleData(QLocale::Language language, QLocale::Script script, QLocale::Country country)
{
    QLocaleId localeId = QLocaleId::fromIds(language, script, country);
    localeId = localeId.withLikelySubtagsAdded();

    uint idx = locale_index[localeId.language_id];

    const QLocaleData *data = locale_data + idx;

    if (idx == 0) // default language has no associated country
        return data;

    Q_ASSERT(m_data->m_language_id == localeId.language_id);

    if (localeId.script_id != QLocale::AnyScript && localeId.country_id != QLocale::AnyCountry) {
        // both script and country are explicitly specified
        do {
            if (m_data->m_script_id == localeId.script_id && m_data->m_country_id == localeId.country_id)
                return data;
            ++data;
        } while (m_data->m_language_id == localeId.language_id);

        // no match; try again with default script
        localeId.script_id = QLocale::AnyScript;
        data = locale_data + idx;
    }

    if (localeId.script_id == QLocale::AnyScript && localeId.country_id == QLocale::AnyCountry)
        return data;

    if (localeId.script_id == QLocale::AnyScript) {
        do {
            if (m_data->m_country_id == localeId.country_id)
                return data;
            ++data;
        } while (m_data->m_language_id == localeId.language_id);
    } else if (localeId.country_id == QLocale::AnyCountry) {
        do {
            if (m_data->m_script_id == localeId.script_id)
                return data;
            ++data;
        } while (m_data->m_language_id == localeId.language_id);
    }

    return locale_data + idx;
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

static const QLocaleData *findLocaleData(const QString &name)
{
    QLocale::Language lang;
    QLocale::Script script;
    QLocale::Country cntry;
    QLocalePrivate::getLangAndCountry(name, lang, script, cntry);

    return QLocaleData::findLocaleData(lang, script, cntry);
}

static const QLocaleData *default_data = 0;
static uint default_number_options = 0;

static const QLocaleData *const c_data = locale_data;
static QLocalePrivate c_private = { c_data, Q_BASIC_ATOMIC_INITIALIZER(1), 0 };

static const QLocaleData *defaultData()
{
    if (!default_data)
        default_data = systemData();
    return default_data;
}

const QLocaleData *QLocaleData::c()
{
    Q_ASSERT(locale_index[QLocale::C] == 0);
    return c_data;
}

static QString getLocaleListData(const ushort *data, int size, int index)
{
    static const ushort separator = ';';
    while (index && size > 0) {
        while (*data != separator)
            ++data, --size;
        --index;
        ++data;
        --size;
    }
    const ushort *end = data;
    while (size > 0 && *end != separator)
        ++end, --size;
    if (end-data == 0)
        return QString();
    return QString::fromRawData(reinterpret_cast<const QChar*>(data), end-data);
}

static inline QString getLocaleData(const ushort *data, int size)
{
    return size ? QString::fromRawData(reinterpret_cast<const QChar*>(data), size) : QString();
}


static const int locale_data_size = sizeof(locale_data)/sizeof(QLocaleData) - 1;

Q_GLOBAL_STATIC_WITH_ARGS(QSharedDataPointer<QLocalePrivate>, defaultLocalePrivate,
                          (QLocalePrivate::create(defaultData(), default_number_options)))

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
    if (m_data->m_language_id == QLocale::C && language != QLocale::C) {
        numberOptions = default_number_options;
        data = defaultData();
    }
    return QLocalePrivate::create(data, numberOptions);
}


QLocale::QLocale(const QString &name)
    : d(localePrivateByName(name))
{
}

QLocale::QLocale()
    : d(*defaultLocalePrivate)
{
}

QLocale::QLocale(Language language, Script script, Country country)
    : d(findLocalePrivate(language, script, country))
{
}

QLocale::QLocale(const QLocale &other)
{
    d = other.d;
}

QLocale::~QLocale()
{
}

QLocale &QLocale::operator=(const QLocale &other)
{
    d = other.d;
    return *this;
}

bool QLocale::operator==(const QLocale &other) const
{
    return m_data == other.m_data;
}

QString QLocale::quoteString(const QStringRef &str, QuotationStyle style) const
{
    if (style == QLocale::StandardQuotation)
        return QChar(m_data->m_quotation_start) % str % QChar(m_data->m_quotation_end);
    else
        return QChar(m_data->m_alternate_quotation_start) % str % QChar(m_data->m_alternate_quotation_end);
}

QString QLocale::createSeparatedList(const QStringList &list) const
{
    const int size = list.size();
    if (size == 1) {
        return list.at(0);
    } else if (size == 2) {
        QString format = getLocaleData(list_pattern_part_data + m_data->m_list_pattern_part_two_idx, m_data->m_list_pattern_part_two_size);
        return format.arg(list.at(0), list.at(1));
    } else if (size > 2) {
        QString formatStart = getLocaleData(list_pattern_part_data + m_data->m_list_pattern_part_start_idx, m_data->m_list_pattern_part_start_size);
        QString formatMid = getLocaleData(list_pattern_part_data + m_data->m_list_pattern_part_mid_idx, m_data->m_list_pattern_part_mid_size);
        QString formatEnd = getLocaleData(list_pattern_part_data + m_data->m_list_pattern_part_end_idx, m_data->m_list_pattern_part_end_size);
        QString result = formatStart.arg(list.at(0), list.at(1));
        for (int i = 2; i < size - 1; ++i)
            result = formatMid.arg(result, list.at(i));
        result = formatEnd.arg(result, list.at(size - 1));
        return result;
    }

    return QString();
}

QLocale::Language QLocale::language() const
{
    return Language(d->languageId());
}

QLocale::Script QLocale::script() const
{
    return Script(m_data->m_script_id);
}

QLocale::Country QLocale::country() const
{
    return Country(d->countryId());
}

QString QLocale::bcp47Name() const
{
    return QString::fromLatin1(d->bcp47Name());
}

QString QLocale::languageToString(Language language)
{
    if (uint(language) > uint(QLocale::LastLanguage))
        return QLatin1String("Unknown");
    return QLatin1String(language_name_list + language_name_index[language]);
}

QString QLocale::countryToString(Country country)
{
    if (uint(country) > uint(QLocale::LastCountry))
        return QLatin1String("Unknown");
    return QLatin1String(country_name_list + country_name_index[country]);
}

QString QLocale::scriptToString(QLocale::Script script)
{
    if (uint(script) > uint(QLocale::LastScript))
        return QLatin1String("Unknown");
    return QLatin1String(script_name_list + script_name_index[script]);
}

QStringList QLocale::uiLanguages() const
{
    QLocaleId id = QLocaleId::fromIds(m_data->m_language_id, m_data->m_script_id, m_data->m_country_id);
    const QLocaleId max = id.withLikelySubtagsAdded();
    const QLocaleId min = max.withLikelySubtagsRemoved();

    QStringList uiLanguages;
    uiLanguages.append(QString::fromLatin1(min.name()));
    if (id.script_id) {
        id.script_id = 0;
        if (id != min && id.withLikelySubtagsAdded() == max)
            uiLanguages.append(QString::fromLatin1(id.name()));
    }
    if (max != min && max != id)
        uiLanguages.append(QString::fromLatin1(max.name()));
    return uiLanguages;
}

QString QLocale::nativeLanguageName() const
{
    return getLocaleData(endonyms_data + m_data->m_language_endonym_idx, m_data->m_language_endonym_size);
}

QString QLocale::nativeCountryName() const
{
    return getLocaleData(endonyms_data + m_data->m_country_endonym_idx, m_data->m_country_endonym_size);
}

qint64 QLocale::toInt64(const QStringRef &s, QLocalePrivate::GroupSeparatorMode mode, bool *ok) const
{
    return QLocalePrivate::stringToLongLong(s, 10, ok, mode);
}

quint64 QLocale::toUint64(const QStringRef &s, QLocalePrivate::GroupSeparatorMode mode, bool *ok) const
{
    return QLocalePrivate::stringToUnsLongLong(s, 10, ok, mode);
}

double QLocale::toDouble(const QStringRef &s, QLocalePrivate::GroupSeparatorMode mode, bool *ok) const
{
    return QLocalePrivate::stringToDouble(s, ok, mode);
}

QString QLocale::toString(qint64 i, QLocalePrivate::Flags flags) const
{
    return QLocalePrivate::longLongToString(i, -1, 10, -1, flags);
}

QString QLocale::toString(quint64 i, QLocalePrivate::Flags flags) const
{
    return QLocalePrivate::unsLongLongToString(i, -1, 10, -1, flags);
}

QString QLocale::toString(const QDate &date, const QString &format) const
{
    return QLocalePrivate::dateTimeToString(format, &date, 0, this);
}

QString QLocale::toString(const QTime &time, const QString &format) const
{
    return QLocalePrivate::dateTimeToString(format, 0, &time, this);
}

QString QLocale::toString(const QDateTime &dateTime, const QString &format) const
{
    const QDate dt = dateTime.date();
    const QTime tm = dateTime.time();
    return QLocalePrivate::dateTimeToString(format, &dt, &tm, this);
}

QString QLocale::dateFormat(QLocale::FormatType format) const
{
    quint32 idx, size;
    switch (format) {
    case LongFormat:
        idx = m_data->m_long_date_format_idx;
        size = m_data->m_long_date_format_size;
        break;
    default:
        idx = m_data->m_short_date_format_idx;
        size = m_data->m_short_date_format_size;
        break;
    }
    return getLocaleData(date_format_data + idx, size);
}

QString QLocale::timeFormat(QLocale::FormatType format) const
{
    quint32 idx, size;
    switch (format) {
    case LongFormat:
        idx = m_data->m_long_time_format_idx;
        size = m_data->m_long_time_format_size;
        break;
    default:
        idx = m_data->m_short_time_format_idx;
        size = m_data->m_short_time_format_size;
        break;
    }
    return getLocaleData(time_format_data + idx, size);
}

QString QLocale::dateTimeFormat(QLocale::FormatType format) const
{
    return dateFormat(format) + QLatin1Char(' ') + timeFormat(format);
}

QTime QLocale::toTime(const QString &string, const QString &format) const
{
    QTime time;
#ifndef QT_BOOTSTRAPPED
    QDateTimeParser dt(QVariant::Time, QDateTimeParser::FromString);
    dt.defaultLocale = QLocale(*this);
    if (dt.parseFormat(format))
        dt.fromString(string, 0, &time);
#else
    Q_UNUSED(string);
    Q_UNUSED(format);
#endif
    return time;
}

#ifndef QT_NO_DATESTRING
QDate QLocale::toDate(const QString &string, const QString &format) const
{
    QDate date;
#ifndef QT_BOOTSTRAPPED
    QDateTimeParser dt(QVariant::Date, QDateTimeParser::FromString);
    dt.defaultLocale = *this;
    if (dt.parseFormat(format))
        dt.fromString(string, &date, 0);
#else
    Q_UNUSED(string);
    Q_UNUSED(format);
#endif
    return date;
}
#endif

#ifndef QT_NO_DATESTRING
QDateTime QLocale::toDateTime(const QString &string, const QString &format) const
{
#ifndef QT_BOOTSTRAPPED
    QTime time;
    QDate date;

    QDateTimeParser dt(QVariant::DateTime, QDateTimeParser::FromString);
    dt.defaultLocale = *this;
    if (dt.parseFormat(format) && dt.fromString(string, &date, &time))
        return QDateTime(date, time);
#else
    Q_UNUSED(string);
    Q_UNUSED(format);
#endif
    return QDateTime(QDate(), QTime(-1, -1, -1));
}
#endif

QChar QLocale::decimalPoint() const
{
    return QChar(m_data->m_decimal);
}

QChar QLocale::groupSeparator() const
{
    return QChar(m_data->m_group);
}

QChar QLocale::percent() const
{
    return QChar(m_data->m_percent);
}

QChar QLocale::zeroDigit() const
{
    return QChar(m_data->m_zero);
}

QChar QLocale::negativeSign() const
{
    return QChar(m_data->m_minus);
}

QChar QLocale::positiveSign() const
{
    return QChar(m_data->m_plus);
}

QChar QLocale::exponential() const
{
    return QChar(m_data->m_exponential);
}

QString QLocale::toString(double i, int prec, QLocalePrivate::DoubleForm form, QLocalePrivate::Flags) const
{
    return d->doubleToString(i, prec, form, -1, flags);
}

QList<QLocale> QLocale::matchingLocales(QLocale::Language language,
                                        QLocale::Script script,
                                        QLocale::Country country)
{
    QList<QLocale> result;
    if (language == QLocale::AnyLanguage && script == QLocale::AnyScript && country == QLocale::AnyCountry)
        result.reserve(locale_data_size);
    const QLocaleData *data = locale_data + locale_index[language];
    while ( (data != locale_data + locale_data_size)
            && (language == QLocale::AnyLanguage || m_data->m_language_id == uint(language))) {
        if ((script == QLocale::AnyScript || m_data->m_script_id == uint(script))
            && (country == QLocale::AnyCountry || m_data->m_country_id == uint(country))) {
            QLocale locale(*QLocalePrivate::create(data));
            result.append(locale);
        }
        ++data;
    }
    return result;
}

QList<QLocale::Country> QLocale::countriesForLanguage(Language language)
{
    unsigned language_id = language;
    const QLocaleData *data = locale_data + locale_index[language_id];
    while (m_data->m_language_id == language_id) {
        const QLocale::Country country = static_cast<Country>(m_data->m_country_id);
        if (!result.contains(country))
            result.append(country);
        ++data;
    }

    return result;
}

QString QLocale::monthName(int month, FormatType type) const
{
    quint32 idx, size;
    switch (type) {
    case QLocale::LongFormat:
        idx = m_data->m_long_month_names_idx;
        size = m_data->m_long_month_names_size;
        break;
    case QLocale::ShortFormat:
        idx = m_data->m_short_month_names_idx;
        size = m_data->m_short_month_names_size;
        break;
    case QLocale::NarrowFormat:
        idx = m_data->m_narrow_month_names_idx;
        size = m_data->m_narrow_month_names_size;
        break;
    default:
        return QString();
    }
    return getLocaleListData(months_data + idx, size, month - 1);
}

QString QLocale::standaloneMonthName(int month, FormatType type) const
{
    quint32 idx, size;
    switch (type) {
    case QLocale::LongFormat:
        idx = m_data->m_standalone_long_month_names_idx;
        size = m_data->m_standalone_long_month_names_size;
        break;
    case QLocale::ShortFormat:
        idx = m_data->m_standalone_short_month_names_idx;
        size = m_data->m_standalone_short_month_names_size;
        break;
    case QLocale::NarrowFormat:
        idx = m_data->m_standalone_narrow_month_names_idx;
        size = m_data->m_standalone_narrow_month_names_size;
        break;
    default:
        return QString();
    }
    QString name = getLocaleListData(months_data + idx, size, month - 1);
    if (name.isEmpty())
        return monthName(month, type);
    return name;
}

QString QLocale::dayName(int day, FormatType type) const
{
    if (day == 7)
        day = 0;

    quint32 idx, size;
    switch (type) {
    case QLocale::LongFormat:
        idx = m_data->m_long_day_names_idx;
        size = m_data->m_long_day_names_size;
        break;
    case QLocale::ShortFormat:
        idx = m_data->m_short_day_names_idx;
        size = m_data->m_short_day_names_size;
        break;
    case QLocale::NarrowFormat:
        idx = m_data->m_narrow_day_names_idx;
        size = m_data->m_narrow_day_names_size;
        break;
    default:
        return QString();
    }
    return getLocaleListData(days_data + idx, size, day);
}

QString QLocale::standaloneDayName(int day, FormatType type) const
{
    if (day == 7)
        day = 0;

    quint32 idx, size;
    switch (type) {
    case QLocale::LongFormat:
        idx = m_data->m_standalone_long_day_names_idx;
        size = m_data->m_standalone_long_day_names_size;
        break;
    case QLocale::ShortFormat:
        idx = m_data->m_standalone_short_day_names_idx;
        size = m_data->m_standalone_short_day_names_size;
        break;
    case QLocale::NarrowFormat:
        idx = m_data->m_standalone_narrow_day_names_idx;
        size = m_data->m_standalone_narrow_day_names_size;
        break;
    default:
        return QString();
    }
    QString name = getLocaleListData(days_data + idx, size, day);
    if (name.isEmpty())
        return dayName(day == 0 ? 7 : day, type);
    return name;
}

Qt::DayOfWeek QLocale::firstDayOfWeek() const
{
    return static_cast<Qt::DayOfWeek>(m_data->m_first_day_of_week);
}

QList<Qt::DayOfWeek> QLocale::weekdays() const
{
    QList<Qt::DayOfWeek> weekdays;
    quint16 weekendStart = m_data->m_weekend_start;
    quint16 weekendEnd = m_data->m_weekend_end;
    for (int day = Qt::Monday; day <= Qt::Sunday; day++) {
        if ((weekendEnd >= weekendStart && (day < weekendStart || day > weekendEnd)) ||
            (weekendEnd < weekendStart && (day > weekendEnd && day < weekendStart)))
                weekdays << static_cast<Qt::DayOfWeek>(day);
    }
    return weekdays;
}

QLocale::MeasurementSystem QLocalePrivate::measurementSystem() const
{
    for (int i = 0; i < ImperialMeasurementSystemsCount; ++i) {
        if (ImperialMeasurementSystems[i].languageId == m_data->m_language_id
            && ImperialMeasurementSystems[i].countryId == m_data->m_country_id) {
            return ImperialMeasurementSystems[i].system;
        }
    }
    return QLocale::MetricSystem;
}

QString QLocale::toUpper(const QString &str) const
{
    return str.toUpper();
}

QString QLocale::toLower(const QString &str) const
{
    return str.toLower();
}

QString QLocale::amText() const
{
    return getLocaleData(am_data + m_data->m_am_idx, m_data->m_am_size);
}

QString QLocale::pmText() const
{
    return getLocaleData(pm_data + m_data->m_pm_idx, m_data->m_pm_size);
}

QString QLocale::currencySymbol(QLocale::CurrencySymbolFormat format) const
{
    quint32 idx, size;
    switch (format) {
    case CurrencySymbol:
        idx = m_data->m_currency_symbol_idx;
        size = m_data->m_currency_symbol_size;
        return getLocaleData(currency_symbol_data + idx, size);
    case CurrencyDisplayName:
        idx = m_data->m_currency_display_name_idx;
        size = m_data->m_currency_display_name_size;
        return getLocaleListData(currency_display_name_data + idx, size, 0);
    case CurrencyIsoCode: {
        int len = 0;
        const QLocaleData *data = m_data;
        for (; len < 3; ++len)
            if (!m_data->m_currency_iso_code[len])
                break;
        return len ? QString::fromLatin1(m_data->m_currency_iso_code, len) : QString();
    }
    }
    return QString();
}

QString QLocale::toCurrencyString(qint64 value, const QString &symbol) const
{
    const QLocalePrivate *d = this->d;
    quint8 idx = m_data->m_currency_format_idx;
    quint8 size = m_data->m_currency_format_size;
    if (m_data->m_currency_negative_format_size && value < 0) {
        idx = m_data->m_currency_negative_format_idx;
        size = m_data->m_currency_negative_format_size;
        value = -value;
    }
    QString str = toString(value);
    QString sym = symbol.isNull() ? currencySymbol() : symbol;
    if (sym.isEmpty())
        sym = currencySymbol(QLocale::CurrencyIsoCode);
    QString format = getLocaleData(currency_format_data + idx, size);
    return format.arg(str, sym);
}

QString QLocale::toCurrencyString(quint64 value, const QString &symbol) const
{
    const QLocaleData *data = m_data;
    quint8 idx = m_data->m_currency_format_idx;
    quint8 size = m_data->m_currency_format_size;
    QString str = toString(value);
    QString sym = symbol.isNull() ? currencySymbol() : symbol;
    if (sym.isEmpty())
        sym = currencySymbol(QLocale::CurrencyIsoCode);
    QString format = getLocaleData(currency_format_data + idx, size);
    return format.arg(str, sym);
}

QString QLocale::toCurrencyString(double value, const QString &symbol) const
{
    const QLocaleData *data = m_data;
    quint8 idx = m_data->m_currency_format_idx;
    quint8 size = m_data->m_currency_format_size;
    if (m_data->m_currency_negative_format_size && value < 0) {
        idx = m_data->m_currency_negative_format_idx;
        size = m_data->m_currency_negative_format_size;
        value = -value;
    }
    QString str = toString(value, 'f', m_data->m_currency_digits);
    QString sym = symbol.isNull() ? currencySymbol() : symbol;
    if (sym.isEmpty())
        sym = currencySymbol(QLocale::CurrencyIsoCode);
    QString format = getLocaleData(currency_format_data + idx, size);
    return format.arg(str, sym);
}

QT_END_NAMESPACE
