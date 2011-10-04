#!/usr/bin/env python
# coding=utf-8
#############################################################################
##
## Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
## All rights reserved.
## Contact: Nokia Corporation (qt-info@nokia.com)
##
## This file is part of the test suite of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:LGPL$
## GNU Lesser General Public License Usage
## This file may be used under the terms of the GNU Lesser General Public
## License version 2.1 as published by the Free Software Foundation and
## appearing in the file LICENSE.LGPL included in the packaging of this
## file. Please review the following information to ensure the GNU Lesser
## General Public License version 2.1 requirements will be met:
## http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## In addition, as a special exception, Nokia gives you certain additional
## rights. These rights are described in the Nokia Qt LGPL Exception
## version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
##
## GNU General Public License Usage
## Alternatively, this file may be used under the terms of the GNU General
## Public License version 3.0 as published by the Free Software Foundation
## and appearing in the file LICENSE.GPL included in the packaging of this
## file. Please review the following information to ensure the GNU General
## Public License version 3.0 requirements will be met:
## http://www.gnu.org/copyleft/gpl.html.
##
## Other Usage
## Alternatively, this file may be used in accordance with the terms and
## conditions contained in a signed written agreement between you and Nokia.
##
##
##
##
##
## $QT_END_LICENSE$
##
#############################################################################

import os
import sys
import enumdata
import xpathlite
from  xpathlite import DraftResolution
from dateconverter import convert_date
import re

findEntry = xpathlite.findEntry
findEntryInFile = xpathlite._findEntryInFile
findTagsInFile = xpathlite.findTagsInFile

def parse_number_format(patterns, data):
    # this is a very limited parsing of the number format for currency only.
    def skip_repeating_pattern(x):
        p = x.replace('0', '#').replace(',', '').replace('.', '')
        seen = False
        result = ''
        for c in p:
            if c == '#':
                if seen:
                    continue
                seen = True
            else:
                seen = False
            result = result + c
        return result
    patterns = patterns.split(';')
    result = []
    for pattern in patterns:
        pattern = skip_repeating_pattern(pattern)
        pattern = pattern.replace('#', "%1")
        # according to http://www.unicode.org/reports/tr35/#Number_Format_Patterns
        # there can be doubled or trippled currency sign, however none of the
        # locales use that.
        pattern = pattern.replace(u'\xa4', "%2")
        pattern = pattern.replace("''", "###").replace("'", '').replace("###", "'")
        pattern = pattern.replace('-', data['minus'])
        pattern = pattern.replace('+', data['plus'])
        result.append(pattern)
    return result

def parse_list_pattern_part_format(pattern):
    # this is a very limited parsing of the format for list pattern part only.
    result = ""
    result = pattern.replace("{0}", "%1")
    result = result.replace("{1}", "%2")
    result = result.replace("{2}", "%3")
    return result

def ordStr(c):
    if len(c) == 1:
        return str(ord(c))
    return "##########"

# the following functions are supposed to fix the problem with QLocale
# returning a character instead of strings for QLocale::exponential()
# and others. So we fallback to default values in these cases.
def fixOrdStrExp(c):
    if len(c) == 1:
        return str(ord(c))
    return str(ord('e'))
def fixOrdStrPercent(c):
    if len(c) == 1:
        return str(ord(c))
    return str(ord('%'))
def fixOrdStrList(c):
    if len(c) == 1:
        return str(ord(c))
    return str(ord(';'))

