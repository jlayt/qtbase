#!/usr/bin/env python
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
import tempfile
import datetime
import xml.dom.minidom
import enumdata

class Error:
    def __init__(self, msg):
        self.msg = msg
    def __str__(self):
        return self.msg

def check_static_char_array_length(name, array):
    # some compilers like VC6 doesn't allow static arrays more than 64K bytes size.
    size = reduce(lambda x, y: x+len(escapedString(y)), array, 0)
    if size > 65535:
        print "\n\n\n#error Array %s is too long! " % name
        sys.stderr.write("\n\n\nERROR: the content of the array '%s' is too long: %d > 65535 " % (name, size))
        sys.exit(1)

def wrap_list(lst):
    def split(lst, size):
        for i in range(len(lst)/size+1):
            yield lst[i*size:(i+1)*size]
    return ",\n".join(map(lambda x: ", ".join(x), split(lst, 20)))

def firstChildElt(parent, name):
    child = parent.firstChild
    while child:
        if child.nodeType == parent.ELEMENT_NODE \
            and (not name or child.nodeName == name):
            return child
        child = child.nextSibling
    return False

def nextSiblingElt(sibling, name):
    sib = sibling.nextSibling
    while sib:
        if sib.nodeType == sibling.ELEMENT_NODE \
            and (not name or sib.nodeName == name):
            return sib
        sib = sib.nextSibling
    return False

def eltText(elt):
    result = ""
    child = elt.firstChild
    while child:
        if child.nodeType == elt.TEXT_NODE:
            if result:
                result += " "
            result += child.nodeValue
        child = child.nextSibling
    return result

def loadLanguageMap(doc):
    result = {}

    language_list_elt = firstChildElt(doc.documentElement, "languageList")
    language_elt = firstChildElt(language_list_elt, "language")
    while language_elt:
        language_id = int(eltText(firstChildElt(language_elt, "id")))
        language_name = eltText(firstChildElt(language_elt, "name"))
        language_code = eltText(firstChildElt(language_elt, "code"))
        result[language_id] = (language_name, language_code)
        language_elt = nextSiblingElt(language_elt, "language")

    return result

def loadScriptMap(doc):
    result = {}

    script_list_elt = firstChildElt(doc.documentElement, "scriptList")
    script_elt = firstChildElt(script_list_elt, "script")
    while script_elt:
        script_id = int(eltText(firstChildElt(script_elt, "id")))
        script_name = eltText(firstChildElt(script_elt, "name"))
        script_code = eltText(firstChildElt(script_elt, "code"))
        result[script_id] = (script_name, script_code)
        script_elt = nextSiblingElt(script_elt, "script")

    return result

def loadCountryMap(doc):
    result = {}

    country_list_elt = firstChildElt(doc.documentElement, "countryList")
    country_elt = firstChildElt(country_list_elt, "country")
    while country_elt:
        country_id = int(eltText(firstChildElt(country_elt, "id")))
        country_name = eltText(firstChildElt(country_elt, "name"))
        country_code = eltText(firstChildElt(country_elt, "code"))
        result[country_id] = (country_name, country_code)
        country_elt = nextSiblingElt(country_elt, "country")

    return result

def loadDefaultMap(doc):
    result = {}

    list_elt = firstChildElt(doc.documentElement, "defaultCountryList")
    elt = firstChildElt(list_elt, "defaultCountry")
    while elt:
        country = eltText(firstChildElt(elt, "country"));
        language = eltText(firstChildElt(elt, "language"));
        result[language] = country;
        elt = nextSiblingElt(elt, "defaultCountry");
    return result

def fixedScriptName(name, dupes):
    name = name.replace(" ", "")
    if name[-6:] != "Script":
        name = name + "Script";
    if name in dupes:
        sys.stderr.write("\n\n\nERROR: The script name '%s' is messy" % name)
        sys.exit(1);
    return name

def fixedCountryName(name, dupes):
    if name in dupes:
        return name.replace(" ", "") + "Country"
    return name.replace(" ", "")

def fixedLanguageName(name, dupes):
    if name in dupes:
        return name.replace(" ", "") + "Language"
    return name.replace(" ", "")

def findDupes(country_map, language_map):
    country_set = set([ v[0] for a, v in country_map.iteritems() ])
    language_set = set([ v[0] for a, v in language_map.iteritems() ])
    return country_set & language_set

def languageNameToId(name, language_map):
    for key in language_map.keys():
        if language_map[key][0] == name:
            return key
    return -1

def scriptNameToId(name, script_map):
    for key in script_map.keys():
        if script_map[key][0] == name:
            return key
    return -1

def countryNameToId(name, country_map):
    for key in country_map.keys():
        if country_map[key][0] == name:
            return key
    return -1

def convertFormat(format):
    result = ""
    i = 0
    while i < len(format):
        if format[i] == "'":
            result += "'"
            i += 1
            while i < len(format) and format[i] != "'":
                result += format[i]
                i += 1
            if i < len(format):
                result += "'"
                i += 1
        else:
            s = format[i:]
            if s.startswith("EEEE"):
                result += "dddd"
                i += 4
            elif s.startswith("EEE"):
                result += "ddd"
                i += 3
            elif s.startswith("a"):
                result += "AP"
                i += 1
            elif s.startswith("z"):
                result += "t"
                i += 1
            elif s.startswith("v"):
                i += 1
            else:
                result += format[i]
                i += 1

    return result

def convertToQtDayOfWeek(firstDay):
    qtDayOfWeek = {"mon":1, "tue":2, "wed":3, "thu":4, "fri":5, "sat":6, "sun":7}
    return qtDayOfWeek[firstDay]

def assertSingleChar(string):
    assert len(string) == 1, "This string is not allowed to be longer than 1 character"
    return string

