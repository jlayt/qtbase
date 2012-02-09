#!/usr/bin/env python
#############################################################################
##
## Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
## Contact: http://www.qt-project.org/
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
##
## $QT_END_LICENSE$
##
#############################################################################

import re

def _convert_pattern(pattern):
    # patterns from http://www.unicode.org/reports/tr35/#Date_Format_Patterns
    qt_regexps = {
        r"L" : "M",          # stand-alone month names. not supported.
        r"g{1,}": "",        # modified julian day. not supported.
        r"A{1,}" : ""        # milliseconds in day. not supported.
    }
    qt_patterns = {
        "G" : "", "GG" : "", "GGG" : "", "GGGG" : "", "GGGGG" : "", # Era. not supported.
        "Q" : "", "QQ" : "", "QQQ" : "", "QQQQ" : "", # quarter. not supported.
        "q" : "", "qq" : "", "qqq" : "", "qqqq" : "", # quarter. not supported.
        "LLLLL" : "MMMMM", # stand-alone narrow month name.
        "l" : "", # special symbol for chinese leap month. not supported.
        "w" : "", "W" : "", # week of year/month. not supported.
        "D" : "", "DD" : "", "DDD" : "", # day of year. not supported.
        "F" : "", # day of week in month. not supported.
        "e" : "E", "ee" : "EE", "eee" : "EEE", "eeeee" : "EEEEE", "eeee" : "EEEE", # local day of week
        "c" : "E", "cc" : "EE", "ccc" : "EEE", "ccccc" : "EEEEE", "cccc" : "EEEE", # stand-alone local day of week
        "K" : "h", # Hour 0-11
        "k" : "H", # Hour 1-24
        "j" : "", # special reserved symbol.
        "Z" : "z", "ZZ" : "zz", "ZZZ" : "zzz", "ZZZZ" : "zzzz", # timezone
        "v" : "z", "vv" : "zz", "vvv" : "zzz", "vvvv" : "zzzz", # timezone
        "V" : "z", "VV" : "zz", "VVV" : "zzz", "VVVV" : "zzzz"  # timezone
    }
    if qt_patterns.has_key(pattern):
        return qt_patterns[pattern]
    for r,v in qt_regexps.items():
        pattern = re.sub(r, v, pattern)
    return pattern

def convert_date(input):
    result = ""
    patterns = "GuQqLlwWDFgecKkjAZvV"
    last = ""
    inquote = 0
    chars_to_strip = " -"
    for c in input:
        if c == "'":
            inquote = inquote + 1
        if inquote % 2 == 0:
            if c in patterns:
                if not last:
                    last = c
                else:
                    if c in last:
                        last += c
                    else:
                        # pattern changed
                        converted = _convert_pattern(last)
                        result += converted
                        if not converted:
                            result = result.rstrip(chars_to_strip)
                        last = c
                continue
        if last:
            # pattern ended
            converted = _convert_pattern(last)
            result += converted
            if not converted:
                result = result.rstrip(chars_to_strip)
            last = ""
        result += c
    if last:
        converted = _convert_pattern(last)
        result += converted
        if not converted:
            result = result.rstrip(chars_to_strip)
    return result.lstrip(chars_to_strip)