def generateLocaleInfo(path):
    (dir_name, file_name) = os.path.split(path)

    if not path.endswith(".xml"):
        return {}
    language_code = findEntryInFile(path, "identity/language", attribute="type")[0]
    if language_code == 'root':
        # just skip it
        return {}
    country_code = findEntryInFile(path, "identity/territory", attribute="type")[0]
    script_code = findEntryInFile(path, "identity/script", attribute="type")[0]
    variant_code = findEntryInFile(path, "identity/variant", attribute="type")[0]

    # we should handle fully qualified names with the territory
    if not country_code:
        return {}

    # we do not support variants
    # ### actually there is only one locale with variant: en_US_POSIX
    #     does anybody care about it at all?
    if variant_code:
        return {}

    language_id = enumdata.languageCodeToId(language_code)
    if language_id == -1:
        sys.stderr.write("unnknown language code \"" + language_code + "\"\n")
        return {}
    language = enumdata.language_list[language_id][0]

    script_id = enumdata.scriptCodeToId(script_code)
    if script_code == -1:
        sys.stderr.write("unnknown script code \"" + script_code + "\"\n")
        return {}
    script = "AnyScript"
    if script_id != -1:
        script = enumdata.script_list[script_id][0]

    country_id = enumdata.countryCodeToId(country_code)
    country = ""
    if country_id != -1:
        country = enumdata.country_list[country_id][0]
    if country == "":
        sys.stderr.write("unnknown country code \"" + country_code + "\"\n")
        return {}

    # So we say we accept only those values that have "contributed" or
    # "approved" resolution. see http://www.unicode.org/cldr/process.html
    # But we only respect the resolution for new datas for backward
    # compatibility.
    draft = DraftResolution.contributed

    result = {}
    result['language'] = language
    result['script'] = script
    result['country'] = country
    result['languageCode'] = language_code
    result['countryCode'] = country_code
    result['scriptCode'] = script_code
    result['variantCode'] = variant_code
    result['languageId'] = language_id
    result['scriptId'] = script_id
    result['countryId'] = country_id

    supplementalPath = dir_name + "/../supplemental/supplementalData.xml"
    currencies = findTagsInFile(supplementalPath, "currencyData/region[iso3166=%s]"%country_code);
    result['currencyIsoCode'] = ''
    result['currencyDigits'] = 2
    result['currencyRounding'] = 1
    if currencies:
        for e in currencies:
            if e[0] == 'currency':
                tender = True
                t = filter(lambda x: x[0] == 'tender', e[1])
                if t and t[0][1] == 'false':
                    tender = False;
                if tender and not filter(lambda x: x[0] == 'to', e[1]):
                    result['currencyIsoCode'] = filter(lambda x: x[0] == 'iso4217', e[1])[0][1]
                    break
        if result['currencyIsoCode']:
            t = findTagsInFile(supplementalPath, "currencyData/fractions/info[iso4217=%s]"%result['currencyIsoCode']);
            if t and t[0][0] == 'info':
                result['currencyDigits'] = int(filter(lambda x: x[0] == 'digits', t[0][1])[0][1])
                result['currencyRounding'] = int(filter(lambda x: x[0] == 'rounding', t[0][1])[0][1])
    numbering_system = None
    try:
        numbering_system = findEntry(path, "numbers/defaultNumberingSystem")
    except:
        pass

    def findEntryDef(path, xpath, value=''):
        try:
            return findEntry(path, xpath)
        except xpathlite.Error:
            return value

    def get_number_in_system(path, xpath, numbering_system):
        if numbering_system:
            try:
                return findEntry(path, xpath + "[numberSystem=" + numbering_system + "]")
            except xpathlite.Error:
                pass
        return findEntry(path, xpath)

    def getMonths(path, calendar, context, width):
        xpath = "dates/calendars/calendar[" + calendar + "]/months/monthContext[" + context + "]/monthWidth[" + width + "]/month"
        result = findEntry(path, xpath + "[1]")  + ";" \
               + findEntry(path, xpath + "[2]")  + ";" \
               + findEntry(path, xpath + "[3]")  + ";" \
               + findEntry(path, xpath + "[4]")  + ";" \
               + findEntry(path, xpath + "[5]")  + ";" \
               + findEntry(path, xpath + "[6]")  + ";" \
               + findEntry(path, xpath + "[7]")  + ";" \
               + findEntry(path, xpath + "[8]")  + ";" \
               + findEntry(path, xpath + "[9]")  + ";" \
               + findEntry(path, xpath + "[10]") + ";" \
               + findEntry(path, xpath + "[11]") + ";" \
               + findEntry(path, xpath + "[12]")
        try:
            month13 = findEntry(path, xpath + "[13]")
            result = result + ";" + month13
        except:
            pass
        return result

    def getDays(path, calendar, context, width):
        xpath = "dates/calendars/calendar[" + calendar + "]/days/dayContext[" + context + "]/dayWidth[" + width + "]/day"
        return findEntry(path, xpath + "[sun]") + ";" \
             + findEntry(path, xpath + "[mon]") + ";" \
             + findEntry(path, xpath + "[tue]") + ";" \
             + findEntry(path, xpath + "[wed]") + ";" \
             + findEntry(path, xpath + "[thu]") + ";" \
             + findEntry(path, xpath + "[fri]") + ";" \
             + findEntry(path, xpath + "[sat]")

    def getQuarters(path, calendar, context, width):
        xpath = "dates/calendars/calendar[" + calendar + "]/quarters/quarterContext[" + context + "]/quarterWidth[" + width + "]/quarter"
        return findEntry(path, xpath + "[1]") + ";" \
             + findEntry(path, xpath + "[2]") + ";" \
             + findEntry(path, xpath + "[3]") + ";" \
             + findEntry(path, xpath + "[4]")

    def getDayPeriod(path, period, calendar, context, width):
        return findEntry(path, "dates/calendars/calendar[" + calendar + "]/dayPeriods/dayPeriodContext[" + context + "]/dayPeriodWidth[" + width + "]/dayPeriod[" + period + "]", draft)

    def getDateFormat(path, calendar, length):
        return convert_date(findEntry(path, "dates/calendars/calendar[" + calendar + "]/dateFormats/dateFormatLength[" + length + "]/dateFormat/pattern"))

    def getTimeFormat(path, calendar, length):
        return convert_date(findEntry(path, "dates/calendars/calendar[" + calendar + "]/timeFormats/timeFormatLength[" + length + "]/timeFormat/pattern"))

    def getDateTimeFormat(path, calendar, length):
        try:
            return parse_list_pattern_part_format(convert_date(findEntry(path, "dates/calendars/calendar["
                                                               + calendar + "]/dateTimeFormats/dateTimeFormatLength["
                                                               + length + "]/dateTimeFormat/pattern")))
        except:
            return "%1 %2"

    result['decimal'] = get_number_in_system(path, "numbers/symbols/decimal", numbering_system)
    result['group'] = get_number_in_system(path, "numbers/symbols/group", numbering_system)
    result['list'] = get_number_in_system(path, "numbers/symbols/list", numbering_system)
    result['percent'] = get_number_in_system(path, "numbers/symbols/percentSign", numbering_system)
    result['zero'] = get_number_in_system(path, "numbers/symbols/nativeZeroDigit", numbering_system)
    result['minus'] = get_number_in_system(path, "numbers/symbols/minusSign", numbering_system)
    result['plus'] = get_number_in_system(path, "numbers/symbols/plusSign", numbering_system)
    result['exp'] = get_number_in_system(path, "numbers/symbols/exponential", numbering_system).lower()
    result['quotationStart'] = findEntry(path, "delimiters/quotationStart")
    result['quotationEnd'] = findEntry(path, "delimiters/quotationEnd")
    result['alternateQuotationStart'] = findEntry(path, "delimiters/alternateQuotationStart")
    result['alternateQuotationEnd'] = findEntry(path, "delimiters/alternateQuotationEnd")
    result['listPatternPartStart'] = parse_list_pattern_part_format(findEntry(path, "listPatterns/listPattern/listPatternPart[start]"))
    result['listPatternPartMiddle'] = parse_list_pattern_part_format(findEntry(path, "listPatterns/listPattern/listPatternPart[middle]"))
    result['listPatternPartEnd'] = parse_list_pattern_part_format(findEntry(path, "listPatterns/listPattern/listPatternPart[end]"))
    result['listPatternPartTwo'] = parse_list_pattern_part_format(findEntry(path, "listPatterns/listPattern/listPatternPart[2]"))

    cldrCalendars = set()
    for calendarId in enumdata.calendar_list:
        cldrCalendars.add(enumdata.calendar_list[calendarId][1])

    for calendar in cldrCalendars:
        result[calendar, 'calendarName'] = findEntryDef(path, "localeDisplayNames/types/type[type=%s]" % (calendar))
        if (result[calendar, 'calendarName'] == ''):
            result[calendar, 'calendarName'] = enumdata.calendar_list[enumdata.calendarCodeToId(calendar)][2]

        result[calendar, 'monthsLong']             = getMonths(path, calendar, "format",      "wide")
        result[calendar, 'monthsShort']            = getMonths(path, calendar, "format",      "abbreviated")
        result[calendar, 'monthsNarrow']           = getMonths(path, calendar, "format",      "narrow")
        result[calendar, 'monthsLongStandalone']   = getMonths(path, calendar, "stand-alone", "wide")
        result[calendar, 'monthsShortStandalone']  = getMonths(path, calendar, "stand-alone", "abbreviated")
        result[calendar, 'monthsNarrowStandalone'] = getMonths(path, calendar, "stand-alone", "narrow")

        result[calendar, 'daysLong']             = getDays(path, calendar, "format",      "wide")
        result[calendar, 'daysShort']            = getDays(path, calendar, "format",      "abbreviated")
        result[calendar, 'daysNarrow']           = getDays(path, calendar, "format",      "narrow")
        result[calendar, 'daysLongStandalone']   = getDays(path, calendar, "stand-alone", "wide")
        result[calendar, 'daysShortStandalone']  = getDays(path, calendar, "stand-alone", "abbreviated")
        result[calendar, 'daysNarrowStandalone'] = getDays(path, calendar, "stand-alone", "narrow")

        result[calendar, 'quartersLong']             = getQuarters(path, calendar, "format",      "wide")
        result[calendar, 'quartersShort']            = getQuarters(path, calendar, "format",      "abbreviated")
        result[calendar, 'quartersNarrow']           = getQuarters(path, calendar, "format",      "narrow")
        result[calendar, 'quartersLongStandalone']   = getQuarters(path, calendar, "stand-alone", "wide")
        result[calendar, 'quartersShortStandalone']  = getQuarters(path, calendar, "stand-alone", "abbreviated")
        result[calendar, 'quartersNarrowStandalone'] = getQuarters(path, calendar, "stand-alone", "narrow")

        result[calendar, 'amLong']             = getDayPeriod(path, "am", calendar, "format",      "wide")
        result[calendar, 'amShort']            = getDayPeriod(path, "am", calendar, "format",      "abbreviated")
        result[calendar, 'amNarrow']           = getDayPeriod(path, "am", calendar, "format",      "narrow")
        result[calendar, 'amLongStandalone']   = getDayPeriod(path, "am", calendar, "stand-alone", "wide")
        result[calendar, 'amShortStandalone']  = getDayPeriod(path, "am", calendar, "stand-alone", "abbreviated")
        result[calendar, 'amNarrowStandalone'] = getDayPeriod(path, "am", calendar, "stand-alone", "narrow")

        result[calendar, 'pmLong']             = getDayPeriod(path, "pm", calendar, "format",      "wide")
        result[calendar, 'pmShort']            = getDayPeriod(path, "pm", calendar, "format",      "abbreviated")
        result[calendar, 'pmNarrow']           = getDayPeriod(path, "pm", calendar, "format",      "narrow")
        result[calendar, 'pmLongStandalone']   = getDayPeriod(path, "pm", calendar, "stand-alone", "wide")
        result[calendar, 'pmShortStandalone']  = getDayPeriod(path, "pm", calendar, "stand-alone", "abbreviated")
        result[calendar, 'pmNarrowStandalone'] = getDayPeriod(path, "pm", calendar, "stand-alone", "narrow")

        result[calendar, 'dateFormatFull']     = getDateFormat(path, calendar, "full")
        result[calendar, 'dateFormatLong']     = getDateFormat(path, calendar, "long")
        result[calendar, 'dateFormatMedium']   = getDateFormat(path, calendar, "medium")
        result[calendar, 'dateFormatShort']    = getDateFormat(path, calendar, "short")

        result[calendar, 'timeFormatFull']     = getTimeFormat(path, calendar, "full")
        result[calendar, 'timeFormatLong']     = getTimeFormat(path, calendar, "long")
        result[calendar, 'timeFormatMedium']   = getTimeFormat(path, calendar, "medium")
        result[calendar, 'timeFormatShort']    = getTimeFormat(path, calendar, "short")

        result[calendar, 'dateTimeFormatFull']     = getDateTimeFormat(path, calendar, "full")
        result[calendar, 'dateTimeFormatLong']     = getDateTimeFormat(path, calendar, "long")
        result[calendar, 'dateTimeFormatMedium']   = getDateTimeFormat(path, calendar, "medium")
        result[calendar, 'dateTimeFormatShort']    = getDateTimeFormat(path, calendar, "short")

    endonym = None
    if country_code and script_code:
        endonym = findEntryDef(path, "localeDisplayNames/languages/language[type=%s_%s_%s]" % (language_code, script_code, country_code))
    if not endonym and script_code:
        endonym = findEntryDef(path, "localeDisplayNames/languages/language[type=%s_%s]" % (language_code, script_code))
    if not endonym and country_code:
        endonym = findEntryDef(path, "localeDisplayNames/languages/language[type=%s_%s]" % (language_code, country_code))
    if not endonym:
        endonym = findEntryDef(path, "localeDisplayNames/languages/language[type=%s]" % (language_code))
    result['languageEndonym'] = endonym
    result['countryEndonym'] = findEntryDef(path, "localeDisplayNames/territories/territory[type=%s]" % (country_code))
    result['scriptEndonym'] = findEntryDef(path, "localeDisplayNames/scripts/script[type=%s]" % (script_code))

    currency_format = get_number_in_system(path, "numbers/currencyFormats/currencyFormatLength/currencyFormat/pattern", numbering_system)
    currency_format = parse_number_format(currency_format, result)
    result['currencyFormat'] = currency_format[0]
    result['currencyNegativeFormat'] = ''
    if len(currency_format) > 1:
        result['currencyNegativeFormat'] = currency_format[1]

    result['currencySymbol'] = ''
    result['currencyDisplayName'] = ''
    if result['currencyIsoCode']:
        result['currencySymbol'] = findEntryDef(path, "numbers/currencies/currency[%s]/symbol" % result['currencyIsoCode'])
        display_name_path = "numbers/currencies/currency[%s]/displayName" % result['currencyIsoCode']
        result['currencyDisplayName'] \
            = findEntryDef(path, display_name_path) + ";" \
            + findEntryDef(path, display_name_path + "[count=zero]")  + ";" \
            + findEntryDef(path, display_name_path + "[count=one]")   + ";" \
            + findEntryDef(path, display_name_path + "[count=two]")   + ";" \
            + findEntryDef(path, display_name_path + "[count=few]")   + ";" \
            + findEntryDef(path, display_name_path + "[count=many]")  + ";" \
            + findEntryDef(path, display_name_path + "[count=other]") + ";"

    return result

def addEscapes(s):
    result = ''
    for c in s:
        n = ord(c)
        if n < 128:
            result += c
        else:
            result += "\\x"
            result += "%02x" % (n)
    return result

def unicodeStr(s):
    utf8 = s.encode('utf-8')
    return "<size>" + str(len(utf8)) + "</size><data>" + addEscapes(utf8) + "</data>"

def usage():
    print "Usage: cldr2qlocalexml.py <path-to-cldr-main>"
    sys.exit()

def integrateCalendarData(filePath):
    if not filePath.endswith(".xml"):
        return {}

    orderingByCountryCode = {}

    preferences = findTagsInFile(filePath, "calendarPreferenceData")
    for pref in preferences:
        ordering = pref[1][0][1].split(" ")
        territories = pref[1][1][1].split(" ")
        for countryCode in territories:
            orderingByCountryCode[countryCode] = ordering

    for (key,locale) in locale_database.iteritems():
        countryCode = locale['countryCode']
        if countryCode in orderingByCountryCode:
            locale_database[key]['calendarPreference'] = ";".join(orderingByCountryCode[countryCode])
        else:
            locale_database[key]['calendarPreference'] = ";".join(orderingByCountryCode["001"])


