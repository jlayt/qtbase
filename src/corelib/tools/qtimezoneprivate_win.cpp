/****************************************************************************
**
** Copyright (C) 2013 John Layt <jlayt@kde.org>
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

#include "qtimezone.h"
#include "qtimezoneprivate_p.h"

#include "qdatetime.h"

#include "qdebug.h"

QT_BEGIN_NAMESPACE

/*
    Private

    Windows system implementation
*/

#define MAX_KEY_LENGTH 255
#define FILETIME_UNIX_EPOCH Q_UINT64_C(116444736000000000)

// MSDN home page for Time support
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms724962%28v=vs.85%29.aspx

// For Windows XP and later refer to MSDN docs on TIME_ZONE_INFORMATION structure
// http://msdn.microsoft.com/en-gb/library/windows/desktop/ms725481%28v=vs.85%29.aspx

// Vista introduced support for historic data, see MSDN docs on DYNAMIC_TIME_ZONE_INFORMATION
// http://msdn.microsoft.com/en-gb/library/windows/desktop/ms724253%28v=vs.85%29.aspx

static const char tzRegPath[] = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones";
static const char currTzRegPath[] = "SYSTEM\\CurrentControlSet\\Control\\TimeZoneInformation";

// Copied from MSDN, see above for link
typedef struct _REG_TZI_FORMAT
{
    LONG Bias;
    LONG StandardBias;
    LONG DaylightBias;
    SYSTEMTIME StandardDate;
    SYSTEMTIME DaylightDate;
} REG_TZI_FORMAT;

static SYSTEMTIME msecsToSystemtime(qint64 forMSecsSinceEpoch)
{
    FILETIME utcFileTime;
    ULONGLONG nsecs = (forMSecsSinceEpoch * 10000 ) + FILETIME_UNIX_EPOCH;
    utcFileTime.dwLowDateTime  = (DWORD) (nsecs & 0xFFFFFFFF);
    utcFileTime.dwHighDateTime = (DWORD) (nsecs >> 32);
    SYSTEMTIME utcTime;
    FileTimeToSystemTime(&utcFileTime, &utcTime);
    return utcTime;
}

static qint64 systemtimeToMsecs(const SYSTEMTIME &systemtime)
{
    FILETIME utcFileTime;
    SystemTimeToFileTime(&systemtime, &utcFileTime);
    ULONGLONG utcNSecs = (((ULONGLONG) utcFileTime.dwHighDateTime) << 32) + utcFileTime.dwLowDateTime;
    return (utcNSecs - FILETIME_UNIX_EPOCH) / 10000;
}

static bool equalSystemtime(const SYSTEMTIME &t1, const SYSTEMTIME &t2)
{
    return (t1.wYear == t2.wYear
            && t1.wMonth == t2.wMonth
            && t1.wDay == t2.wDay
            && t1.wDayOfWeek == t2.wDayOfWeek
            && t1.wHour == t2.wHour
            && t1.wMinute == t2.wMinute
            && t1.wSecond == t2.wSecond
            && t1.wMilliseconds == t2.wMilliseconds);
}

static bool equalTzi(const TIME_ZONE_INFORMATION &tzi1, const TIME_ZONE_INFORMATION &tzi2)
{
    return(tzi1.Bias == tzi2.Bias
           && tzi1.StandardBias == tzi2.StandardBias
           && equalSystemtime(tzi1.StandardDate, tzi2.StandardDate)
           && wcscmp(tzi1.StandardName, tzi2.StandardName) == 0
           && tzi1.DaylightBias == tzi2.DaylightBias
           && equalSystemtime(tzi1.DaylightDate, tzi2.DaylightDate)
           && wcscmp(tzi1.DaylightName, tzi2.DaylightName) == 0);
}

static bool openRegistryKey(const QString &keyPath, HKEY *key)
{
    return (RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const wchar_t*)keyPath.utf16(), 0, KEY_READ, key)
            == ERROR_SUCCESS);
}

