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
** Alternatively, this file may be used in accordance padding the terms and
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

#include "qlocalecode_p.h"
#include "qlocalecodeprivate_p.h"

QT_BEGIN_NAMESPACE

static QLocaleCodePrivate *default_code;
#ifndef QT_NO_SYSTEMLOCALE
static QLocaleCodePrivate *system_code;
#endif // QT_NO_SYSTEMLOCALE

#ifndef QT_NO_SYSTEMLOCALE
QLocaleCodePrivate *qt_systemLocale()
{
    if (!system_code)
        // TODO Create system backend as appropriate
        system_code = new QLocaleCodePrivate();
    return system_code;
}
#endif // QT_NO_SYSTEMLOCALE

QLocaleCodePrivate *qt_defaultLocale()
{
    if (!default_code)
#ifndef QT_NO_SYSTEMLOCALE
        return qt_systemLocale();
#else
        default_code = new QLocaleCodePrivate();
#endif // QT_NO_SYSTEMLOCALE
    return default_code;
}

QLocaleCode::QLocaleCode()
           : d(qt_defaultLocale())
{
}

QLocaleCode::QLocaleCode(const QString &locale)
           : d(new QLocaleCodePrivate(locale))
{
}

QLocaleCode::QLocaleCode(QLocaleCodePrivate &dd)
           : d(&dd)
{
}
QLocaleCode::QLocaleCode(const QLocaleCode &other)
{
    d = other.d;
}

QLocaleCode::~QLocaleCode()
{
}

QLocaleCode &QLocaleCode::operator=(const QLocaleCode &other)
{
    d = other.d;
    return *this;
}

QLocaleCode QLocaleCode::system()
{
#ifndef QT_NO_SYSTEMLOCALE
    return QLocaleCode(*qt_systemLocale());
#else
    return QLocaleCode(*qt_defaultLocale());
#endif // QT_NO_SYSTEMLOCALE
}

QString QLocaleCode::locale() const
{
    return d->locale();
}

QString QLocaleCode::localeBase() const
{
    return d->localeBase();
}

qint32 QLocaleCode::localeWindowsId() const
{
    return d->localeWindowsId();
}

QString QLocaleCode::localeName(const QString &inLocale) const
{
    return d->locale(inLocale);
}

bool QLocaleCode::isValid() const
{
    return d->isValid();
}

bool QLocaleCode::isInvalid() const
{
    return d->isInvalid();
}

QLocale::Language QLocaleCode::language() const
{
    if (isValid())
        return d->langauge();
    else
        return QLocale::AnyLanguage;
}

QString QLocaleCode::languageCode(QLocaleCode::IsoCodeFormat format) const
{
    if (isValid())
        return d->languageCode(format);
    else
        return QString();
}

QString QLocaleCode::languageName(const QString &inLocale) const
{
    if (isValid())
        return d->languageName(inLocale);
    else
        return QString();
}

QLocale::Country QLocaleCode::country() const
{
    if (isValid())
        return d->country();
    else
        return QLocale::AnyCountry;
}

QString QLocaleCode::countryCode(QLocaleCode::IsoCodeFormat format) const
{
    if (isValid())
        return d->countryCode(format);
    else
        return QString();
}

QString QLocaleCode::countryName(const QString &inLocale) const
{
    if (isValid())
        return d->countryName(inLocale);
    else
        return QString();
}

QLocale::Script QLocaleCode::script() const
{
    if (isValid())
        return d->script();
    else
        return QLocale::AnyScript;
}

QString QLocaleCode::scriptCode(QLocaleCode::IsoCodeFormat format) const
{
    if (isValid())
        return d->scriptCode(format);
    else
        return QString();
}

QString QLocaleCode::scriptName(const QString &inLocale) const
{
    if (isValid())
        return d->scriptName(inLocale);
    else
        return QString();
}

QString QLocaleCode::variant() const
{
    if (isValid())
        return d->variant();
    else
        return QString();
}

QString QLocaleCode::variantName(const QString &inLocale) const
{
    if (isValid())
        return d->variantName(inLocale);
    else
        return QString();
}

// keywords

QString QLocaleCode::currencyCode(const QDate & date, QLocaleCode::IsoCodeFormat format) const
{
    if (isValid())
        return d->currencyCode(date, format);
    else
        return QString();
}

QString QLocaleCode::currencySymbol(const QDate & date, const QString &inLocale) const
{
    if (isValid())
        return d->currencySymbol(date, inLocale);
    else
        return QString();
}

QString QLocaleCode::currencyName(const QDate & date, const QString &inLocale, quint32 pluralCount) const
{
    if (isValid())
        return d->currencyName(date, inLocale, pluralCount);
    else
        return QString();
}

Qt::LayoutDirection QLocaleCode::characterOrientation() const
{
    if (isValid())
        return d->characterOrientation();
    else
        return Qt::LeftToRight;
}

Qt::LayoutDirection QLocaleCode::lineOrientation() const
{
    if (isValid())
        return d->lineOrientation();
    else
        return Qt::LeftToRight;
}