def integrateWeekData(filePath):
    if not filePath.endswith(".xml"):
        return {}
    monFirstDayIn = findEntryInFile(filePath, "weekData/firstDay[day=mon]", attribute="territories")[0].split(" ")
    tueFirstDayIn = findEntryInFile(filePath, "weekData/firstDay[day=tue]", attribute="territories")[0].split(" ")
    wedFirstDayIn = findEntryInFile(filePath, "weekData/firstDay[day=wed]", attribute="territories")[0].split(" ")
    thuFirstDayIn = findEntryInFile(filePath, "weekData/firstDay[day=thu]", attribute="territories")[0].split(" ")
    friFirstDayIn = findEntryInFile(filePath, "weekData/firstDay[day=fri]", attribute="territories")[0].split(" ")
    satFirstDayIn = findEntryInFile(filePath, "weekData/firstDay[day=sat]", attribute="territories")[0].split(" ")
    sunFirstDayIn = findEntryInFile(filePath, "weekData/firstDay[day=sun]", attribute="territories")[0].split(" ")

    monWeekendStart = findEntryInFile(filePath, "weekData/weekendStart[day=mon]", attribute="territories")[0].split(" ")
    tueWeekendStart = findEntryInFile(filePath, "weekData/weekendStart[day=tue]", attribute="territories")[0].split(" ")
    wedWeekendStart = findEntryInFile(filePath, "weekData/weekendStart[day=wed]", attribute="territories")[0].split(" ")
    thuWeekendStart = findEntryInFile(filePath, "weekData/weekendStart[day=thu]", attribute="territories")[0].split(" ")
    friWeekendStart = findEntryInFile(filePath, "weekData/weekendStart[day=fri]", attribute="territories")[0].split(" ")
    satWeekendStart = findEntryInFile(filePath, "weekData/weekendStart[day=sat]", attribute="territories")[0].split(" ")
    sunWeekendStart = findEntryInFile(filePath, "weekData/weekendStart[day=sun]", attribute="territories")[0].split(" ")

    monWeekendEnd = findEntryInFile(filePath, "weekData/weekendEnd[day=mon]", attribute="territories")[0].split(" ")
    tueWeekendEnd = findEntryInFile(filePath, "weekData/weekendEnd[day=tue]", attribute="territories")[0].split(" ")
    wedWeekendEnd = findEntryInFile(filePath, "weekData/weekendEnd[day=wed]", attribute="territories")[0].split(" ")
    thuWeekendEnd = findEntryInFile(filePath, "weekData/weekendEnd[day=thu]", attribute="territories")[0].split(" ")
    friWeekendEnd = findEntryInFile(filePath, "weekData/weekendEnd[day=fri]", attribute="territories")[0].split(" ")
    satWeekendEnd = findEntryInFile(filePath, "weekData/weekendEnd[day=sat]", attribute="territories")[0].split(" ")
    sunWeekendEnd = findEntryInFile(filePath, "weekData/weekendEnd[day=sun]", attribute="territories")[0].split(" ")

    firstDayByCountryCode = {}
    for countryCode in monFirstDayIn:
        firstDayByCountryCode[countryCode] = "mon"
    for countryCode in tueFirstDayIn:
        firstDayByCountryCode[countryCode] = "tue"
    for countryCode in wedFirstDayIn:
        firstDayByCountryCode[countryCode] = "wed"
    for countryCode in thuFirstDayIn:
        firstDayByCountryCode[countryCode] = "thu"
    for countryCode in friFirstDayIn:
        firstDayByCountryCode[countryCode] = "fri"
    for countryCode in satFirstDayIn:
        firstDayByCountryCode[countryCode] = "sat"
    for countryCode in sunFirstDayIn:
        firstDayByCountryCode[countryCode] = "sun"

    weekendStartByCountryCode = {}
    for countryCode in monWeekendStart:
        weekendStartByCountryCode[countryCode] = "mon"
    for countryCode in tueWeekendStart:
        weekendStartByCountryCode[countryCode] = "tue"
    for countryCode in wedWeekendStart:
        weekendStartByCountryCode[countryCode] = "wed"
    for countryCode in thuWeekendStart:
        weekendStartByCountryCode[countryCode] = "thu"
    for countryCode in friWeekendStart:
        weekendStartByCountryCode[countryCode] = "fri"
    for countryCode in satWeekendStart:
        weekendStartByCountryCode[countryCode] = "sat"
    for countryCode in sunWeekendStart:
        weekendStartByCountryCode[countryCode] = "sun"

    weekendEndByCountryCode = {}
    for countryCode in monWeekendEnd:
        weekendEndByCountryCode[countryCode] = "mon"
    for countryCode in tueWeekendEnd:
        weekendEndByCountryCode[countryCode] = "tue"
    for countryCode in wedWeekendEnd:
        weekendEndByCountryCode[countryCode] = "wed"
    for countryCode in thuWeekendEnd:
        weekendEndByCountryCode[countryCode] = "thu"
    for countryCode in friWeekendEnd:
        weekendEndByCountryCode[countryCode] = "fri"
    for countryCode in satWeekendEnd:
        weekendEndByCountryCode[countryCode] = "sat"
    for countryCode in sunWeekendEnd:
        weekendEndByCountryCode[countryCode] = "sun"

    for (key,locale) in locale_database.iteritems():
        countryCode = locale['countryCode']
        if countryCode in firstDayByCountryCode:
            locale_database[key]['firstDayOfWeek'] = firstDayByCountryCode[countryCode]
        else:
            locale_database[key]['firstDayOfWeek'] = firstDayByCountryCode["001"]

        if countryCode in weekendStartByCountryCode:
            locale_database[key]['weekendStart'] = weekendStartByCountryCode[countryCode]
        else:
            locale_database[key]['weekendStart'] = weekendStartByCountryCode["001"]

        if countryCode in weekendEndByCountryCode:
            locale_database[key]['weekendEnd'] = weekendEndByCountryCode[countryCode]
        else:
            locale_database[key]['weekendEnd'] = weekendEndByCountryCode["001"]

if len(sys.argv) != 2:
    usage()

cldr_dir = sys.argv[1]

if not os.path.isdir(cldr_dir):
    usage()

cldr_files = os.listdir(cldr_dir)

locale_database = {}
for file in cldr_files:
    l = generateLocaleInfo(cldr_dir + "/" + file)
    if not l:
        sys.stderr.write("skipping file \"" + file + "\"\n")
        continue
    else:
        sys.stderr.write(file + "\n")

    locale_database[(l['languageId'], l['scriptId'], l['countryId'], l['variantCode'])] = l

integrateCalendarData(cldr_dir+"/../supplemental/supplementalData.xml")
integrateWeekData(cldr_dir+"/../supplemental/supplementalData.xml")
locale_keys = locale_database.keys()
locale_keys.sort()

cldr_version = 'unknown'
ldml = open(cldr_dir+"/../dtd/ldml.dtd", "r")
for line in ldml:
    if 'version cldrVersion CDATA #FIXED' in line:
        cldr_version = line.split('"')[1]

print "<localeDatabase>"
print "    <version>" + cldr_version + "</version>"
print "    <languageList>"
for id in enumdata.language_list:
    l = enumdata.language_list[id]
    print "        <language>"
    print "            <name>" + l[0] + "</name>"
    print "            <id>" + str(id) + "</id>"
    print "            <code>" + l[1] + "</code>"
    print "        </language>"
print "    </languageList>"

print "    <scriptList>"
for id in enumdata.script_list:
    l = enumdata.script_list[id]
    print "        <script>"
    print "            <name>" + l[0] + "</name>"
    print "            <id>" + str(id) + "</id>"
    print "            <code>" + l[1] + "</code>"
    print "        </script>"
print "    </scriptList>"

print "    <countryList>"
for id in enumdata.country_list:
    l = enumdata.country_list[id]
    print "        <country>"
    print "            <name>" + l[0] + "</name>"
    print "            <id>" + str(id) + "</id>"
    print "            <code>" + l[1] + "</code>"
    print "        </country>"
print "    </countryList>"

print "    <calendarList>"
for id in enumdata.calendar_list:
    l = enumdata.calendar_list[id]
    print "        <calendar>"
    print "            <name>" + l[0] + "</name>"
    print "            <id>" + str(id) + "</id>"
    print "            <code>" + l[1] + "</code>"
    print "        </calendar>"
print "    </calendarList>"

print \
"    <defaultCountryList>\n\
        <defaultCountry>\n\
            <language>Afrikaans</language>\n\
            <country>SouthAfrica</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Afan</language>\n\
            <country>Ethiopia</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Afar</language>\n\
            <country>Djibouti</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Arabic</language>\n\
            <country>SaudiArabia</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Chinese</language>\n\
            <country>China</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Dutch</language>\n\
            <country>Netherlands</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>English</language>\n\
            <country>UnitedStates</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>French</language>\n\
            <country>France</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>German</language>\n\
            <country>Germany</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Greek</language>\n\
            <country>Greece</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Italian</language>\n\
            <country>Italy</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Malay</language>\n\
            <country>Malaysia</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Portuguese</language>\n\
            <country>Portugal</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Russian</language>\n\
            <country>RussianFederation</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Serbian</language>\n\
            <country>SerbiaAndMontenegro</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>SerboCroatian</language>\n\
            <country>SerbiaAndMontenegro</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Somali</language>\n\
            <country>Somalia</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Spanish</language>\n\
            <country>Spain</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Swahili</language>\n\
            <country>Kenya</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Swedish</language>\n\
            <country>Sweden</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Tigrinya</language>\n\
            <country>Eritrea</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Uzbek</language>\n\
            <country>Uzbekistan</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Persian</language>\n\
            <country>Iran</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Mongolian</language>\n\
            <country>Mongolia</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Nepali</language>\n\
            <country>Nepal</country>\n\
        </defaultCountry>\n\
    </defaultCountryList>"