static QString readRegistryString(const HKEY &key, const QString &value)
{
    wchar_t buffer[MAX_PATH] = {0};
    DWORD size = sizeof(wchar_t) * MAX_PATH;
    RegQueryValueEx(key, (LPCWSTR)value.utf16(), NULL, NULL, (LPBYTE)buffer, &size);
    return QString::fromWCharArray(buffer);
}

static int readRegistryValue(const HKEY &key, const QString &value)
{
    DWORD buffer;
    DWORD size = sizeof(buffer);
    RegQueryValueEx(key, (LPCWSTR)value.utf16(), NULL, NULL, (LPBYTE)&buffer, &size);
    return buffer;
}

static QWinTimeZonePrivate::QWinTransitionRule readRegistryRule(const HKEY &key,
                                                                const QString &value, bool *ok)
{
    *ok = false;
    QWinTimeZonePrivate::QWinTransitionRule rule;
    REG_TZI_FORMAT tzi;
    DWORD tziSize = sizeof(tzi);
    if (RegQueryValueEx(key, (LPCWSTR)value.utf16(), NULL, NULL, (BYTE *)&tzi, &tziSize)
        == ERROR_SUCCESS) {
        rule.startYear = 0;
        rule.standardTimeBias = tzi.Bias + tzi.StandardBias;
        rule.daylightTimeBias = tzi.Bias + tzi.DaylightBias - rule.standardTimeBias;
        rule.standardTimeRule = tzi.StandardDate;
        rule.daylightTimeRule = tzi.DaylightDate;
        *ok = true;
    }
    return rule;
}

static TIME_ZONE_INFORMATION getRegistryTzi(const QByteArray &windowsId, bool *ok)
{
    *ok = false;
    TIME_ZONE_INFORMATION tzi;
    REG_TZI_FORMAT regTzi;
    DWORD regTziSize = sizeof(regTzi);
    HKEY key = NULL;
    const QString tziKeyPath = QString::fromUtf8(tzRegPath) + QLatin1Char('\\')
                               + QString::fromUtf8(windowsId);

    if (openRegistryKey(tziKeyPath, &key)) {

        DWORD size = sizeof(tzi.DaylightName);
        RegQueryValueEx(key, L"Dlt", NULL, NULL, (LPBYTE)tzi.DaylightName, &size);

        size = sizeof(tzi.StandardName);
        RegQueryValueEx(key, L"Std", NULL, NULL, (LPBYTE)tzi.StandardName, &size);

        if (RegQueryValueEx(key, L"TZI", NULL, NULL, (BYTE *) &regTzi, &regTziSize)
            == ERROR_SUCCESS) {
            tzi.Bias = regTzi.Bias;
            tzi.StandardBias = regTzi.StandardBias;
            tzi.DaylightBias = regTzi.DaylightBias;
            tzi.StandardDate = regTzi.StandardDate;
            tzi.DaylightDate = regTzi.DaylightDate;
            *ok = true;
        }

        RegCloseKey(key);
    }

    return tzi;
}

static QList<QByteArray> availableWindowsIds()
{
    // TODO Consider caching results in a global static, very unlikely to change.
    QList<QByteArray> list;
    HKEY key = NULL;
    if (openRegistryKey(QString::fromUtf8(tzRegPath), &key)) {
        DWORD idCount = 0;
        if (RegQueryInfoKey(key, 0, 0, 0, &idCount, 0, 0, 0, 0, 0, 0, 0) == ERROR_SUCCESS
            && idCount > 0) {
            for (DWORD i = 0; i < idCount; ++i) {
                DWORD maxLen = MAX_KEY_LENGTH;
                TCHAR buffer[MAX_KEY_LENGTH];
                if (RegEnumKeyEx(key, i, buffer, &maxLen, 0, 0, 0, 0) == ERROR_SUCCESS)
                    list.append(QString::fromWCharArray(buffer).toUtf8());
            }
        }
        RegCloseKey(key);
    }
    return list;
}