class Locale:
    def __init__(self, elt):
        self.language = eltText(firstChildElt(elt, "language"))
        self.languageEndonym = eltText(firstChildElt(elt, "languageEndonym"))
        self.script = eltText(firstChildElt(elt, "script"))
        self.country = eltText(firstChildElt(elt, "country"))
        print "locale = " + self.language + " / " + self.country + " / " + self.script
        self.countryEndonym = eltText(firstChildElt(elt, "countryEndonym"))
        self.decimal = int(eltText(firstChildElt(elt, "decimal")))
        self.group = int(eltText(firstChildElt(elt, "group")))
        self.listDelim = int(eltText(firstChildElt(elt, "list")))
        self.percent = int(eltText(firstChildElt(elt, "percent")))
        self.zero = int(eltText(firstChildElt(elt, "zero")))
        self.minus = int(eltText(firstChildElt(elt, "minus")))
        self.plus = int(eltText(firstChildElt(elt, "plus")))
        self.exp = int(eltText(firstChildElt(elt, "exp")))
        self.quotationStart = ord(assertSingleChar(eltText(firstChildElt(elt, "quotationStart"))))
        self.quotationEnd = ord(assertSingleChar(eltText(firstChildElt(elt, "quotationEnd"))))
        self.alternateQuotationStart = ord(assertSingleChar(eltText(firstChildElt(elt, "alternateQuotationStart"))))
        self.alternateQuotationEnd = ord(assertSingleChar(eltText(firstChildElt(elt, "alternateQuotationEnd"))))
        self.listPatternPartStart = eltText(firstChildElt(elt, "listPatternPartStart"))
        self.listPatternPartMiddle = eltText(firstChildElt(elt, "listPatternPartMiddle"))
        self.listPatternPartEnd = eltText(firstChildElt(elt, "listPatternPartEnd"))
        self.listPatternPartTwo = eltText(firstChildElt(elt, "listPatternPartTwo"))
        self.currencyIsoCode = eltText(firstChildElt(elt, "currencyIsoCode"))
        self.currencySymbol = eltText(firstChildElt(elt, "currencySymbol"))
        self.currencyDisplayName = eltText(firstChildElt(elt, "currencyDisplayName"))
        self.currencyDigits = int(eltText(firstChildElt(elt, "currencyDigits")))
        self.currencyRounding = int(eltText(firstChildElt(elt, "currencyRounding")))
        self.currencyFormat = eltText(firstChildElt(elt, "currencyFormat"))
        self.currencyNegativeFormat = eltText(firstChildElt(elt, "currencyNegativeFormat"))
        self.firstDayOfWeek = convertToQtDayOfWeek(eltText(firstChildElt(elt, "firstDayOfWeek")))
        self.weekendStart = convertToQtDayOfWeek(eltText(firstChildElt(elt, "weekendStart")))
        self.weekendEnd = convertToQtDayOfWeek(eltText(firstChildElt(elt, "weekendEnd")))
        self.calendarPreference = eltText(firstChildElt(elt, "calendarPreference"))
        self.calendarPreferenceList = self.calendarPreference.split(";")
        self.calendarPreferenceIdList = []
        for calendar in self.calendarPreferenceList :
            self.calendarPreferenceIdList.append(enumdata.calendarCodeToId(calendar))

        self.calendarName = {}
        self.monthsLong = {}
        self.monthsShort = {}
        self.monthsNarrow = {}
        self.monthsLongStandalone = {}
        self.monthsShortStandalone = {}
        self.monthsNarrowStandalone = {}
        self.daysLong = {}
        self.daysShort = {}
        self.daysNarrow = {}
        self.daysLongStandalone = {}
        self.daysShortStandalone = {}
        self.daysNarrowStandalone = {}
        self.quartersLong = {}
        self.quartersShort = {}
        self.quartersNarrow = {}
        self.quartersLongStandalone = {}
        self.quartersShortStandalone = {}
        self.quartersNarrowStandalone = {}
        self.amLong = {}
        self.amShort = {}
        self.amNarrow = {}
        self.amLongStandalone = {}
        self.amShortStandalone = {}
        self.amNarrowStandalone = {}
        self.pmLong = {}
        self.pmShort = {}
        self.pmNarrow = {}
        self.pmLongStandalone = {}
        self.pmShortStandalone = {}
        self.pmNarrowStandalone = {}
        self.dateFormatFull = {}
        self.dateFormatLong = {}
        self.dateFormatMedium = {}
        self.dateFormatShort = {}
        self.timeFormatFull = {}
        self.timeFormatLong = {}
        self.timeFormatMedium = {}
        self.timeFormatShort = {}
        self.dateTimeFormatFull = {}
        self.dateTimeFormatLong = {}
        self.dateTimeFormatMedium = {}
        self.dateTimeFormatShort = {}

        calendarElt = firstChildElt(elt, "calendar")
        while calendarElt:
            calendarType = calendarElt.attributes["type"].value
            self.calendarName[calendarType] = eltText(firstChildElt(calendarElt, "calendarName"))
            self.monthsLong[calendarType]   = eltText(firstChildElt(calendarElt, "monthsLong"))
            self.monthsShort[calendarType]  = eltText(firstChildElt(calendarElt, "monthsShort"))
            self.monthsNarrow[calendarType] = eltText(firstChildElt(calendarElt, "monthsNarrow"))
            self.monthsLongStandalone[calendarType]   = eltText(firstChildElt(calendarElt, "monthsLongStandalone"))
            self.monthsShortStandalone[calendarType]  = eltText(firstChildElt(calendarElt, "monthsShortStandalone"))
            self.monthsNarrowStandalone[calendarType] = eltText(firstChildElt(calendarElt, "monthsNarrowStandalone"))
            self.daysLong[calendarType]   = eltText(firstChildElt(calendarElt, "daysLong"))
            self.daysShort[calendarType]  = eltText(firstChildElt(calendarElt, "daysShort"))
            self.daysNarrow[calendarType] = eltText(firstChildElt(calendarElt, "daysNarrow"))
            self.daysLongStandalone[calendarType]   = eltText(firstChildElt(calendarElt, "daysLongStandalone"))
            self.daysShortStandalone[calendarType]  = eltText(firstChildElt(calendarElt, "daysShortStandalone"))
            self.daysNarrowStandalone[calendarType] = eltText(firstChildElt(calendarElt, "daysNarrowStandalone"))
            self.quartersLong[calendarType]   = eltText(firstChildElt(calendarElt, "quartersLong"))
            self.quartersShort[calendarType]  = eltText(firstChildElt(calendarElt, "quartersShort"))
            self.quartersNarrow[calendarType] = eltText(firstChildElt(calendarElt, "quartersNarrow"))
            self.quartersLongStandalone[calendarType]   = eltText(firstChildElt(calendarElt, "quartersLongStandalone"))
            self.quartersShortStandalone[calendarType]  = eltText(firstChildElt(calendarElt, "quartersShortStandalone"))
            self.quartersNarrowStandalone[calendarType] = eltText(firstChildElt(calendarElt, "quartersNarrowStandalone"))
            self.amLong[calendarType]   = eltText(firstChildElt(calendarElt, "amLong"))
            self.amShort[calendarType]  = eltText(firstChildElt(calendarElt, "amShort"))
            self.amNarrow[calendarType] = eltText(firstChildElt(calendarElt, "amNarrow"))
            self.amLongStandalone[calendarType]   = eltText(firstChildElt(calendarElt, "amLongStandalone"))
            self.amShortStandalone[calendarType]  = eltText(firstChildElt(calendarElt, "amShortStandalone"))
            self.amNarrowStandalone[calendarType] = eltText(firstChildElt(calendarElt, "amNarrowStandalone"))
            self.pmLong[calendarType]   = eltText(firstChildElt(calendarElt, "pmLong"))
            self.pmShort[calendarType]  = eltText(firstChildElt(calendarElt, "pmShort"))
            self.pmNarrow[calendarType] = eltText(firstChildElt(calendarElt, "pmNarrow"))
            self.pmLongStandalone[calendarType]   = eltText(firstChildElt(calendarElt, "pmLongStandalone"))
            self.pmShortStandalone[calendarType]  = eltText(firstChildElt(calendarElt, "pmShortStandalone"))
            self.pmNarrowStandalone[calendarType] = eltText(firstChildElt(calendarElt, "pmNarrowStandalone"))
            self.dateFormatFull[calendarType]   = convertFormat(eltText(firstChildElt(calendarElt, "dateFormatFull")))
            self.dateFormatLong[calendarType]   = convertFormat(eltText(firstChildElt(calendarElt, "dateFormatLong")))
            self.dateFormatMedium[calendarType] = convertFormat(eltText(firstChildElt(calendarElt, "dateFormatMedium")))
            self.dateFormatShort[calendarType]  = convertFormat(eltText(firstChildElt(calendarElt, "dateFormatShort")))
            self.timeFormatFull[calendarType]   = convertFormat(eltText(firstChildElt(calendarElt, "timeFormatFull")))
            self.timeFormatLong[calendarType]   = convertFormat(eltText(firstChildElt(calendarElt, "timeFormatLong")))
            self.timeFormatMedium[calendarType] = convertFormat(eltText(firstChildElt(calendarElt, "timeFormatMedium")))
            self.timeFormatShort[calendarType]  = convertFormat(eltText(firstChildElt(calendarElt, "timeFormatShort")))
            self.dateTimeFormatFull[calendarType]   = convertFormat(eltText(firstChildElt(calendarElt, "dateTimeFormatFull")))
            self.dateTimeFormatLong[calendarType]   = convertFormat(eltText(firstChildElt(calendarElt, "dateTimeFormatLong")))
            self.dateTimeFormatMedium[calendarType] = convertFormat(eltText(firstChildElt(calendarElt, "dateTimeFormatMedium")))
            self.dateTimeFormatShort[calendarType]  = convertFormat(eltText(firstChildElt(calendarElt, "dateTimeFormatShort")))
            calendarElt = nextSiblingElt(calendarElt, "calendar")