print "    <localeList>"
print \
"        <locale>\n\
            <language>C</language>\n\
            <languageEndonym></languageEndonym>\n\
            <script>AnyScript</script>\n\
            <scriptEndonym></scriptEndonym>\n\
            <country>AnyCountry</country>\n\
            <countryEndonym></countryEndonym>\n\
            <decimal>46</decimal>\n\
            <group>44</group>\n\
            <list>59</list>\n\
            <percent>37</percent>\n\
            <zero>48</zero>\n\
            <minus>45</minus>\n\
            <plus>43</plus>\n\
            <exp>101</exp>\n\
            <quotationStart>\"</quotationStart>\n\
            <quotationEnd>\"</quotationEnd>\n\
            <alternateQuotationStart>\'</alternateQuotationStart>\n\
            <alternateQuotationEnd>\'</alternateQuotationEnd>\n\
            <listPatternPartStart>%1, %2</listPatternPartStart>\n\
            <listPatternPartMiddle>%1, %2</listPatternPartMiddle>\n\
            <listPatternPartEnd>%1, %2</listPatternPartEnd>\n\
            <listPatternPartTwo>%1, %2</listPatternPartTwo>\n\
            <currencyIsoCode></currencyIsoCode>\n\
            <currencySymbol></currencySymbol>\n\
            <currencyDisplayName>;;;;;;;</currencyDisplayName>\n\
            <currencyDigits>2</currencyDigits>\n\
            <currencyRounding>1</currencyRounding>\n\
            <currencyFormat>%1%2</currencyFormat>\n\
            <currencyNegativeFormat></currencyNegativeFormat>\n\
            <firstDayOfWeek>mon</firstDayOfWeek>\n\
            <weekendStart>sat</weekendStart>\n\
            <weekendEnd>sun</weekendEnd>\n\
            <calendarPreference>gregorian</calendarPreference>\n\
            <calendar type=\"gregorian\">\n\
                <calendarName>Gregorian Calendar</calendarName>\n\
                <monthsLong>January;February;March;April;May;June;July;August;September;October;November;December</monthsLong>\n\
                <monthsShort>Jan;Feb;Mar;Apr;May;Jun;Jul;Aug;Sep;Oct;Nov;Dec</monthsShort>\n\
                <monthsNarrow>J;F;M;A;M;J;J;A;S;O;N;D</monthsNarrow>\n\
                <monthsLongStandalone>January;February;March;April;May;June;July;August;September;October;November;December</monthsLongStandalone>\n\
                <monthsShortStandalone>Jan;Feb;Mar;Apr;May;Jun;Jul;Aug;Sep;Oct;Nov;Dec</monthsShortStandalone>\n\
                <monthsNarrowStandalone>J;F;M;A;M;J;J;A;S;O;N;D</monthsNarrowStandalone>\n\
                <daysLong>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLong>\n\
                <daysShort>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShort>\n\
                <daysNarrow>S;M;T;W;T;F;S</daysNarrow>\n\
                <daysLongStandalone>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLongStandalone>\n\
                <daysShortStandalone>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShortStandalone>\n\
                <daysNarrowStandalone>S;M;T;W;T;F;S</daysNarrowStandalone>\n\
                <quartersLong>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLong>\n\
                <quartersShort>Q1;Q2;Q3;Q4</quartersShort>\n\
                <quartersNarrow>1;2;3;4</quartersNarrow>\n\
                <quartersLongStandalone>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLongStandalone>\n\
                <quartersShortStandalone>Q1;Q2;Q3;Q4</quartersShortStandalone>\n\
                <quartersNarrowStandalone>1;2;3;4</quartersNarrowStandalone>\n\
                <amLong>AM</amLong>\n\
                <amShort>AM</amShort>\n\
                <amNarrow>AM</amNarrow>\n\
                <amLongStandalone>AM</amLongStandalone>\n\
                <amShortStandalone>AM</amShortStandalone>\n\
                <amNarrowStandalone>AM</amNarrowStandalone>\n\
                <pmLong>PM</pmLong>\n\
                <pmShort>PM</pmShort>\n\
                <pmNarrow>PM</pmNarrow>\n\
                <pmLongStandalone>PM</pmLongStandalone>\n\
                <pmShortStandalone>PM</pmShortStandalone>\n\
                <pmNarrowStandalone>PM</pmNarrowStandalone>\n\
                <dateFormatFull>dddd, d MMMM yyyy</dateFormatFull>\n\
                <dateFormatLong>d MMMM yyyy</dateFormatLong>\n\
                <dateFormatMedium>d MMM yyyy</dateFormatMedium>\n\
                <dateFormatShort>dd/MM/yyyy</dateFormatShort>\n\
                <timeFormatFull>HH:mm:ss t</timeFormatFull>\n\
                <timeFormatLong>HH:mm:ss t</timeFormatLong>\n\
                <timeFormatMedium>HH:mm:ss</timeFormatMedium>\n\
                <timeFormatShort>HH:mm</timeFormatShort>\n\
                <dateTimeFormatFull>%2 %1</dateTimeFormatFull>\n\
                <dateTimeFormatLong>%2 %1</dateTimeFormatLong>\n\
                <dateTimeFormatMedium>%2 %1</dateTimeFormatMedium>\n\
                <dateTimeFormatShort>%2 %1</dateTimeFormatShort>\n\
            </calendar>\n\
            <calendar type=\"buddhist\">\n\
                <calendarName>Buddhist Calendar</calendarName>\n\
                <monthsLong>January;February;March;April;May;June;July;August;September;October;November;December</monthsLong>\n\
                <monthsShort>Jan;Feb;Mar;Apr;May;Jun;Jul;Aug;Sep;Oct;Nov;Dec</monthsShort>\n\
                <monthsNarrow>J;F;M;A;M;J;J;A;S;O;N;D</monthsNarrow>\n\
                <monthsLongStandalone>January;February;March;April;May;June;July;August;September;October;November;December</monthsLongStandalone>\n\
                <monthsShortStandalone>Jan;Feb;Mar;Apr;May;Jun;Jul;Aug;Sep;Oct;Nov;Dec</monthsShortStandalone>\n\
                <monthsNarrowStandalone>J;F;M;A;M;J;J;A;S;O;N;D</monthsNarrowStandalone>\n\
                <daysLong>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLong>\n\
                <daysShort>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShort>\n\
                <daysNarrow>S;M;T;W;T;F;S</daysNarrow>\n\
                <daysLongStandalone>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLongStandalone>\n\
                <daysShortStandalone>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShortStandalone>\n\
                <daysNarrowStandalone>S;M;T;W;T;F;S</daysNarrowStandalone>\n\
                <quartersLong>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLong>\n\
                <quartersShort>Q1;Q2;Q3;Q4</quartersShort>\n\
                <quartersNarrow>1;2;3;4</quartersNarrow>\n\
                <quartersLongStandalone>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLongStandalone>\n\
                <quartersShortStandalone>Q1;Q2;Q3;Q4</quartersShortStandalone>\n\
                <quartersNarrowStandalone>1;2;3;4</quartersNarrowStandalone>\n\
                <amLong>AM</amLong>\n\
                <amShort>AM</amShort>\n\
                <amNarrow>AM</amNarrow>\n\
                <amLongStandalone>AM</amLongStandalone>\n\
                <amShortStandalone>AM</amShortStandalone>\n\
                <amNarrowStandalone>AM</amNarrowStandalone>\n\
                <pmLong>PM</pmLong>\n\
                <pmShort>PM</pmShort>\n\
                <pmNarrow>PM</pmNarrow>\n\
                <pmLongStandalone>PM</pmLongStandalone>\n\
                <pmShortStandalone>PM</pmShortStandalone>\n\
                <pmNarrowStandalone>PM</pmNarrowStandalone>\n\
                <dateFormatFull>dddd, MMMM d, yyyy</dateFormatFull>\n\
                <dateFormatLong>MMMM d, yyyy</dateFormatLong>\n\
                <dateFormatMedium>MMM d, yyyy</dateFormatMedium>\n\
                <dateFormatShort>M/d/yyyy</dateFormatShort>\n\
                <timeFormatFull>HH:mm:ss t</timeFormatFull>\n\
                <timeFormatLong>HH:mm:ss t</timeFormatLong>\n\
                <timeFormatMedium>HH:mm:ss</timeFormatMedium>\n\
                <timeFormatShort>HH:mm</timeFormatShort>\n\
                <dateTimeFormatFull>%2 %1</dateTimeFormatFull>\n\
                <dateTimeFormatLong>%2 %1</dateTimeFormatLong>\n\
                <dateTimeFormatMedium>%2 %1</dateTimeFormatMedium>\n\
                <dateTimeFormatShort>%2 %1</dateTimeFormatShort>\n\
            </calendar>\n\
            <calendar type=\"chinese\">\n\
                <calendarName>Chinese Calendar</calendarName>\n\
                <monthsLong>1;2;3;4;5;6;7;8;9;10;11;12</monthsLong>\n\
                <monthsShort>1;2;3;4;5;6;7;8;9;10;11;12</monthsShort>\n\
                <monthsNarrow>1;2;3;4;5;6;7;8;9;10;11;12</monthsNarrow>\n\
                <monthsLongStandalone>1;2;3;4;5;6;7;8;9;10;11;12</monthsLongStandalone>\n\
                <monthsShortStandalone>1;2;3;4;5;6;7;8;9;10;11;12</monthsShortStandalone>\n\
                <monthsNarrowStandalone>1;2;3;4;5;6;7;8;9;10;11;12</monthsNarrowStandalone>\n\
                <daysLong>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLong>\n\
                <daysShort>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShort>\n\
                <daysNarrow>S;M;T;W;T;F;S</daysNarrow>\n\
                <daysLongStandalone>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLongStandalone>\n\
                <daysShortStandalone>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShortStandalone>\n\
                <daysNarrowStandalone>S;M;T;W;T;F;S</daysNarrowStandalone>\n\
                <quartersLong>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLong>\n\
                <quartersShort>Q1;Q2;Q3;Q4</quartersShort>\n\
                <quartersNarrow>1;2;3;4</quartersNarrow>\n\
                <quartersLongStandalone>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLongStandalone>\n\
                <quartersShortStandalone>Q1;Q2;Q3;Q4</quartersShortStandalone>\n\
                <quartersNarrowStandalone>1;2;3;4</quartersNarrowStandalone>\n\
                <amLong>AM</amLong>\n\
                <amShort>AM</amShort>\n\
                <amNarrow>AM</amNarrow>\n\
                <amLongStandalone>AM</amLongStandalone>\n\
                <amShortStandalone>AM</amShortStandalone>\n\
                <amNarrowStandalone>AM</amNarrowStandalone>\n\
                <pmLong>PM</pmLong>\n\
                <pmShort>PM</pmShort>\n\
                <pmNarrow>PM</pmNarrow>\n\
                <pmLongStandalone>PM</pmLongStandalone>\n\
                <pmShortStandalone>PM</pmShortStandalone>\n\
                <pmNarrowStandalone>PM</pmNarrowStandalone>\n\
                <dateFormatFull>dddd yyyy'x'-M-d</dateFormatFull>\n\
                <dateFormatLong>yyyy'x'-M-d</dateFormatLong>\n\
                <dateFormatMedium>yyyy'x'-M-d</dateFormatMedium>\n\
                <dateFormatShort>yyyy'x'-M-d</dateFormatShort>\n\
                <timeFormatFull>HH:mm:ss t</timeFormatFull>\n\
                <timeFormatLong>HH:mm:ss t</timeFormatLong>\n\
                <timeFormatMedium>HH:mm:ss</timeFormatMedium>\n\
                <timeFormatShort>HH:mm</timeFormatShort>\n\
                <dateTimeFormatFull>%2 %1</dateTimeFormatFull>\n\
                <dateTimeFormatLong>%2 %1</dateTimeFormatLong>\n\
                <dateTimeFormatMedium>%2 %1</dateTimeFormatMedium>\n\
                <dateTimeFormatShort>%2 %1</dateTimeFormatShort>\n\
            </calendar>\n\
            <calendar type=\"coptic\">\n\
                <calendarName>Coptic Calendar</calendarName>\n\
                <monthsLong>Tout;Baba;Hator;Kiahk;Toba;Amshir;Baramhat;Baramouda;Bashans;Paona;Epep;Mesra;Nasie</monthsLong>\n\
                <monthsShort>Tout;Baba;Hator;Kiahk;Toba;Amshir;Baramhat;Baramouda;Bashans;Paona;Epep;Mesra;Nasie</monthsShort>\n\
                <monthsNarrow>1;2;3;4;5;6;7;8;9;10;11;12;13</monthsNarrow>\n\
                <monthsLongStandalone>Tout;Baba;Hator;Kiahk;Toba;Amshir;Baramhat;Baramouda;Bashans;Paona;Epep;Mesra;Nasie</monthsLongStandalone>\n\
                <monthsShortStandalone>Tout;Baba;Hator;Kiahk;Toba;Amshir;Baramhat;Baramouda;Bashans;Paona;Epep;Mesra;Nasie</monthsShortStandalone>\n\
                <monthsNarrowStandalone>1;2;3;4;5;6;7;8;9;10;11;12;13</monthsNarrowStandalone>\n\
                <daysLong>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLong>\n\
                <daysShort>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShort>\n\
                <daysNarrow>S;M;T;W;T;F;S</daysNarrow>\n\
                <daysLongStandalone>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLongStandalone>\n\
                <daysShortStandalone>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShortStandalone>\n\
                <daysNarrowStandalone>S;M;T;W;T;F;S</daysNarrowStandalone>\n\
                <quartersLong>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLong>\n\
                <quartersShort>Q1;Q2;Q3;Q4</quartersShort>\n\
                <quartersNarrow>1;2;3;4</quartersNarrow>\n\
                <quartersLongStandalone>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLongStandalone>\n\
                <quartersShortStandalone>Q1;Q2;Q3;Q4</quartersShortStandalone>\n\
                <quartersNarrowStandalone>1;2;3;4</quartersNarrowStandalone>\n\
                <amLong>AM</amLong>\n\
                <amShort>AM</amShort>\n\
                <amNarrow>AM</amNarrow>\n\
                <amLongStandalone>AM</amLongStandalone>\n\
                <amShortStandalone>AM</amShortStandalone>\n\
                <amNarrowStandalone>AM</amNarrowStandalone>\n\
                <pmLong>PM</pmLong>\n\
                <pmShort>PM</pmShort>\n\
                <pmNarrow>PM</pmNarrow>\n\
                <pmLongStandalone>PM</pmLongStandalone>\n\
                <pmShortStandalone>PM</pmShortStandalone>\n\
                <pmNarrowStandalone>PM</pmNarrowStandalone>\n\
                <dateFormatFull>dddd, d MMMM yyyy</dateFormatFull>\n\
                <dateFormatLong>d MMMM yyyy</dateFormatLong>\n\
                <dateFormatMedium>d MMM yyyy</dateFormatMedium>\n\
                <dateFormatShort>dd/MM/yyyy</dateFormatShort>\n\
                <timeFormatFull>HH:mm:ss t</timeFormatFull>\n\
                <timeFormatLong>HH:mm:ss t</timeFormatLong>\n\
                <timeFormatMedium>HH:mm:ss</timeFormatMedium>\n\
                <timeFormatShort>HH:mm</timeFormatShort>\n\
                <dateTimeFormatFull>%2 %1</dateTimeFormatFull>\n\
                <dateTimeFormatLong>%2 %1</dateTimeFormatLong>\n\
                <dateTimeFormatMedium>%2 %1</dateTimeFormatMedium>\n\
                <dateTimeFormatShort>%2 %1</dateTimeFormatShort>\n\
            </calendar>\n\
            <calendar type=\"ethiopic\">\n\
                <calendarName>Ethiopic Calendar</calendarName>\n\
                <monthsLong>Meskerem;Tekemt;Hedar;Tahsas;Ter;Yekatit;Megabit;Miazia;Genbot;Sene;Hamle;Nehasse;Pagumen</monthsLong>\n\
                <monthsShort>Meskerem;Tekemt;Hedar;Tahsas;Ter;Yekatit;Megabit;Miazia;Genbot;Sene;Hamle;Nehasse;Pagumen</monthsShort>\n\
                <monthsNarrow>1;2;3;4;5;6;7;8;9;10;11;12;13</monthsNarrow>\n\
                <monthsLongStandalone>Meskerem;Tekemt;Hedar;Tahsas;Ter;Yekatit;Megabit;Miazia;Genbot;Sene;Hamle;Nehasse;Pagumen</monthsLongStandalone>\n\
                <monthsShortStandalone>Meskerem;Tekemt;Hedar;Tahsas;Ter;Yekatit;Megabit;Miazia;Genbot;Sene;Hamle;Nehasse;Pagumen</monthsShortStandalone>\n\
                <monthsNarrowStandalone>1;2;3;4;5;6;7;8;9;10;11;12;13</monthsNarrowStandalone>\n\
                <daysLong>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLong>\n\
                <daysShort>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShort>\n\
                <daysNarrow>S;M;T;W;T;F;S</daysNarrow>\n\
                <daysLongStandalone>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLongStandalone>\n\
                <daysShortStandalone>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShortStandalone>\n\
                <daysNarrowStandalone>S;M;T;W;T;F;S</daysNarrowStandalone>\n\
                <quartersLong>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLong>\n\
                <quartersShort>Q1;Q2;Q3;Q4</quartersShort>\n\
                <quartersNarrow>1;2;3;4</quartersNarrow>\n\
                <quartersLongStandalone>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLongStandalone>\n\
                <quartersShortStandalone>Q1;Q2;Q3;Q4</quartersShortStandalone>\n\
                <quartersNarrowStandalone>1;2;3;4</quartersNarrowStandalone>\n\
                <amLong>AM</amLong>\n\
                <amShort>AM</amShort>\n\
                <amNarrow>AM</amNarrow>\n\
                <amLongStandalone>AM</amLongStandalone>\n\
                <amShortStandalone>AM</amShortStandalone>\n\
                <amNarrowStandalone>AM</amNarrowStandalone>\n\
                <pmLong>PM</pmLong>\n\
                <pmShort>PM</pmShort>\n\
                <pmNarrow>PM</pmNarrow>\n\
                <pmLongStandalone>PM</pmLongStandalone>\n\
                <pmShortStandalone>PM</pmShortStandalone>\n\
                <pmNarrowStandalone>PM</pmNarrowStandalone>\n\
                <dateFormatFull>dddd, d MMMM yyyy</dateFormatFull>\n\
                <dateFormatLong>d MMMM yyyy</dateFormatLong>\n\
                <dateFormatMedium>d MMM yyyy</dateFormatMedium>\n\
                <dateFormatShort>dd/MM/yyyy</dateFormatShort>\n\
                <timeFormatFull>HH:mm:ss t</timeFormatFull>\n\
                <timeFormatLong>HH:mm:ss t</timeFormatLong>\n\
                <timeFormatMedium>HH:mm:ss</timeFormatMedium>\n\
                <timeFormatShort>HH:mm</timeFormatShort>\n\
                <dateTimeFormatFull>%2 %1</dateTimeFormatFull>\n\
                <dateTimeFormatLong>%2 %1</dateTimeFormatLong>\n\
                <dateTimeFormatMedium>%2 %1</dateTimeFormatMedium>\n\
                <dateTimeFormatShort>%2 %1</dateTimeFormatShort>\n\
            </calendar>\n\
            <calendar type=\"hebrew\">\n\
                <calendarName>Hebrew Calendar</calendarName>\n\
                <monthsLong>Tishri;Heshvan;Kislev;Tevet;Shevat;Adar I;Adar;Nisan;Iyar;Sivan;Tamuz;Av;Elul</monthsLong>\n\
                <monthsShort>Tishri;Heshvan;Kislev;Tevet;Shevat;Adar I;Adar;Nisan;Iyar;Sivan;Tamuz;Av;Elul</monthsShort>\n\
                <monthsNarrow>Tishri;Heshvan;Kislev;Tevet;Shevat;Adar I;Adar;Nisan;Iyar;Sivan;Tamuz;Av;Elul</monthsNarrow>\n\
                <monthsLongStandalone>Tishri;Heshvan;Kislev;Tevet;Shevat;Adar I;Adar;Nisan;Iyar;Sivan;Tamuz;Av;Elul</monthsLongStandalone>\n\
                <monthsShortStandalone>Tishri;Heshvan;Kislev;Tevet;Shevat;Adar I;Adar;Nisan;Iyar;Sivan;Tamuz;Av;Elul</monthsShortStandalone>\n\
                <monthsNarrowStandalone>Tishri;Heshvan;Kislev;Tevet;Shevat;Adar I;Adar;Nisan;Iyar;Sivan;Tamuz;Av;Elul</monthsNarrowStandalone>\n\
                <daysLong>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLong>\n\
                <daysShort>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShort>\n\
                <daysNarrow>S;M;T;W;T;F;S</daysNarrow>\n\
                <daysLongStandalone>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLongStandalone>\n\
                <daysShortStandalone>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShortStandalone>\n\
                <daysNarrowStandalone>S;M;T;W;T;F;S</daysNarrowStandalone>\n\
                <quartersLong>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLong>\n\
                <quartersShort>Q1;Q2;Q3;Q4</quartersShort>\n\
                <quartersNarrow>1;2;3;4</quartersNarrow>\n\
                <quartersLongStandalone>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLongStandalone>\n\
                <quartersShortStandalone>Q1;Q2;Q3;Q4</quartersShortStandalone>\n\
                <quartersNarrowStandalone>1;2;3;4</quartersNarrowStandalone>\n\
                <amLong>AM</amLong>\n\
                <amShort>AM</amShort>\n\
                <amNarrow>AM</amNarrow>\n\
                <amLongStandalone>AM</amLongStandalone>\n\
                <amShortStandalone>AM</amShortStandalone>\n\
                <amNarrowStandalone>AM</amNarrowStandalone>\n\
                <pmLong>PM</pmLong>\n\
                <pmShort>PM</pmShort>\n\
                <pmNarrow>PM</pmNarrow>\n\
                <pmLongStandalone>PM</pmLongStandalone>\n\
                <pmShortStandalone>PM</pmShortStandalone>\n\
                <pmNarrowStandalone>PM</pmNarrowStandalone>\n\
                <dateFormatFull>dddd, d MMMM yyyy</dateFormatFull>\n\
                <dateFormatLong>d MMMM yyyy</dateFormatLong>\n\
                <dateFormatMedium>d MMM yyyy</dateFormatMedium>\n\
                <dateFormatShort>dd/MM/yyyy</dateFormatShort>\n\
                <timeFormatFull>HH:mm:ss t</timeFormatFull>\n\
                <timeFormatLong>HH:mm:ss t</timeFormatLong>\n\
                <timeFormatMedium>HH:mm:ss</timeFormatMedium>\n\
                <timeFormatShort>HH:mm</timeFormatShort>\n\
                <dateTimeFormatFull>%2 %1</dateTimeFormatFull>\n\
                <dateTimeFormatLong>%2 %1</dateTimeFormatLong>\n\
                <dateTimeFormatMedium>%2 %1</dateTimeFormatMedium>\n\
                <dateTimeFormatShort>%2 %1</dateTimeFormatShort>\n\
            </calendar>\n\
            <calendar type=\"indian\">\n\
                <calendarName>Indian National Calendar</calendarName>\n\
                <monthsLong>Chaitra;Vaisakha;Jyaistha;Asadha;Sravana;Bhadra;Asvina;Kartika;Agrahayana;Pausa;Magha;Phalguna</monthsLong>\n\
                <monthsShort>Chaitra;Vaisakha;Jyaistha;Asadha;Sravana;Bhadra;Asvina;Kartika;Agrahayana;Pausa;Magha;Phalguna</monthsShort>\n\
                <monthsNarrow>1;2;3;4;5;6;7;8;9;10;11;12</monthsNarrow>\n\
                <monthsLongStandalone>Chaitra;Vaisakha;Jyaistha;Asadha;Sravana;Bhadra;Asvina;Kartika;Agrahayana;Pausa;Magha;Phalguna</monthsLongStandalone>\n\
                <monthsShortStandalone>Chaitra;Vaisakha;Jyaistha;Asadha;Sravana;Bhadra;Asvina;Kartika;Agrahayana;Pausa;Magha;Phalguna</monthsShortStandalone>\n\
                <monthsNarrowStandalone>1;2;3;4;5;6;7;8;9;10;11;12</monthsNarrowStandalone>\n\
                <daysLong>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLong>\n\
                <daysShort>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShort>\n\
                <daysNarrow>S;M;T;W;T;F;S</daysNarrow>\n\
                <daysLongStandalone>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLongStandalone>\n\
                <daysShortStandalone>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShortStandalone>\n\
                <daysNarrowStandalone>S;M;T;W;T;F;S</daysNarrowStandalone>\n\
                <quartersLong>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLong>\n\
                <quartersShort>Q1;Q2;Q3;Q4</quartersShort>\n\
                <quartersNarrow>1;2;3;4</quartersNarrow>\n\
                <quartersLongStandalone>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLongStandalone>\n\
                <quartersShortStandalone>Q1;Q2;Q3;Q4</quartersShortStandalone>\n\
                <quartersNarrowStandalone>1;2;3;4</quartersNarrowStandalone>\n\
                <amLong>AM</amLong>\n\
                <amShort>AM</amShort>\n\
                <amNarrow>AM</amNarrow>\n\
                <amLongStandalone>AM</amLongStandalone>\n\
                <amShortStandalone>AM</amShortStandalone>\n\
                <amNarrowStandalone>AM</amNarrowStandalone>\n\
                <pmLong>PM</pmLong>\n\
                <pmShort>PM</pmShort>\n\
                <pmNarrow>PM</pmNarrow>\n\
                <pmLongStandalone>PM</pmLongStandalone>\n\
                <pmShortStandalone>PM</pmShortStandalone>\n\
                <pmNarrowStandalone>PM</pmNarrowStandalone>\n\
                <dateFormatFull>dddd, d MMMM yyyy</dateFormatFull>\n\
                <dateFormatLong>d MMMM yyyy</dateFormatLong>\n\
                <dateFormatMedium>d MMM yyyy</dateFormatMedium>\n\
                <dateFormatShort>dd/MM/yyyy</dateFormatShort>\n\
                <timeFormatFull>HH:mm:ss t</timeFormatFull>\n\
                <timeFormatLong>HH:mm:ss t</timeFormatLong>\n\
                <timeFormatMedium>HH:mm:ss</timeFormatMedium>\n\
                <timeFormatShort>HH:mm</timeFormatShort>\n\
                <dateTimeFormatFull>%2 %1</dateTimeFormatFull>\n\
                <dateTimeFormatLong>%2 %1</dateTimeFormatLong>\n\
                <dateTimeFormatMedium>%2 %1</dateTimeFormatMedium>\n\
                <dateTimeFormatShort>%2 %1</dateTimeFormatShort>\n\
            </calendar>\n\
            <calendar type=\"islamic\">\n\
                <calendarName>Islamic Calendar</calendarName>\n\
                <monthsLong>Muharram;Safar;Rabiʻ I;Rabiʻ II;Jumada I;Jumada II;Rajab;Shaʻban;Ramadan;Shawwal;Dhuʻl-Qiʻdah;Dhuʻl-Hijjah</monthsLong>\n\
                <monthsShort>Muh.;Saf.;Rab. I;Rab. II;Jum. I;Jum. II;Raj.;Sha.;Ram.;Shaw.;Dhuʻl-Q.;Dhuʻl-H.</monthsShort>\n\
                <monthsNarrow>1;2;3;4;5;6;7;8;9;10;11;12</monthsNarrow>\n\
                <monthsLongStandalone>Muharram;Safar;Rabiʻ I;Rabiʻ II;Jumada I;Jumada II;Rajab;Shaʻban;Ramadan;Shawwal;Dhuʻl-Qiʻdah;Dhuʻl-Hijjah</monthsLongStandalone>\n\
                <monthsShortStandalone>Muh.;Saf.;Rab. I;Rab. II;Jum. I;Jum. II;Raj.;Sha.;Ram.;Shaw.;Dhuʻl-Q.;Dhuʻl-H.</monthsShortStandalone>\n\
                <monthsNarrowStandalone>1;2;3;4;5;6;7;8;9;10;11;12</monthsNarrowStandalone>\n\
                <daysLong>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLong>\n\
                <daysShort>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShort>\n\
                <daysNarrow>S;M;T;W;T;F;S</daysNarrow>\n\
                <daysLongStandalone>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLongStandalone>\n\
                <daysShortStandalone>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShortStandalone>\n\
                <daysNarrowStandalone>S;M;T;W;T;F;S</daysNarrowStandalone>\n\
                <quartersLong>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLong>\n\
                <quartersShort>Q1;Q2;Q3;Q4</quartersShort>\n\
                <quartersNarrow>1;2;3;4</quartersNarrow>\n\
                <quartersLongStandalone>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLongStandalone>\n\
                <quartersShortStandalone>Q1;Q2;Q3;Q4</quartersShortStandalone>\n\
                <quartersNarrowStandalone>1;2;3;4</quartersNarrowStandalone>\n\
                <amLong>AM</amLong>\n\
                <amShort>AM</amShort>\n\
                <amNarrow>AM</amNarrow>\n\
                <amLongStandalone>AM</amLongStandalone>\n\
                <amShortStandalone>AM</amShortStandalone>\n\
                <amNarrowStandalone>AM</amNarrowStandalone>\n\
                <pmLong>PM</pmLong>\n\
                <pmShort>PM</pmShort>\n\
                <pmNarrow>PM</pmNarrow>\n\
                <pmLongStandalone>PM</pmLongStandalone>\n\
                <pmShortStandalone>PM</pmShortStandalone>\n\
                <pmNarrowStandalone>PM</pmNarrowStandalone>\n\
                <dateFormatFull>dddd, d MMMM yyyy</dateFormatFull>\n\
                <dateFormatLong>d MMMM yyyy</dateFormatLong>\n\
                <dateFormatMedium>d MMM yyyy</dateFormatMedium>\n\
                <dateFormatShort>dd/MM/yyyy</dateFormatShort>\n\
                <timeFormatFull>HH:mm:ss t</timeFormatFull>\n\
                <timeFormatLong>HH:mm:ss t</timeFormatLong>\n\
                <timeFormatMedium>HH:mm:ss</timeFormatMedium>\n\
                <timeFormatShort>HH:mm</timeFormatShort>\n\
                <dateTimeFormatFull>%2 %1</dateTimeFormatFull>\n\
                <dateTimeFormatLong>%2 %1</dateTimeFormatLong>\n\
                <dateTimeFormatMedium>%2 %1</dateTimeFormatMedium>\n\
                <dateTimeFormatShort>%2 %1</dateTimeFormatShort>\n\
            </calendar>\n\
            <calendar type=\"islamic-civil\">\n\
                <calendarName>Islamic-Civil Calendar</calendarName>\n\
                <monthsLong>Muharram;Safar;Rabiʻ I;Rabiʻ II;Jumada I;Jumada II;Rajab;Shaʻban;Ramadan;Shawwal;Dhuʻl-Qiʻdah;Dhuʻl-Hijjah</monthsLong>\n\
                <monthsShort>Muh.;Saf.;Rab. I;Rab. II;Jum. I;Jum. II;Raj.;Sha.;Ram.;Shaw.;Dhuʻl-Q.;Dhuʻl-H.</monthsShort>\n\
                <monthsNarrow>1;2;3;4;5;6;7;8;9;10;11;12</monthsNarrow>\n\
                <monthsLongStandalone>Muharram;Safar;Rabiʻ I;Rabiʻ II;Jumada I;Jumada II;Rajab;Shaʻban;Ramadan;Shawwal;Dhuʻl-Qiʻdah;Dhuʻl-Hijjah</monthsLongStandalone>\n\
                <monthsShortStandalone>Muh.;Saf.;Rab. I;Rab. II;Jum. I;Jum. II;Raj.;Sha.;Ram.;Shaw.;Dhuʻl-Q.;Dhuʻl-H.</monthsShortStandalone>\n\
                <monthsNarrowStandalone>1;2;3;4;5;6;7;8;9;10;11;12</monthsNarrowStandalone>\n\
                <daysLong>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLong>\n\
                <daysShort>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShort>\n\
                <daysNarrow>S;M;T;W;T;F;S</daysNarrow>\n\
                <daysLongStandalone>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLongStandalone>\n\
                <daysShortStandalone>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShortStandalone>\n\
                <daysNarrowStandalone>S;M;T;W;T;F;S</daysNarrowStandalone>\n\
                <quartersLong>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLong>\n\
                <quartersShort>Q1;Q2;Q3;Q4</quartersShort>\n\
                <quartersNarrow>1;2;3;4</quartersNarrow>\n\
                <quartersLongStandalone>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLongStandalone>\n\
                <quartersShortStandalone>Q1;Q2;Q3;Q4</quartersShortStandalone>\n\
                <quartersNarrowStandalone>1;2;3;4</quartersNarrowStandalone>\n\
                <amLong>AM</amLong>\n\
                <amShort>AM</amShort>\n\
                <amNarrow>AM</amNarrow>\n\
                <amLongStandalone>AM</amLongStandalone>\n\
                <amShortStandalone>AM</amShortStandalone>\n\
                <amNarrowStandalone>AM</amNarrowStandalone>\n\
                <pmLong>PM</pmLong>\n\
                <pmShort>PM</pmShort>\n\
                <pmNarrow>PM</pmNarrow>\n\
                <pmLongStandalone>PM</pmLongStandalone>\n\
                <pmShortStandalone>PM</pmShortStandalone>\n\
                <pmNarrowStandalone>PM</pmNarrowStandalone>\n\
                <dateFormatFull>dddd, d MMMM yyyy</dateFormatFull>\n\
                <dateFormatLong>d MMMM yyyy</dateFormatLong>\n\
                <dateFormatMedium>d MMM yyyy</dateFormatMedium>\n\
                <dateFormatShort>dd/MM/yyyy</dateFormatShort>\n\
                <timeFormatFull>HH:mm:ss t</timeFormatFull>\n\
                <timeFormatLong>HH:mm:ss t</timeFormatLong>\n\
                <timeFormatMedium>HH:mm:ss</timeFormatMedium>\n\
                <timeFormatShort>HH:mm</timeFormatShort>\n\
                <dateTimeFormatFull>%2 %1</dateTimeFormatFull>\n\
                <dateTimeFormatLong>%2 %1</dateTimeFormatLong>\n\
                <dateTimeFormatMedium>%2 %1</dateTimeFormatMedium>\n\
                <dateTimeFormatShort>%2 %1</dateTimeFormatShort>\n\
            </calendar>\n\
            <calendar type=\"japanese\">\n\
                <calendarName>Japanese Calendar</calendarName>\n\
                <monthsLong>January;February;March;April;May;June;July;August;September;October;November;December</monthsLong>\n\
                <monthsShort>Jan;Feb;Mar;Apr;May;Jun;Jul;Aug;Sep;Oct;Nov;Dec</monthsShort>\n\
                <monthsNarrow>J;F;M;A;M;J;J;A;S;O;N;D</monthsNarrow>\n\
                <monthsLongStandalone>January;February;March;April;May;June;July;August;September;October;November;December</monthsLongStandalone>\n\
                <monthsShortStandalone>Jan;Feb;Mar;Apr;May;Jun;Jul;Aug;Sep;Oct;Nov;Dec</monthsShortStandalone>\n\
                <monthsNarrowStandalone>J;F;M;A;M;J;J;A;S;O;N;D</monthsNarrowStandalone>\n\
                <daysLong>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLong>\n\
                <daysShort>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShort>\n\
                <daysNarrow>S;M;T;W;T;F;S</daysNarrow>\n\
                <daysLongStandalone>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLongStandalone>\n\
                <daysShortStandalone>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShortStandalone>\n\
                <daysNarrowStandalone>S;M;T;W;T;F;S</daysNarrowStandalone>\n\
                <quartersLong>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLong>\n\
                <quartersShort>Q1;Q2;Q3;Q4</quartersShort>\n\
                <quartersNarrow>1;2;3;4</quartersNarrow>\n\
                <quartersLongStandalone>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLongStandalone>\n\
                <quartersShortStandalone>Q1;Q2;Q3;Q4</quartersShortStandalone>\n\
                <quartersNarrowStandalone>1;2;3;4</quartersNarrowStandalone>\n\
                <amLong>AM</amLong>\n\
                <amShort>AM</amShort>\n\
                <amNarrow>AM</amNarrow>\n\
                <amLongStandalone>AM</amLongStandalone>\n\
                <amShortStandalone>AM</amShortStandalone>\n\
                <amNarrowStandalone>AM</amNarrowStandalone>\n\
                <pmLong>PM</pmLong>\n\
                <pmShort>PM</pmShort>\n\
                <pmNarrow>PM</pmNarrow>\n\
                <pmLongStandalone>PM</pmLongStandalone>\n\
                <pmShortStandalone>PM</pmShortStandalone>\n\
                <pmNarrowStandalone>PM</pmNarrowStandalone>\n\
                <dateFormatFull>dddd, yyyy MMMM dd</dateFormatFull>\n\
                <dateFormatLong>yyyy MMMM d</dateFormatLong>\n\
                <dateFormatMedium>yyyy MMM d</dateFormatMedium>\n\
                <dateFormatShort>yy-MM-dd</dateFormatShort>\n\
                <timeFormatFull>HH:mm:ss t</timeFormatFull>\n\
                <timeFormatLong>HH:mm:ss t</timeFormatLong>\n\
                <timeFormatMedium>HH:mm:ss</timeFormatMedium>\n\
                <timeFormatShort>HH:mm</timeFormatShort>\n\
                <dateTimeFormatFull>%2 %1</dateTimeFormatFull>\n\
                <dateTimeFormatLong>%2 %1</dateTimeFormatLong>\n\
                <dateTimeFormatMedium>%2 %1</dateTimeFormatMedium>\n\
                <dateTimeFormatShort>%2 %1</dateTimeFormatShort>\n\
            </calendar>\n\
            <calendar type=\"persian\">\n\
                <calendarName>Persian Calendar</calendarName>\n\
                <monthsLong>Farvardin;Ordibehesht;Khordad;Tir;Mordad;Shahrivar;Mehr;Aban;Azar;Dey;Bahman;Esfand</monthsLong>\n\
                <monthsShort>Farvardin;Ordibehesht;Khordad;Tir;Mordad;Shahrivar;Mehr;Aban;Azar;Dey;Bahman;Esfand</monthsShort>\n\
                <monthsNarrow>1;2;3;4;5;6;7;8;9;10;11;12</monthsNarrow>\n\
                <monthsLongStandalone>Farvardin;Ordibehesht;Khordad;Tir;Mordad;Shahrivar;Mehr;Aban;Azar;Dey;Bahman;Esfand</monthsLongStandalone>\n\
                <monthsShortStandalone>Farvardin;Ordibehesht;Khordad;Tir;Mordad;Shahrivar;Mehr;Aban;Azar;Dey;Bahman;Esfand</monthsShortStandalone>\n\
                <monthsNarrowStandalone>1;2;3;4;5;6;7;8;9;10;11;12</monthsNarrowStandalone>\n\
                <daysLong>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLong>\n\
                <daysShort>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShort>\n\
                <daysNarrow>S;M;T;W;T;F;S</daysNarrow>\n\
                <daysLongStandalone>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLongStandalone>\n\
                <daysShortStandalone>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShortStandalone>\n\
                <daysNarrowStandalone>S;M;T;W;T;F;S</daysNarrowStandalone>\n\
                <quartersLong>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLong>\n\
                <quartersShort>Q1;Q2;Q3;Q4</quartersShort>\n\
                <quartersNarrow>1;2;3;4</quartersNarrow>\n\
                <quartersLongStandalone>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLongStandalone>\n\
                <quartersShortStandalone>Q1;Q2;Q3;Q4</quartersShortStandalone>\n\
                <quartersNarrowStandalone>1;2;3;4</quartersNarrowStandalone>\n\
                <amLong>AM</amLong>\n\
                <amShort>AM</amShort>\n\
                <amNarrow>AM</amNarrow>\n\
                <amLongStandalone>AM</amLongStandalone>\n\
                <amShortStandalone>AM</amShortStandalone>\n\
                <amNarrowStandalone>AM</amNarrowStandalone>\n\
                <pmLong>PM</pmLong>\n\
                <pmShort>PM</pmShort>\n\
                <pmNarrow>PM</pmNarrow>\n\
                <pmLongStandalone>PM</pmLongStandalone>\n\
                <pmShortStandalone>PM</pmShortStandalone>\n\
                <pmNarrowStandalone>PM</pmNarrowStandalone>\n\
                <dateFormatFull>dddd, d MMMM yyyy</dateFormatFull>\n\
                <dateFormatLong>d MMMM yyyy</dateFormatLong>\n\
                <dateFormatMedium>d MMM yyyy</dateFormatMedium>\n\
                <dateFormatShort>dd/MM/yyyy</dateFormatShort>\n\
                <timeFormatFull>HH:mm:ss t</timeFormatFull>\n\
                <timeFormatLong>HH:mm:ss t</timeFormatLong>\n\
                <timeFormatMedium>HH:mm:ss</timeFormatMedium>\n\
                <timeFormatShort>HH:mm</timeFormatShort>\n\
                <dateTimeFormatFull>%2 %1</dateTimeFormatFull>\n\
                <dateTimeFormatLong>%2 %1</dateTimeFormatLong>\n\
                <dateTimeFormatMedium>%2 %1</dateTimeFormatMedium>\n\
                <dateTimeFormatShort>%2 %1</dateTimeFormatShort>\n\
            </calendar>\n\
            <calendar type=\"roc\">\n\
                <calendarName>Republic of China Calendar</calendarName>\n\
                <monthsLong>January;February;March;April;May;June;July;August;September;October;November;December</monthsLong>\n\
                <monthsShort>Jan;Feb;Mar;Apr;May;Jun;Jul;Aug;Sep;Oct;Nov;Dec</monthsShort>\n\
                <monthsNarrow>J;F;M;A;M;J;J;A;S;O;N;D</monthsNarrow>\n\
                <monthsLongStandalone>January;February;March;April;May;June;July;August;September;October;November;December</monthsLongStandalone>\n\
                <monthsShortStandalone>Jan;Feb;Mar;Apr;May;Jun;Jul;Aug;Sep;Oct;Nov;Dec</monthsShortStandalone>\n\
                <monthsNarrowStandalone>J;F;M;A;M;J;J;A;S;O;N;D</monthsNarrowStandalone>\n\
                <daysLong>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLong>\n\
                <daysShort>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShort>\n\
                <daysNarrow>S;M;T;W;T;F;S</daysNarrow>\n\
                <daysLongStandalone>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday</daysLongStandalone>\n\
                <daysShortStandalone>Sun;Mon;Tue;Wed;Thu;Fri;Sat</daysShortStandalone>\n\
                <daysNarrowStandalone>S;M;T;W;T;F;S</daysNarrowStandalone>\n\
                <quartersLong>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLong>\n\
                <quartersShort>Q1;Q2;Q3;Q4</quartersShort>\n\
                <quartersNarrow>1;2;3;4</quartersNarrow>\n\
                <quartersLongStandalone>1st quarter;2nd quarter;3rd quarter;4th quarter</quartersLongStandalone>\n\
                <quartersShortStandalone>Q1;Q2;Q3;Q4</quartersShortStandalone>\n\
                <quartersNarrowStandalone>1;2;3;4</quartersNarrowStandalone>\n\
                <amLong>AM</amLong>\n\
                <amShort>AM</amShort>\n\
                <amNarrow>AM</amNarrow>\n\
                <amLongStandalone>AM</amLongStandalone>\n\
                <amShortStandalone>AM</amShortStandalone>\n\
                <amNarrowStandalone>AM</amNarrowStandalone>\n\
                <pmLong>PM</pmLong>\n\
                <pmShort>PM</pmShort>\n\
                <pmNarrow>PM</pmNarrow>\n\
                <pmLongStandalone>PM</pmLongStandalone>\n\
                <pmShortStandalone>PM</pmShortStandalone>\n\
                <pmNarrowStandalone>PM</pmNarrowStandalone>\n\
                <dateFormatFull>dddd, MMMM d, yyyy</dateFormatFull>\n\
                <dateFormatLong>MMMM d, yyyy</dateFormatLong>\n\
                <dateFormatMedium>MMM d, yyyy</dateFormatMedium>\n\
                <dateFormatShort>yyyy/M/d</dateFormatShort>\n\
                <timeFormatFull>HH:mm:ss t</timeFormatFull>\n\
                <timeFormatLong>HH:mm:ss t</timeFormatLong>\n\
                <timeFormatMedium>HH:mm:ss</timeFormatMedium>\n\
                <timeFormatShort>HH:mm</timeFormatShort>\n\
                <dateTimeFormatFull></dateTimeFormatFull>\n\
                <dateTimeFormatLong></dateTimeFormatLong>\n\
                <dateTimeFormatMedium></dateTimeFormatMedium>\n\
                <dateTimeFormatShort></dateTimeFormatShort>\n\
            </calendar>\n\
        </locale>"