static QByteArray windowsSystemZoneId()
{
    // On Vista and later is held in the value TimeZoneKeyName in key currTzRegPath
    QString id;
    HKEY key = NULL;
    QString tziKeyPath = QString::fromUtf8(currTzRegPath);
    if (openRegistryKey(tziKeyPath, &key)) {
        id = readRegistryString(key, QStringLiteral("TimeZoneKeyName"));
        RegCloseKey(key);
        if (!id.isEmpty())
            return id.toUtf8();
    }

    // On XP we have to iterate over the zones until we find a match on
    // names/offsets with the current data
    TIME_ZONE_INFORMATION sysTzi;
    GetTimeZoneInformation(&sysTzi);
    bool ok = false;
    foreach (const QByteArray &winId, availableWindowsIds()) {
        if (equalTzi(getRegistryTzi(winId, &ok), sysTzi))
            return winId;
    }

    // If we can't determine the current ID use UTC
    return QByteArrayLiteral("UTC");
}

static SYSTEMTIME calculateTransitionLocalTime(const SYSTEMTIME &rule, int year)
{
    SYSTEMTIME time = rule;
    // If the year isn't set, then the rule date is relative
    if (time.wYear == 0) {
        if (time.wDayOfWeek == 0)
            time.wDayOfWeek = 7;
        QDate date(year, time.wMonth, 1);
        int startDow = date.dayOfWeek();
        if (startDow <= time.wDayOfWeek)
            date = date.addDays(time.wDayOfWeek - startDow - 7);
        else
            date = date.addDays(time.wDayOfWeek - startDow);
        date = date.addDays(time.wDay * 7);
        while (date.month() != time.wMonth)
            date = date.addDays(-7);
        time.wYear = year;
        time.wDay = date.day();
    }
    return time;
}

static void calculateTransitionsForYear(const QWinTimeZonePrivate::QWinTransitionRule &rule, int year,
                                       qint64 *stdMSecs, qint64 *dstMSecs)
{
    // TODO Consider caching the calculated values

    // Set up the required TZI
    TIME_ZONE_INFORMATION tzi;
    tzi.Bias = rule.standardTimeBias;
    tzi.StandardBias = 0;
    tzi.DaylightBias = rule.daylightTimeBias;
    tzi.StandardDate = rule.standardTimeRule;
    tzi.DaylightDate = rule.daylightTimeRule;

    // First calculate the local time transition for the year
    SYSTEMTIME standardTime = calculateTransitionLocalTime(tzi.StandardDate, year);
    SYSTEMTIME daylightTime = calculateTransitionLocalTime(tzi.DaylightDate, year);

    // TzSpecificLocalTimeToSystemTime assumes the transition has happened,
    // so need to adjust using the daylight offset
    const int adjust = rule.daylightTimeBias * -60 * 1000;

    // Next convert the transitions to UTC and then to MSecsSinceEpoch
    SYSTEMTIME utcTime;
    TzSpecificLocalTimeToSystemTime(&tzi, &standardTime, &utcTime);
    *stdMSecs = systemtimeToMsecs(utcTime) - adjust;

    TzSpecificLocalTimeToSystemTime(&tzi, &daylightTime, &utcTime);
    *dstMSecs = systemtimeToMsecs(utcTime) + adjust;
}

static QLocale::Country userCountry()
{
    const GEOID id = GetUserGeoID(GEOCLASS_NATION);
    wchar_t code[3];
    const int size = GetGeoInfo(id, GEO_ISO2, code, 3, 0);
    return (size == 3) ? QLocalePrivate::codeToCountry(QString::fromWCharArray(code))
                       : QLocale::AnyCountry;
}

// Create the system default time zone
QWinTimeZonePrivate::QWinTimeZonePrivate()
                   : QTimeZonePrivate()
{
    init(QByteArray());
}

// Create a named time zone
QWinTimeZonePrivate::QWinTimeZonePrivate(const QByteArray &olsenId)
                   : QTimeZonePrivate()
{
    init(olsenId);
}

QWinTimeZonePrivate::QWinTimeZonePrivate(const QWinTimeZonePrivate &other)
                   : QTimeZonePrivate(other), m_windowsId(other.m_windowsId),
                     m_displayName(other.m_displayName), m_standardName(other.m_standardName),
                     m_daylightName(other.m_daylightName), m_tranRules(other.m_tranRules)
{
}