def loadLocaleMap(doc, language_map, script_map, country_map):
    result = {}

    locale_list_elt = firstChildElt(doc.documentElement, "localeList")
    locale_elt = firstChildElt(locale_list_elt, "locale")
    while locale_elt:
        locale = Locale(locale_elt)
        language_id = languageNameToId(locale.language, language_map)
        if language_id == -1:
            sys.stderr.write("Cannot find a language id for '%s'\n" % locale.language)
        script_id = scriptNameToId(locale.script, script_map)
        if script_id == -1:
            sys.stderr.write("Cannot find a script id for '%s'\n" % locale.script)
        country_id = countryNameToId(locale.country, country_map)
        if country_id == -1:
            sys.stderr.write("Cannot find a country id for '%s'\n" % locale.country)
        result[(language_id, script_id, country_id)] = locale

        locale_elt = nextSiblingElt(locale_elt, "locale")

    return result

def compareLocaleKeys(key1, key2):
    if key1 == key2:
        return 0

    if key1[0] == key2[0]:
        l1 = compareLocaleKeys.locale_map[key1]
        l2 = compareLocaleKeys.locale_map[key2]

        if l1.language in compareLocaleKeys.default_map:
            default = compareLocaleKeys.default_map[l1.language]
            if l1.country == default and key1[1] == 0:
                return -1
            if l2.country == default and key2[1] == 0:
                return 1

        if key1[1] != key2[1]:
            return key1[1] - key2[1]
    else:
        return key1[0] - key2[0]

    return key1[2] - key2[2]


def languageCount(language_id, locale_map):
    result = 0
    for key in locale_map.keys():
        if key[0] == language_id:
            result += 1
    return result

def unicode2hex(s):
    lst = []
    for x in s:
        v = ord(x)
        if v > 0xFFFF:
            # make a surrogate pair
            # copied from qchar.h
            high = (v >> 10) + 0xd7c0
            low = (v % 0x400 + 0xdc00)
            lst.append(hex(high))
            lst.append(hex(low))
        else:
            lst.append(hex(v))
    return lst

class DataToken:
    def __init__(self, index, length):
        if index > 0xFFFF or length > 0xFFFF:
            raise Error("Position exceeds ushort range: %d,%d " % (index, length))
        self.index = index
        self.length = length
    def __str__(self):
        return " %d,%d " % (self.index, self.length)

class StringData:
    def __init__(self):
        self.data = []
        self.hash = {}
    def append(self, s):
        if s in self.hash:
            return self.hash[s]

        lst = unicode2hex(s)
        index = len(self.data)
        if index > 65535:
            print "\n\n\n#error Data index is too big!"
            sys.stderr.write ("\n\n\nERROR: index exceeds the uint16 range! index = %d\n" % index)
            sys.exit(1)
        size = len(lst)
        if size >= 65535:
            print "\n\n\n#error Data is too big!"
            sys.stderr.write ("\n\n\nERROR: data size exceeds the uint16 range! size = %d\n" % size)
            sys.exit(1)
        token = None
        try:
            token = DataToken(index, size)
        except Error as e:
            sys.stderr.write("\n\n\nERROR: %s: on data '%s'" % (e, s))
            sys.exit(1)
        self.hash[s] = token
        self.data += lst
        return token

class NumberListData:
    def __init__(self):
        self.data = []
        self.hash = {}
    def append(self, lst):
        key = ""
        for num in lst :
            key += str(num)
        if key in self.hash:
            return self.hash[key]

        index = len(self.data)
        if index > 65535:
            print "\n\n\n#error Data index is too big!"
            sys.stderr.write ("\n\n\nERROR: index exceeds the uint16 range! index = %d\n" % index)
            sys.exit(1)
        size = len(lst)
        if size >= 65535:
            print "\n\n\n#error Data is too big!"
            sys.stderr.write ("\n\n\nERROR: data size exceeds the uint16 range! size = %d\n" % size)
            sys.exit(1)
        token = None
        try:
            token = DataToken(index, size)
        except Error as e:
            sys.stderr.write("\n\n\nERROR: %s: on data '%s'" % (e, lst))
            sys.exit(1)
        self.hash[key] = token
        self.data += lst
        return token

def escapedString(s):
    result = ""
    i = 0
    while i < len(s):
        if s[i] == '"':
            result += '\\"'
            i += 1
        else:
            result += s[i]
            i += 1
    s = result

    line = ""
    need_escape = False
    result = ""
    for c in s:
        if ord(c) < 128 and (not need_escape or ord(c.lower()) < ord('a') or ord(c.lower()) > ord('f')):
            line += c
            need_escape = False
        else:
            line += "\\x%02x" % (ord(c))
            need_escape = True
        if len(line) > 80:
            result = result + "\n" + "\"" + line + "\""
            line = ""
    line += "\\0"
    result = result + "\n" + "\"" + line + "\""
    if result[0] == "\n":
        result = result[1:]
    return result

def printEscapedString(s):
    print escapedString(s);


def currencyIsoCodeData(s):
    if s:
        return ",".join(map(lambda x: str(ord(x)), s))
    return "0,0,0"

def usage():
    print "Usage: qlocalexml2cpp.py <path-to-locale.xml> <path-to-qt-src-tree>"
    sys.exit(1)

GENERATED_BLOCK_START = "// GENERATED PART STARTS HERE\n"
GENERATED_BLOCK_END = "// GENERATED PART ENDS HERE\n"