def printTagData(tag, data):
    print "            <" + tag + ">" + data + "</" + tag + ">"

def printTag(key, tag):
    print "            <" + tag + ">" + locale_database[key][tag] + "</" + tag + ">"

def printStrTag(key, tag):
    print "            <" + tag + ">" + str(locale_database[key][tag]) + "</" + tag + ">"

def printUnicodeTag(key, tag):
    print "            <" + tag + ">" + locale_database[key][tag].encode('utf-8') + "</" + tag + ">"

def printCalendarUnicodeTag(key, calendar, tag):
    print "                <" + tag + ">" + locale_database[key][calendar, tag].encode('utf-8') + "</" + tag + ">"

for key in locale_keys:
    l = locale_database[key]

    print "        <locale>"
    printTag(key, 'languageCode')
    printTag(key, 'language')
    printUnicodeTag(key, 'languageEndonym')
    printTag(key, 'scriptCode')
    printTag(key, 'script')
    printUnicodeTag(key, 'scriptEndonym')
    printTag(key, 'countryCode')
    printTag(key, 'country')
    printUnicodeTag(key, 'countryEndonym')
    printTagData('decimal', ordStr(l['decimal']))
    printTagData('group', ordStr(l['group']))
    printTagData('list', fixOrdStrList(l['list']))
    printTagData('percent', fixOrdStrPercent(l['percent']))
    printTagData('zero', ordStr(l['zero']))
    printTagData('minus', ordStr(l['minus']))
    printTagData('plus', ordStr(l['plus']))
    printTagData('exp', fixOrdStrExp(l['exp']))
    printUnicodeTag(key, 'quotationStart')
    printUnicodeTag(key, 'quotationEnd')
    printUnicodeTag(key, 'alternateQuotationStart')
    printUnicodeTag(key, 'alternateQuotationEnd')
    printUnicodeTag(key, 'listPatternPartStart')
    printUnicodeTag(key, 'listPatternPartMiddle')
    printUnicodeTag(key, 'listPatternPartEnd')
    printUnicodeTag(key, 'listPatternPartTwo')
    printUnicodeTag(key, 'currencyIsoCode')
    printUnicodeTag(key, 'currencySymbol')
    printUnicodeTag(key, 'currencyDisplayName')
    printStrTag(key, 'currencyDigits')
    printStrTag(key, 'currencyRounding')
    printUnicodeTag(key, 'currencyFormat')
    printUnicodeTag(key, 'currencyNegativeFormat')
    printUnicodeTag(key, 'firstDayOfWeek')
    printUnicodeTag(key, 'weekendStart')
    printUnicodeTag(key, 'weekendEnd')
    printUnicodeTag(key, 'calendarPreference')

    cldrCalendars = set()
    for calendarId in enumdata.calendar_list:
        cldrCalendars.add(enumdata.calendar_list[calendarId][1])

    for calendar in cldrCalendars:
        print "            <calendar type=\"" + calendar + "\">"
        printCalendarUnicodeTag(key, calendar, 'calendarName')
        printCalendarUnicodeTag(key, calendar, 'monthsLong')
        printCalendarUnicodeTag(key, calendar, 'monthsShort')
        printCalendarUnicodeTag(key, calendar, 'monthsNarrow')
        printCalendarUnicodeTag(key, calendar, 'monthsLongStandalone')
        printCalendarUnicodeTag(key, calendar, 'monthsShortStandalone')
        printCalendarUnicodeTag(key, calendar, 'monthsNarrowStandalone')
        printCalendarUnicodeTag(key, calendar, 'daysLong')
        printCalendarUnicodeTag(key, calendar, 'daysShort')
        printCalendarUnicodeTag(key, calendar, 'daysNarrow')
        printCalendarUnicodeTag(key, calendar, 'daysLongStandalone')
        printCalendarUnicodeTag(key, calendar, 'daysShortStandalone')
        printCalendarUnicodeTag(key, calendar, 'daysNarrowStandalone')
        printCalendarUnicodeTag(key, calendar, 'quartersLong')
        printCalendarUnicodeTag(key, calendar, 'quartersShort')
        printCalendarUnicodeTag(key, calendar, 'quartersNarrow')
        printCalendarUnicodeTag(key, calendar, 'quartersLongStandalone')
        printCalendarUnicodeTag(key, calendar, 'quartersShortStandalone')
        printCalendarUnicodeTag(key, calendar, 'quartersNarrowStandalone')
        printCalendarUnicodeTag(key, calendar, 'amLong')
        printCalendarUnicodeTag(key, calendar, 'amShort')
        printCalendarUnicodeTag(key, calendar, 'amNarrow')
        printCalendarUnicodeTag(key, calendar, 'amLongStandalone')
        printCalendarUnicodeTag(key, calendar, 'amShortStandalone')
        printCalendarUnicodeTag(key, calendar, 'amNarrowStandalone')
        printCalendarUnicodeTag(key, calendar, 'pmLong')
        printCalendarUnicodeTag(key, calendar, 'pmShort')
        printCalendarUnicodeTag(key, calendar, 'pmNarrow')
        printCalendarUnicodeTag(key, calendar, 'pmLongStandalone')
        printCalendarUnicodeTag(key, calendar, 'pmShortStandalone')
        printCalendarUnicodeTag(key, calendar, 'pmNarrowStandalone')
        printCalendarUnicodeTag(key, calendar, 'dateFormatFull')
        printCalendarUnicodeTag(key, calendar, 'dateFormatLong')
        printCalendarUnicodeTag(key, calendar, 'dateFormatMedium')
        printCalendarUnicodeTag(key, calendar, 'dateFormatShort')
        printCalendarUnicodeTag(key, calendar, 'timeFormatFull')
        printCalendarUnicodeTag(key, calendar, 'timeFormatLong')
        printCalendarUnicodeTag(key, calendar, 'timeFormatMedium')
        printCalendarUnicodeTag(key, calendar, 'timeFormatShort')
        printCalendarUnicodeTag(key, calendar, 'dateTimeFormatFull')
        printCalendarUnicodeTag(key, calendar, 'dateTimeFormatLong')
        printCalendarUnicodeTag(key, calendar, 'dateTimeFormatMedium')
        printCalendarUnicodeTag(key, calendar, 'dateTimeFormatShort')
        print "            </calendar>"

    print "        </locale>"
print "    </localeList>"
print "</localeDatabase>"