QWinTimeZonePrivate::~QWinTimeZonePrivate()
{
}

QTimeZonePrivate *QWinTimeZonePrivate::clone()
{
    return new QWinTimeZonePrivate(*this);
}

void QWinTimeZonePrivate::init(const QByteArray &olsenId)
{
    if (olsenId.isEmpty()) {
        m_windowsId = windowsSystemZoneId();
        m_id = systemTimeZoneId();
    } else {
        m_windowsId = olsenIdToWindowsId(olsenId);
        m_id = olsenId;
    }

    if (!m_windowsId.isEmpty()) {
        // Open the base TZI for the time zone
        HKEY baseKey = NULL;
        const QString baseKeyPath = QString::fromUtf8(tzRegPath) + QLatin1Char('\\')
                                   + QString::fromUtf8(m_windowsId);
        if (openRegistryKey(baseKeyPath, &baseKey)) {
            //  Load the localized names
            m_displayName = readRegistryString(baseKey, QStringLiteral("Display"));
            m_standardName = readRegistryString(baseKey, QStringLiteral("Std"));
            m_daylightName = readRegistryString(baseKey, QStringLiteral("Dlt"));
            // On Vista and later the optional dynamic key holds historic data
            const QString dynamicKeyPath = baseKeyPath + QStringLiteral("\\Dynamic DST");
            HKEY dynamicKey = NULL;
            if (openRegistryKey(dynamicKeyPath, &dynamicKey)) {
                // Find out the start and end years stored, then iterate over them
                int startYear = readRegistryValue(dynamicKey, QStringLiteral("FirstEntry"));
                int endYear = readRegistryValue(dynamicKey, QStringLiteral("LastEntry"));
                for (int year = startYear; year <= endYear; ++year) {
                    bool ruleOk;
                    QWinTransitionRule rule = readRegistryRule(dynamicKey, QString::number(year), &ruleOk);
                    rule.startYear = year;
                    if (ruleOk)
                        m_tranRules.append(rule);
                }
                RegCloseKey(dynamicKey);
            } else {
                // No dynamic data so use the base data
                bool ruleOk;
                QWinTransitionRule rule = readRegistryRule(baseKey, QStringLiteral("TZI"), &ruleOk);
                rule.startYear = 1970;
                if (ruleOk)
                    m_tranRules.append(rule);
            }
            RegCloseKey(baseKey);
        }
    }

    // If there are no rules then we failed to find a windowsId or any tzi info
    if (m_tranRules.size() == 0) {
        m_id.clear();
        m_windowsId.clear();
        m_displayName.clear();
    }
}

QString QWinTimeZonePrivate::comment() const
{
    return m_displayName;
}

QString QWinTimeZonePrivate::displayName(QTimeZone::TimeType timeType,
                                         QTimeZone::NameType nameType,
                                         const QLocale &locale) const
{
    // TODO Registry holds MUI keys, should be able to look up translations?
    Q_UNUSED(locale);

    if (nameType == QTimeZone::OffsetName) {
        QWinTransitionRule rule = ruleForYear(QDate::currentDate().year());
        if (timeType == QTimeZone::DaylightTime)
            return isoOffsetFormat((rule.standardTimeBias + rule.daylightTimeBias) * -60);
        else
            return isoOffsetFormat((rule.standardTimeBias) * -60);
    }

    switch (timeType) {
    case  QTimeZone::DaylightTime :
        return m_daylightName;
    case  QTimeZone::GenericTime :
        return m_displayName;
    case  QTimeZone::StandardTime :
    default :
        return m_standardName;
    }
}

QString QWinTimeZonePrivate::abbreviation(qint64 atMSecsSinceEpoch) const
{
    return data(atMSecsSinceEpoch).abbreviation;
}

int QWinTimeZonePrivate::offsetFromUTC(qint64 atMSecsSinceEpoch) const
{
    return data(atMSecsSinceEpoch).offsetFromUTC;
}