def main():
    if len(sys.argv) != 3:
        usage()

    localexml = sys.argv[1]
    qtsrcdir = sys.argv[2]

    if not os.path.exists(qtsrcdir) or not os.path.exists(qtsrcdir):
        usage()
    if not os.path.isfile(qtsrcdir + "/src/corelib/tools/qlocale_data_p.h"):
        usage()
    if not os.path.isfile(qtsrcdir + "/src/corelib/tools/qlocale.h"):
        usage()
    if not os.path.isfile(qtsrcdir + "/src/corelib/tools/qlocale.qdoc"):
        usage()

    (data_temp_file, data_temp_file_path) = tempfile.mkstemp("qlocale_data_p", dir=qtsrcdir)
    data_temp_file = os.fdopen(data_temp_file, "w")
    qlocaledata_file = open(qtsrcdir + "/src/corelib/tools/qlocale_data_p.h", "r")
    s = qlocaledata_file.readline()
    while s and s != GENERATED_BLOCK_START:
        data_temp_file.write(s)
        s = qlocaledata_file.readline()
    data_temp_file.write(GENERATED_BLOCK_START)

    doc = xml.dom.minidom.parse(localexml)
    language_map = loadLanguageMap(doc)
    script_map = loadScriptMap(doc)
    country_map = loadCountryMap(doc)
    default_map = loadDefaultMap(doc)
    locale_map = loadLocaleMap(doc, language_map, script_map, country_map)
    dupes = findDupes(language_map, country_map)

    cldr_version = eltText(firstChildElt(doc.documentElement, "version"))

    data_temp_file.write("\n\
/*\n\
    This part of the file was generated on %s from the\n\
    Common Locale Data Repository v%s\n\
\n\
    http://www.unicode.org/cldr/\n\
\n\
    Do not change it, instead edit CLDR data and regenerate this file using\n\
    cldr2qlocalexml.py and qlocalexml2cpp.py.\n\
*/\n\n\n\
" % (str(datetime.date.today()), cldr_version) )

    # Locale index
    data_temp_file.write("static const quint16 locale_index[] = {\n")
    index = 0
    for key in language_map.keys():
        i = 0
        count = languageCount(key, locale_map)
        if count > 0:
            i = index
            index += count
        data_temp_file.write("%6d, // %s\n" % (i, language_map[key][0]))
    data_temp_file.write("     0 // trailing 0\n")
    data_temp_file.write("};\n")

    data_temp_file.write("\n")

    list_pattern_part_data = StringData()
    currency_symbol_data = StringData()
    currency_display_name_data = StringData()
    currency_format_data = StringData()
    endonyms_data = StringData()
    calendar_preference_data = NumberListData()
    calendar_name_data = StringData()
    months_data = StringData()
    days_data = StringData()
    quarters_data = StringData()
    day_periods_data = StringData()
    date_format_data = StringData()
    time_format_data = StringData()
    date_time_format_data = StringData()
    # Second copy of the calendar data to use for QT_NO_CALENDAR_SYSTEMS
    gregorian_months_data = StringData()
    gregorian_days_data = StringData()
    gregorian_quarters_data = StringData()
    gregorian_day_periods_data = StringData()
    gregorian_date_format_data = StringData()
    gregorian_time_format_data = StringData()
    gregorian_date_time_format_data = StringData()

    # Locale data
    data_temp_file.write("static const QLocalePrivate locale_data[] = {\n")
    data_temp_file.write("//  lang  langEndo  script  terr  terrEndo  dec  group  list  pcnt  zero  minus  plus  exp\n")
    data_temp_file.write("//  quotStart  quotEnd  altQuotStart  altQuotEnd  lpStart  lpMid  lpEnd  lpTwo\n")
    data_temp_file.write("//  currCode  currSym  currName  currDig  currRnd  currFmt  currFmtNeg  fdow  wkedStrt  wkedEnd\n")
    data_temp_file.write("//  calIdx  calDef  calPref  calName  mthLong  mthShort  mthNrw  mthLongSt  mthShortSt  mthNrwSt\n")
    data_temp_file.write("//  dayLong  dayShort  dayNrw  dayLongSt  dayShortSt  dayNrwSt  qtrLong  qtrShort  qtrNrw  qtrLongSt  qtrShortSt  qtrNrwSt\n")
    data_temp_file.write("//  amLong  amShort  amNrw  amLongSt  amShortSt  amNrwSt  pmLong  pmShort  pmNrw  pmLongSt  pmShortSt  pmNrwSt\n")
    data_temp_file.write("//  dtFmtFull  dtFmtLong  dtFmtMed  dtFmtShort  tmFmtFull  tmFmtLong  tmFmtMed  tmFmtShort  dttmFmtFull  dttmFmtLong  dttmFmtMed  dttmFmtShort\n")

    locale_keys = locale_map.keys()
    compareLocaleKeys.default_map = default_map
    compareLocaleKeys.locale_map = locale_map
    locale_keys.sort(compareLocaleKeys)
    calendar_keys = []
    for calendarId in enumdata.calendar_list:
        calendar_keys.append(enumdata.calendar_list[calendarId][1])

    def writeNumber(data, comma = True):
        if comma :
            data_temp_file.write(" , %6d" % (data))
        else :
            data_temp_file.write("%6d" % (data))

    def writeString(data, comma = True):
        if comma :
            data_temp_file.write(" , %s" % (data))
        else :
            data_temp_file.write("%s" % (data))

    def writeStringArray(data, comma = True):
        if comma :
            data_temp_file.write(" , { %s }" % (data))
        else :
            data_temp_file.write("{ %s }" % (data))

    def writeCalendar(dataMap, data, calendar, comma = True) :
        writeString(dataMap.append(data[calendar]), comma)

    def writeCalendarFields(l, calendar, pad, writeGregorian = False):
        months = StringData()
        days = StringData()
        quarters = StringData()
        periods = StringData()
        dates = StringData()
        times = StringData()
        datetimes = StringData()
        if writeGregorian:
            months = gregorian_months_data
            days = gregorian_days_data
            quarters = gregorian_quarters_data
            periods = gregorian_day_periods_data
            dates = gregorian_date_format_data
            times = gregorian_time_format_data
            datetimes = gregorian_date_time_format_data
            data_temp_file.write(pad)
        else:
            months = months_data
            days = days_data
            quarters = quarters_data
            periods = day_periods_data
            dates = date_format_data
            times = time_format_data
            datetimes = date_time_format_data
        if writeGregorian:
            writeCalendar(months, l.monthsLong, calendar, False)               # m_month_names_long_idx, m_month_names_long_size
        else:
            writeCalendar(calendar_name_data, l.calendarName, calendar, False) # m_calendar_name_idx, m_calendar_name_size
            writeCalendar(months, l.monthsLong, calendar)                      # m_month_names_long_idx, m_month_names_long_size
        writeCalendar(months, l.monthsShort, calendar)                         # m_month_names_short_idx, m_month_names_short_size
        writeCalendar(months, l.monthsNarrow, calendar)                        # m_month_names_narrow_idx, m_month_names_narrow_size
        writeCalendar(months, l.monthsLongStandalone, calendar)                # m_month_names_long_standalone_idx, m_month_names_long_standalone_size
        writeCalendar(months, l.monthsShortStandalone, calendar)               # m_month_names_short_standalone_idx, m_month_names_short_standalone_size
        writeCalendar(months, l.monthsNarrowStandalone, calendar)              # m_month_names_narrow_standalone_idx, m_month_names_narrow_standalone_size
        data_temp_file.write(",\n")
        data_temp_file.write(pad)
        writeCalendar(days, l.daysLong, calendar, False)                       # m_day_names_long_idx, m_day_names_long_size
        writeCalendar(days, l.daysShort, calendar)                             # m_day_names_short_idx, m_day_names_short_size
        writeCalendar(days, l.daysNarrow, calendar)                            # m_day_names_narrow_idx, m_day_names_narrow_size
        writeCalendar(days, l.daysLongStandalone, calendar)                    # m_day_names_long_standalone_idx, m_day_names_long_standalone_size
        writeCalendar(days, l.daysShortStandalone, calendar)                   # m_day_names_short_standalone_idx, m_day_names_narrow_standalone_size
        writeCalendar(days, l.daysNarrowStandalone, calendar)                  # m_day_names_narrow_standalone_idx, m_day_names_narrow_standalone_size
        writeCalendar(quarters, l.quartersLong, calendar)                      # m_quarter_names_long_idx, m_quarter_names_long_size
        writeCalendar(quarters, l.quartersShort, calendar)                     # m_quarter_names_short_idx, m_quarter_names_short_size
        writeCalendar(quarters, l.quartersNarrow, calendar)                    # m_quarter_names_narrow_idx, m_quarter_names_narrow_size
        writeCalendar(quarters, l.quartersLongStandalone, calendar)            # m_quarter_names_long_standalone_idx, m_quarter_names_long_standalone_size
        writeCalendar(quarters, l.quartersShortStandalone, calendar)           # m_quarter_names_short_standalone_idx, m_quarter_names_short_standalone_size
        writeCalendar(quarters, l.quartersNarrowStandalone, calendar)          # m_quarter_names_narrow_standalone_idx, m_quarter_names_narrow_standalone_size
        data_temp_file.write(",\n")
        data_temp_file.write(pad)
        writeCalendar(periods, l.amLong, calendar, False)                      # m_am_long_idx, m_am_long_size
        writeCalendar(periods, l.amShort, calendar)                            # m_am_short_idx, m_am_short_size
        writeCalendar(periods, l.amNarrow, calendar)                           # m_am_narrow_idx, m_am_narrow_size
        writeCalendar(periods, l.amLongStandalone, calendar)                   # m_am_long_standalone_idx, m_am_long_standalone_size
        writeCalendar(periods, l.amShortStandalone, calendar)                  # m_am_short_standalone_idx, m_am_short_standalone_size
        writeCalendar(periods, l.amNarrowStandalone, calendar)                 # m_am_narrow_standalone_idx, m_am_narrow_standalone_size
        writeCalendar(periods, l.pmLong, calendar)                             # m_pm_long_idx, m_pm_long_size
        writeCalendar(periods, l.pmShort, calendar)                            # m_pm_short_idx, m_pm_short_size
        writeCalendar(periods, l.pmNarrow, calendar)                           # m_pm_narrow_idx, m_pm_narrow_size
        writeCalendar(periods, l.pmLongStandalone, calendar)                   # m_pm_long_standalone_idx, m_pm_long_standalone_size
        writeCalendar(periods, l.pmShortStandalone, calendar)                  # m_pm_short_standalone_idx, m_pm_short_standalone_size
        writeCalendar(periods, l.pmNarrowStandalone, calendar)                 # m_pm_narrow_standalone_idx, m_pm_narrow_standalone_size
        data_temp_file.write(",\n")
        data_temp_file.write(pad)
        writeCalendar(dates, l.dateFormatFull, calendar, False)                # m_date_format_full_idx, m_date_format_full_size
        writeCalendar(dates, l.dateFormatLong, calendar)                       # m_date_format_long_idx, m_date_format_long_size
        writeCalendar(dates, l.dateFormatMedium, calendar)                     # m_date_format_medium_idx, m_date_format_medium_size
        writeCalendar(dates, l.dateFormatShort, calendar)                      # m_date_format_short_idx, m_date_format_short_size
        writeCalendar(times, l.timeFormatFull, calendar)                       # m_time_format_full_idx, m_time_format_full_size
        writeCalendar(times, l.timeFormatLong, calendar)                       # m_time_format_long_idx, m_time_format_long_size
        writeCalendar(times, l.timeFormatMedium, calendar)                     # m_time_format_medium_idx, m_time_format_medium_size
        writeCalendar(times, l.timeFormatShort, calendar)                      # m_time_format_short_idx, m_time_format_short_size
        writeCalendar(datetimes, l.dateTimeFormatFull, calendar)               # m_date_time_format_full_idx, m_date_time_format_full_size
        writeCalendar(datetimes, l.dateTimeFormatLong, calendar)               # m_date_time_format_long_idx, m_date_time_format_long_size
        writeCalendar(datetimes, l.dateTimeFormatMedium, calendar)             # m_date_time_format_medium_idx, m_date_time_format_medium_size
        writeCalendar(datetimes, l.dateTimeFormatShort, calendar)              # m_date_time_format_short_idx, m_date_time_format_short_size

    def appendCalendarData(dataMap, data, calendar):
        dataMap.append(data[calendar])

    def appendAllCalendarsData(dataMap, data):
        for calendar in calendar_keys:
            appendCalendarData(dataMap, data, calendar)

    def appendCalendarNameData(data):
        appendAllCalendarsData(calendar_name_data, data)

    def appendMonthsData(data):
        appendAllCalendarsData(months_data, data)
        appendCalendarData(gregorian_months_data, data, "gregorian")

    def appendDaysData(data):
        appendAllCalendarsData(days_data, data)
        appendCalendarData(gregorian_days_data, data, "gregorian")

    def appendQuartersData(data):
        appendAllCalendarsData(quarters_data, data)
        appendCalendarData(gregorian_quarters_data, data, "gregorian")

    def appendDayPeriodsData(data):
        appendAllCalendarsData(day_periods_data, data)
        appendCalendarData(gregorian_day_periods_data, data, "gregorian")

    def appendDateFormatData(data):
        appendAllCalendarsData(date_format_data, data)
        appendCalendarData(gregorian_date_format_data, data, "gregorian")

    def appendTimeFormatData(data):
        appendAllCalendarsData(time_format_data, data)
        appendCalendarData(gregorian_time_format_data, data, "gregorian")

    def appendDateTimeFormatData(data):
        appendAllCalendarsData(date_time_format_data, data)
        appendCalendarData(gregorian_date_time_format_data, data, "gregorian")

    def appendLocaleCalendarData(l):
        appendCalendarNameData(l.calendarName)                                 # m_calendar_name_idx, m_calendar_name_size
        appendMonthsData(l.monthsLong)                                         # m_month_names_long_idx, m_month_names_long_size
        appendMonthsData(l.monthsShort)                                        # m_month_names_short_idx, m_month_names_short_size
        appendMonthsData(l.monthsNarrow)                                       # m_month_names_narrow_idx, m_month_names_narrow_size
        appendMonthsData(l.monthsLongStandalone)                               # m_month_names_long_standalone_idx, m_month_names_long_standalone_size
        appendMonthsData(l.monthsShortStandalone)                              # m_month_names_short_standalone_idx, m_month_names_short_standalone_size
        appendMonthsData(l.monthsNarrowStandalone)                             # m_month_names_narrow_standalone_idx, m_month_names_narrow_standalone_size
        appendDaysData(l.daysLong)                                             # m_day_names_long_idx, m_day_names_long_size
        appendDaysData(l.daysShort)                                            # m_day_names_short_idx, m_day_names_short_size
        appendDaysData(l.daysNarrow)                                           # m_day_names_narrow_idx, m_day_names_narrow_size
        appendDaysData(l.daysLongStandalone)                                   # m_day_names_long_standalone_idx, m_day_names_long_standalone_size
        appendDaysData(l.daysShortStandalone)                                  # m_day_names_short_standalone_idx, m_day_names_narrow_standalone_size
        appendDaysData(l.daysNarrowStandalone)                                 # m_day_names_narrow_standalone_idx, m_day_names_narrow_standalone_size
        appendQuartersData(l.quartersLong)                                     # m_quarter_names_long_idx, m_quarter_names_long_size
        appendQuartersData(l.quartersShort)                                    # m_quarter_names_short_idx, m_quarter_names_short_size
        appendQuartersData(l.quartersNarrow)                                   # m_quarter_names_narrow_idx, m_quarter_names_narrow_size
        appendQuartersData(l.quartersLongStandalone)                           # m_quarter_names_long_standalone_idx, m_quarter_names_long_standalone_size
        appendQuartersData(l.quartersShortStandalone)                          # m_quarter_names_short_standalone_idx, m_quarter_names_short_standalone_size
        appendQuartersData(l.quartersNarrowStandalone)                         # m_quarter_names_narrow_standalone_idx, m_quarter_names_narrow_standalone_size
        appendDayPeriodsData(l.amLong)                                         # m_am_long_idx, m_am_long_size
        appendDayPeriodsData(l.amShort)                                        # m_am_short_idx, m_am_short_size
        appendDayPeriodsData(l.amNarrow)                                       # m_am_narrow_idx, m_am_narrow_size
        appendDayPeriodsData(l.amLongStandalone)                               # m_am_long_standalone_idx, m_am_long_standalone_size
        appendDayPeriodsData(l.amShortStandalone)                              # m_am_short_standalone_idx, m_am_short_standalone_size
        appendDayPeriodsData(l.amNarrowStandalone)                             # m_am_narrow_standalone_idx, m_am_narrow_standalone_size
        appendDayPeriodsData(l.pmLong)                                         # m_pm_long_idx, m_pm_long_size
        appendDayPeriodsData(l.pmShort)                                        # m_pm_short_idx, m_pm_short_size
        appendDayPeriodsData(l.pmNarrow)                                       # m_pm_narrow_idx, m_pm_narrow_size
        appendDayPeriodsData(l.pmLongStandalone)                               # m_pm_long_standalone_idx, m_pm_long_standalone_size
        appendDayPeriodsData(l.pmShortStandalone)                              # m_pm_short_standalone_idx, m_pm_short_standalone_size
        appendDayPeriodsData(l.pmNarrowStandalone)                             # m_pm_narrow_standalone_idx, m_pm_narrow_standalone_size
        appendDateFormatData(l.dateFormatFull)                                 # m_date_format_full_idx, m_date_format_full_size
        appendDateFormatData(l.dateFormatLong)                                 # m_date_format_long_idx, m_date_format_long_size
        appendDateFormatData(l.dateFormatMedium)                               # m_date_format_medium_idx, m_date_format_medium_size
        appendDateFormatData(l.dateFormatShort)                                # m_date_format_short_idx, m_date_format_short_size
        appendTimeFormatData(l.timeFormatFull)                                 # m_time_format_full_idx, m_time_format_full_size
        appendTimeFormatData(l.timeFormatLong)                                 # m_time_format_long_idx, m_time_format_long_size
        appendTimeFormatData(l.timeFormatMedium)                               # m_time_format_medium_idx, m_time_format_medium_size
        appendTimeFormatData(l.timeFormatShort)                                # m_time_format_short_idx, m_time_format_short_size
        appendDateTimeFormatData(l.dateTimeFormatFull)                         # m_date_time_format_full_idx, m_date_time_format_full_size
        appendDateTimeFormatData(l.dateTimeFormatLong)                         # m_date_time_format_long_idx, m_date_time_format_long_size
        appendDateTimeFormatData(l.dateTimeFormatMedium)                       # m_date_time_format_medium_idx, m_date_time_format_medium_size
        appendDateTimeFormatData(l.dateTimeFormatShort)                        # m_date_time_format_short_idx, m_date_time_format_short_size

    calendarIndex = 0
    for key in locale_keys:
        l = locale_map[key]
        data_temp_file.write("    {\n      ")
        writeNumber(key[0], False)                                             # m_language_id
        writeString(endonyms_data.append(l.languageEndonym))                   # m_language_endonym_idx, m_language_endonym_size
        writeNumber(key[1])                                                    # m_script_id
        writeNumber(key[2])                                                    # m_country_id
        writeString(endonyms_data.append(l.countryEndonym))                    # m_country_endonym_idx, m_country_endonym_size
        writeNumber(l.decimal)                                                 # m_decimal
        writeNumber(l.group)                                                   # m_group
        writeNumber(l.listDelim)                                               # m_list
        writeNumber(l.percent)                                                 # m_percent
        writeNumber(l.zero)                                                    # m_zero
        writeNumber(l.minus)                                                   # m_minus
        writeNumber(l.plus)                                                    # m_plus
        writeNumber(l.exp)                                                     # m_exponential
        data_temp_file.write(",\n      ")
        writeNumber(l.quotationStart, False)                                   # m_quotation_start
        writeNumber(l.quotationEnd)                                            # m_quotation_end
        writeNumber(l.alternateQuotationStart)                                 # m_alternate_quotation_start
        writeNumber(l.alternateQuotationEnd)                                   # m_alternate_quotation_end
        writeString(list_pattern_part_data.append(l.listPatternPartStart))     # m_list_pattern_part_start_idx, m_list_pattern_part_start_size
        writeString(list_pattern_part_data.append(l.listPatternPartMiddle))    # m_list_pattern_part_mid_idx, m_list_pattern_part_mid_size
        writeString(list_pattern_part_data.append(l.listPatternPartEnd))       # m_list_pattern_part_end_idx, m_list_pattern_part_end_size
        writeString(list_pattern_part_data.append(l.listPatternPartTwo))       # m_list_pattern_part_two_idx, m_list_pattern_part_two_size
        data_temp_file.write(",\n        ")
        writeStringArray(currencyIsoCodeData(l.currencyIsoCode), False)        # m_currency_iso_code
        writeString(currency_symbol_data.append(l.currencySymbol))             # m_currency_symbol_idx, m_currency_symbol_size
        writeString(currency_display_name_data.append(l.currencyDisplayName))  # m_currency_display_name_idx, m_currency_display_name_size
        writeNumber(l.currencyDigits)                                          # m_currency_digits
        writeNumber(l.currencyRounding)                                        # m_currency_rounding
        writeString(currency_format_data.append(l.currencyFormat))             # m_currency_format_idx, m_currency_format_size
        writeString(currency_format_data.append(l.currencyNegativeFormat))     # m_currency_negative_format_idx, m_currency_negative_format_size
        writeNumber(l.firstDayOfWeek)                                          # m_first_day_of_week
        writeNumber(l.weekendStart)                                            # m_weekend_start
        writeNumber(l.weekendEnd)                                              # m_weekend_end
        data_temp_file.write(",\n")
        appendLocaleCalendarData(l)
        # Only write out the default calendar settings in the main locale index to save memory
        data_temp_file.write("#ifdef QT_NO_CALENDAR_SYSTEMS\n")
        writeCalendarFields(l, "gregorian", "       ", True)
        data_temp_file.write("\n#else\n    ")
        writeNumber(calendarIndex, False)                                      # m_calendar_data_index
        writeNumber(l.calendarPreferenceIdList[0])                             # m_calendar_default
        writeString(calendar_preference_data.append(l.calendarPreferenceIdList)) # m_calendar_preference_idx, m_calendar_preference_size
        data_temp_file.write(",")
        writeCalendarFields(l, l.calendarPreferenceList[0], "       ")
        data_temp_file.write("\n#endif\n")
        calendarIndex += 1

        data_temp_file.write("    }, // %s/%s/%s\n" % (l.language, l.script, l.country))

    data_temp_file.write("    {   0 ,  0,0 ,  0 ,  0 ,  0,0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,\n")
    data_temp_file.write("        0 ,  0 ,  0 ,  0 ,  0,0 ,  0,0 , 0,0 , 0,0 ,\n")
    data_temp_file.write("        { 0,0,0 } ,  0,0 ,  0,0 ,  0 ,  0 ,  0,0 ,  0,0 ,  0 ,  0 ,  0 ,\n")
    data_temp_file.write("#ifdef QT_NO_CALENDAR_SYSTEMS\n")
    data_temp_file.write("        0,0 ,  0,0 ,  0,0 ,  0,0 ,  0,0 ,  0,0 ,\n")
    data_temp_file.write("#else\n")
    data_temp_file.write("        0 ,  0 ,  0,0 ,  0,0 ,  0,0 ,  0,0 ,  0,0 ,  0,0 ,  0,0 ,  0,0 ,\n")
    data_temp_file.write("#endif\n")
    data_temp_file.write("        0,0 ,  0,0 ,  0,0 , 0,0 , 0,0 , 0,0 , 0,0 ,  0,0 ,  0,0 , 0,0 , 0,0 , 0,0 ,\n")
    data_temp_file.write("        0,0 ,  0,0 ,  0,0 , 0,0 , 0,0 , 0,0 , 0,0 ,  0,0 ,  0,0 , 0,0 , 0,0 , 0,0 ,\n")
    data_temp_file.write("        0,0 ,  0,0 ,  0,0 , 0,0 , 0,0 , 0,0 , 0,0 ,  0,0 ,  0,0 , 0,0 , 0,0 , 0,0\n")
    data_temp_file.write("    }  // trailing 0s\n")

    data_temp_file.write("};\n")

    data_temp_file.write("\n")

    # Calendar data index
    data_temp_file.write("#ifndef QT_NO_CALENDAR_SYSTEMS\n")
    data_temp_file.write("static const ushort calendar_index_data[] = {\n")
    calendarIndex = 0
    for key in locale_keys:
        l = locale_map[key]
        for calendar in calendar_keys:
            data_temp_file.write("// %d : %s / %s / %s / %s\n" % (calendarIndex, l.language, l.script, l.country, calendar))
            writeCalendarFields(l, calendar, "")
            data_temp_file.write(",\n")
        calendarIndex += 1
    calAtt = range(42);  # there are 43 calendar attributes, but we need to manually write the last
    first = True
    for i in calAtt:
        if first == True :
            data_temp_file.write( " 0,0" )
            first = False
        else :
            data_temp_file.write( " , 0,0" )
    data_temp_file.write("\n};\n")
    data_temp_file.write("#endif\n\n")

    # List patterns data
    #check_static_char_array_length("list_pattern_part", list_pattern_part_data.data)
    data_temp_file.write("static const ushort list_pattern_part_data[] = {\n")
    data_temp_file.write(wrap_list(list_pattern_part_data.data))
    data_temp_file.write("\n};\n")

    data_temp_file.write("\n")

    # Currency symbol data
    #check_static_char_array_length("currency_symbol", currency_symbol_data.data)
    data_temp_file.write("static const ushort currency_symbol_data[] = {\n")
    data_temp_file.write(wrap_list(currency_symbol_data.data))
    data_temp_file.write("\n};\n")

    data_temp_file.write("\n")

    # Currency display name data
    #check_static_char_array_length("currency_display_name", currency_display_name_data.data)
    data_temp_file.write("static const ushort currency_display_name_data[] = {\n")
    data_temp_file.write(wrap_list(currency_display_name_data.data))
    data_temp_file.write("\n};\n")

    data_temp_file.write("\n")

    # Currency format data
    #check_static_char_array_length("currency_format", currency_format_data.data)
    data_temp_file.write("static const ushort currency_format_data[] = {\n")
    data_temp_file.write(wrap_list(currency_format_data.data))
    data_temp_file.write("\n};\n")

    # Endonyms data
    #check_static_char_array_length("endonyms", endonyms_data.data)
    data_temp_file.write("static const ushort endonyms_data[] = {\n")
    data_temp_file.write(wrap_list(endonyms_data.data))
    data_temp_file.write("\n};\n")

    data_temp_file.write("\n")

    # Calendar Preference data
    data_temp_file.write("#ifndef QT_NO_CALENDAR_SYSTEMS\n")
    data_temp_file.write("static const ushort calendar_preference_data[] = {\n")
    comma = False
    count = 0
    for pref in calendar_preference_data.data:
        writeNumber(pref, comma)
        comma = True
        count += 1
        if (count % 10 == 0):
            comma = False
            count = 0
            data_temp_file.write(",\n")
    data_temp_file.write("\n};\n")
    data_temp_file.write("#endif\n\n")

    # Calendar Name data
    #check_static_char_array_length("calendar_names", calendar_name_data.data)
    data_temp_file.write("#ifndef QT_NO_CALENDAR_SYSTEMS\n")
    data_temp_file.write("static const ushort calendar_name_data[] = {\n")
    data_temp_file.write(wrap_list(calendar_name_data.data))
    data_temp_file.write("\n};\n")
    data_temp_file.write("#endif\n\n")

    # Months data
    #check_static_char_array_length("months", months_data.data)
    data_temp_file.write("static const ushort months_data[] = {\n")
    data_temp_file.write("#ifdef QT_NO_CALENDAR_SYSTEMS\n")
    data_temp_file.write(wrap_list(gregorian_months_data.data))
    data_temp_file.write("\n#else\n")
    data_temp_file.write(wrap_list(months_data.data))
    data_temp_file.write("\n#endif\n")
    data_temp_file.write("};\n\n")

    # Days data
    #check_static_char_array_length("days", days_data.data)
    data_temp_file.write("static const ushort days_data[] = {\n")
    data_temp_file.write("#ifdef QT_NO_CALENDAR_SYSTEMS\n")
    data_temp_file.write(wrap_list(gregorian_days_data.data))
    data_temp_file.write("\n#else\n")
    data_temp_file.write(wrap_list(days_data.data))
    data_temp_file.write("\n#endif\n")
    data_temp_file.write("};\n\n")

    # Quarters data
    #check_static_char_array_length("quarters", quarters_data.data)
    data_temp_file.write("static const ushort quarters_data[] = {\n")
    data_temp_file.write("#ifdef QT_NO_CALENDAR_SYSTEMS\n")
    data_temp_file.write(wrap_list(gregorian_quarters_data.data))
    data_temp_file.write("\n#else\n")
    data_temp_file.write(wrap_list(quarters_data.data))
    data_temp_file.write("\n#endif\n")
    data_temp_file.write("};\n\n")

    # Day Period data
    #check_static_char_array_length("day_periods", day_periods_data.data)
    data_temp_file.write("static const ushort day_periods_data[] = {\n")
    data_temp_file.write("#ifdef QT_NO_CALENDAR_SYSTEMS\n")
    data_temp_file.write(wrap_list(gregorian_day_periods_data.data))
    data_temp_file.write("\n#else\n")
    data_temp_file.write(wrap_list(day_periods_data.data))
    data_temp_file.write("\n#endif\n")
    data_temp_file.write("};\n\n")

    # Date format data
    #check_static_char_array_length("date_format", date_format_data.data)
    data_temp_file.write("static const ushort date_format_data[] = {\n")
    data_temp_file.write("#ifdef QT_NO_CALENDAR_SYSTEMS\n")
    data_temp_file.write(wrap_list(gregorian_date_format_data.data))
    data_temp_file.write("\n#else\n")
    data_temp_file.write(wrap_list(date_format_data.data))
    data_temp_file.write("\n#endif\n")
    data_temp_file.write("};\n\n")

    # Time format data
    #check_static_char_array_length("time_format", time_format_data.data)
    data_temp_file.write("static const ushort time_format_data[] = {\n")
    data_temp_file.write("#ifdef QT_NO_CALENDAR_SYSTEMS\n")
    data_temp_file.write(wrap_list(gregorian_time_format_data.data))
    data_temp_file.write("\n#else\n")
    data_temp_file.write(wrap_list(time_format_data.data))
    data_temp_file.write("\n#endif\n")
    data_temp_file.write("};\n\n")

    # DateTime format data
    #check_static_char_array_length("date_time_format", date_time_format_data.data)
    data_temp_file.write("static const ushort date_time_format_data[] = {\n")
    data_temp_file.write("#ifdef QT_NO_CALENDAR_SYSTEMS\n")
    data_temp_file.write(wrap_list(gregorian_date_time_format_data.data))
    data_temp_file.write("\n#else\n")
    data_temp_file.write(wrap_list(date_time_format_data.data))
    data_temp_file.write("\n#endif\n")
    data_temp_file.write("};\n\n")

    # Language name list
    data_temp_file.write("static const char language_name_list[] =\n")
    data_temp_file.write("\"Default\\0\"\n")
    for key in language_map.keys():
        if key == 0:
            continue
        data_temp_file.write("\"" + language_map[key][0] + "\\0\"\n")
    data_temp_file.write(";\n")

    data_temp_file.write("\n")

    # Language name index
    data_temp_file.write("static const quint16 language_name_index[] = {\n")
    data_temp_file.write("     0, // AnyLanguage\n")
    index = 8
    for key in language_map.keys():
        if key == 0:
            continue
        language = language_map[key][0]
        data_temp_file.write("%6d, // %s\n" % (index, language))
        index += len(language) + 1
    data_temp_file.write("};\n")

    data_temp_file.write("\n")

    # Script name list
    data_temp_file.write("static const char script_name_list[] =\n")
    data_temp_file.write("\"Default\\0\"\n")
    for key in script_map.keys():
        if key == 0:
            continue
        data_temp_file.write("\"" + script_map[key][0] + "\\0\"\n")
    data_temp_file.write(";\n")

    data_temp_file.write("\n")

    # Script name index
    data_temp_file.write("static const quint16 script_name_index[] = {\n")
    data_temp_file.write("     0, // AnyScript\n")
    index = 8
    for key in script_map.keys():
        if key == 0:
            continue
        script = script_map[key][0]
        data_temp_file.write("%6d, // %s\n" % (index, script))
        index += len(script) + 1
    data_temp_file.write("};\n")

    data_temp_file.write("\n")

    # Country name list
    data_temp_file.write("static const char country_name_list[] =\n")
    data_temp_file.write("\"Default\\0\"\n")
    for key in country_map.keys():
        if key == 0:
            continue
        data_temp_file.write("\"" + country_map[key][0] + "\\0\"\n")
    data_temp_file.write(";\n")

    data_temp_file.write("\n")

    # Country name index
    data_temp_file.write("static const quint16 country_name_index[] = {\n")
    data_temp_file.write("     0, // AnyCountry\n")
    index = 8
    for key in country_map.keys():
        if key == 0:
            continue
        country = country_map[key][0]
        data_temp_file.write("%6d, // %s\n" % (index, country))
        index += len(country) + 1
    data_temp_file.write("};\n")

    data_temp_file.write("\n")

    # Language code list
    data_temp_file.write("static const unsigned char language_code_list[] =\n")
    for key in language_map.keys():
        code = language_map[key][1]
        if len(code) == 2:
            code += r"\0"
        data_temp_file.write("\"%2s\" // %s\n" % (code, language_map[key][0]))
    data_temp_file.write(";\n")

    data_temp_file.write("\n")

    # Script code list
    data_temp_file.write("static const unsigned char script_code_list[] =\n")
    for key in script_map.keys():
        code = script_map[key][1]
        for i in range(4 - len(code)):
            code += "\\0"
        data_temp_file.write("\"%2s\" // %s\n" % (code, script_map[key][0]))
    data_temp_file.write(";\n")

    # Country code list
    data_temp_file.write("static const unsigned char country_code_list[] =\n")
    for key in country_map.keys():
        code = country_map[key][1]
        if len(code) == 2:
            code += "\\0"
        data_temp_file.write("\"%2s\" // %s\n" % (code, country_map[key][0]))
    data_temp_file.write(";\n")

    data_temp_file.write("\n")
    data_temp_file.write(GENERATED_BLOCK_END)
    s = qlocaledata_file.readline()
    # skip until end of the block
    while s and s != GENERATED_BLOCK_END:
        s = qlocaledata_file.readline()

    s = qlocaledata_file.readline()
    while s:
        data_temp_file.write(s)
        s = qlocaledata_file.readline()
    data_temp_file.close()
    qlocaledata_file.close()

    os.rename(data_temp_file_path, qtsrcdir + "/src/corelib/tools/qlocale_data_p.h")

    # qlocale.h

    (qlocaleh_temp_file, qlocaleh_temp_file_path) = tempfile.mkstemp("qlocale.h", dir=qtsrcdir)
    qlocaleh_temp_file = os.fdopen(qlocaleh_temp_file, "w")
    qlocaleh_file = open(qtsrcdir + "/src/corelib/tools/qlocale.h", "r")
    s = qlocaleh_file.readline()
    while s and s != GENERATED_BLOCK_START:
        qlocaleh_temp_file.write(s)
        s = qlocaleh_file.readline()
    qlocaleh_temp_file.write(GENERATED_BLOCK_START)
    qlocaleh_temp_file.write("// see qlocale_data_p.h for more info on generated data\n")

    # Language enum
    qlocaleh_temp_file.write("    enum Language {\n")
    language = ""
    for key in language_map.keys():
        language = fixedLanguageName(language_map[key][0], dupes)
        qlocaleh_temp_file.write("        " + language + " = " + str(key) + ",\n")
    # special cases for norwegian. we really need to make it right at some point.
    qlocaleh_temp_file.write("        NorwegianBokmal = Norwegian,\n")
    qlocaleh_temp_file.write("        NorwegianNynorsk = Nynorsk,\n")
    qlocaleh_temp_file.write("        LastLanguage = " + language + "\n")
    qlocaleh_temp_file.write("    };\n")

    qlocaleh_temp_file.write("\n")

    # Script enum
    qlocaleh_temp_file.write("    enum Script {\n")
    script = ""
    for key in script_map.keys():
        script = fixedScriptName(script_map[key][0], dupes)
        qlocaleh_temp_file.write("        " + script + " = " + str(key) + ",\n")
    qlocaleh_temp_file.write("        SimplifiedChineseScript = SimplifiedHanScript,\n")
    qlocaleh_temp_file.write("        TraditionalChineseScript = TraditionalHanScript,\n")
    qlocaleh_temp_file.write("        LastScript = " + script + "\n")
    qlocaleh_temp_file.write("    };\n\n")

    # Country enum
    qlocaleh_temp_file.write("    enum Country {\n")
    country = ""
    for key in country_map.keys():
        country = fixedCountryName(country_map[key][0], dupes)
        qlocaleh_temp_file.write("        " + country + " = " + str(key) + ",\n")
    qlocaleh_temp_file.write("        LastCountry = " + country + "\n")
    qlocaleh_temp_file.write("    };\n\n")

    # Calendar enum
    qlocaleh_temp_file.write("    enum CalendarSystem {\n")
    qlocaleh_temp_file.write("        DefaultCalendar = -1,\n")
    qlocaleh_temp_file.write("#ifdef QT_NO_CALENDAR_SYSTEMS\n")
    qlocaleh_temp_file.write("        GregorianCalendar = 0,\n")
    qlocaleh_temp_file.write("        LastCalendar = GregorianCalendar\n")
    qlocaleh_temp_file.write("#else\n")
    calendar = ""
    for key in enumdata.calendar_list.keys():
        calendar = enumdata.calendar_list[key][0]
        qlocaleh_temp_file.write("        " + calendar + " = " + str(key) + ",\n")
    qlocaleh_temp_file.write("        LastCalendar = " + calendar + "\n")
    qlocaleh_temp_file.write("#endif\n")
    qlocaleh_temp_file.write("    };\n\n")

    qlocaleh_temp_file.write(GENERATED_BLOCK_END)
    s = qlocaleh_file.readline()
    # skip until end of the block
    while s and s != GENERATED_BLOCK_END:
        s = qlocaleh_file.readline()

    s = qlocaleh_file.readline()
    while s:
        qlocaleh_temp_file.write(s)
        s = qlocaleh_file.readline()
    qlocaleh_temp_file.close()
    qlocaleh_file.close()

    os.rename(qlocaleh_temp_file_path, qtsrcdir + "/src/corelib/tools/qlocale.h")

    # qlocale.qdoc

    (qlocaleqdoc_temp_file, qlocaleqdoc_temp_file_path) = tempfile.mkstemp("qlocale.qdoc", dir=qtsrcdir)
    qlocaleqdoc_temp_file = os.fdopen(qlocaleqdoc_temp_file, "w")
    qlocaleqdoc_file = open(qtsrcdir + "/src/corelib/tools/qlocale.qdoc", "r")
    s = qlocaleqdoc_file.readline()
    DOCSTRING="    QLocale's data is based on Common Locale Data Repository "
    while s:
        if DOCSTRING in s:
            qlocaleqdoc_temp_file.write(DOCSTRING + "v" + cldr_version + ".\n")
        else:
            qlocaleqdoc_temp_file.write(s)
        s = qlocaleqdoc_file.readline()
    qlocaleqdoc_temp_file.close()
    qlocaleqdoc_file.close()

    os.rename(qlocaleqdoc_temp_file_path, qtsrcdir + "/src/corelib/tools/qlocale.qdoc")

if __name__ == "__main__":
    main()