int QWinTimeZonePrivate::standardTimeOffset(qint64 atMSecsSinceEpoch) const
{
    return data(atMSecsSinceEpoch).standardTimeOffset;
}

int QWinTimeZonePrivate::daylightTimeOffset(qint64 atMSecsSinceEpoch) const
{
    return data(atMSecsSinceEpoch).daylightTimeOffset;
}

bool QWinTimeZonePrivate::hasDaylightTime() const
{
    return hasTransitions();
}

bool QWinTimeZonePrivate::isDaylightTime(qint64 atMSecsSinceEpoch) const
{
    return (data(atMSecsSinceEpoch).daylightTimeOffset != 0);
}

QTimeZonePrivate::Data QWinTimeZonePrivate::data(qint64 forMSecsSinceEpoch) const
{
    // Convert MSecs to year to get transitions for, but around 31 Dec/1 Jan may not be right year
    // So get the year after we think we want transitions for, to be safe
    const SYSTEMTIME utcTime = msecsToSystemtime(forMSecsSinceEpoch);
    int year = utcTime.wYear;
    if ((utcTime.wMonth == 12 && utcTime.wDay == 31) || (utcTime.wMonth == 1 && utcTime.wDay == 1))
        ++year;

    qint64 first = maxMSecs();
    qint64 second = maxMSecs();
    qint64 next = maxMSecs();
    qint64 stdMSecs = maxMSecs();
    qint64 dstMSecs = maxMSecs();
    QWinTransitionRule rule;
    while (forMSecsSinceEpoch < first && year >= 1970) {
        // Convert the transition rules into msecs for the year we want to try
        rule = ruleForYear(year);
        calculateTransitionsForYear(rule, year, &stdMSecs, &dstMSecs);
        first = qMin(stdMSecs, dstMSecs);
        second = qMax(stdMSecs, dstMSecs);
        if (forMSecsSinceEpoch >= second)
            next = second;
        else if (forMSecsSinceEpoch >= first)
            next = first;
        // If didn't fall in this year, try the previous
        --year;
    }

    if (next == dstMSecs)
        return ruleToData(rule, forMSecsSinceEpoch, QTimeZone::DaylightTime);
    else if (next == stdMSecs)
        return ruleToData(rule, forMSecsSinceEpoch, QTimeZone::StandardTime);
    return invalidData();
}

bool QWinTimeZonePrivate::hasTransitions() const
{
    foreach (const QWinTransitionRule &rule, m_tranRules) {
        if (rule.standardTimeRule.wMonth > 0 && rule.daylightTimeRule.wMonth > 0)
            return true;
    }
    return false;
}

QTimeZonePrivate::Data QWinTimeZonePrivate::nextTransition(qint64 afterMSecsSinceEpoch) const
{
    // Convert MSecs to year to get transitions for, but around 31 Dec/1 Jan may not be right year
    // Get the year before we think we want transitions for, to be safe
    const SYSTEMTIME utcTime = msecsToSystemtime(afterMSecsSinceEpoch);
    int year = utcTime.wYear;
    if ((utcTime.wMonth == 12 && utcTime.wDay == 31) || (utcTime.wMonth == 1 && utcTime.wDay == 1))
        --year;

    qint64 first = minMSecs();
    qint64 second = minMSecs();
    qint64 next = minMSecs();
    qint64 stdMSecs = minMSecs();
    qint64 dstMSecs = minMSecs();
    QWinTransitionRule rule;
    while (afterMSecsSinceEpoch >= second && year <= 2050) {
        // Convert the transition rules into msecs for the year we want to try
        rule = ruleForYear(year);
        calculateTransitionsForYear(rule, year, &stdMSecs, &dstMSecs);
        // Find the first and second transition for the year
        first = qMin(stdMSecs, dstMSecs);
        second = qMax(stdMSecs, dstMSecs);
        if (afterMSecsSinceEpoch < first)
            next = first;
        else if (afterMSecsSinceEpoch < second)
            next = second;
        // If didn't fall in this year, try the next
        ++year;
    }

    if (next == dstMSecs)
        return ruleToData(rule, next, QTimeZone::DaylightTime);
    else if (next == stdMSecs)
        return ruleToData(rule, next, QTimeZone::StandardTime);
    return invalidData();
}

QTimeZonePrivate::Data QWinTimeZonePrivate::previousTransition(qint64 beforeMSecsSinceEpoch) const
{
    // Convert MSecs to year to get transitions for, but around 31 Dec/1 Jan may not be right year
    // So get the year after we think we want transitions for, to be safe
    const SYSTEMTIME utcTime = msecsToSystemtime(beforeMSecsSinceEpoch);
    int year = utcTime.wYear;
    if ((utcTime.wMonth == 12 && utcTime.wDay == 31) || (utcTime.wMonth == 1 && utcTime.wDay == 1))
        ++year;

    qint64 first = maxMSecs();
    qint64 second = maxMSecs();
    qint64 next = maxMSecs();
    qint64 stdMSecs = maxMSecs();
    qint64 dstMSecs = maxMSecs();
    QWinTransitionRule rule;
    while (beforeMSecsSinceEpoch <= first && year >= 1970) {
        // Convert the transition rules into msecs for the year we want to try
        rule = ruleForYear(year);
        calculateTransitionsForYear(rule, year, &stdMSecs, &dstMSecs);
        first = qMin(stdMSecs, dstMSecs);
        second = qMax(stdMSecs, dstMSecs);
        if (beforeMSecsSinceEpoch > second)
            next = second;
        else if (beforeMSecsSinceEpoch > first)
            next = first;
        // If didn't fall in this year, try the previous
        --year;
    }

    if (next == dstMSecs)
        return ruleToData(rule, next, QTimeZone::DaylightTime);
    else if (next == stdMSecs)
        return ruleToData(rule, next, QTimeZone::StandardTime);
    return invalidData();
}

QByteArray QWinTimeZonePrivate::systemTimeZoneId() const
{
    const QLocale::Country country = userCountry();
    const QByteArray windowsId = windowsSystemZoneId();
    QByteArray olsenId;
    // If we have a real country, then try get a specific match for that country
    if (country != QLocale::AnyCountry)
        olsenId = windowsIdToDefaultOlsenId(windowsId, country);
    // If we don't have a real country, or there wasn't a specific match, try the global default
    if (olsenId.isEmpty()) {
        olsenId = windowsIdToDefaultOlsenId(windowsId);
        // If no global default then probably an unknown Windows ID so return UTC
        if (olsenId.isEmpty())
            return QByteArrayLiteral("UTC");
    }
    return olsenId;
}

QSet<QByteArray> QWinTimeZonePrivate::availableTimeZoneIds() const
{
    QSet<QByteArray> set;
    foreach (const QByteArray &winId, availableWindowsIds()) {
        foreach (const QByteArray &olsenId, windowsIdToOlsenIds(winId))
            set << olsenId;
    }
    return set;
}

QWinTimeZonePrivate::QWinTransitionRule QWinTimeZonePrivate::ruleForYear(int year) const
{
    for (int i = m_tranRules.size() - 1; i >= 0; --i) {
        if (m_tranRules.at(i).startYear <= year)
            return m_tranRules.at(i);
    }
}

QTimeZonePrivate::Data QWinTimeZonePrivate::ruleToData(const QWinTransitionRule &rule,
                                                       qint64 atMSecsSinceEpoch,
                                                       QTimeZone::TimeType type) const
{
    QTimeZonePrivate::Data tran = QTimeZonePrivate::invalidData();
    tran.atMSecsSinceEpoch = atMSecsSinceEpoch;
    tran.standardTimeOffset = rule.standardTimeBias * -60;
    if (type == QTimeZone::DaylightTime) {
        tran.daylightTimeOffset = rule.daylightTimeBias * -60;
        tran.abbreviation = m_daylightName;
    } else {
        tran.daylightTimeOffset = 0;
        tran.abbreviation = m_standardName;
    }
    tran.offsetFromUTC = tran.standardTimeOffset + tran.daylightTimeOffset;
    return tran;
}

QT_END_NAMESPACE
