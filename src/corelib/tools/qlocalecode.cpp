/****************************************************************************
**
** Copyright (C) 2016 John Layt jlayt@kde.org
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlocalecode.h"
#include "qlocalecodeprivate_p.h"

#include <QtCore/qdatastream.h>

#include <qdebug.h>

QT_BEGIN_NAMESPACE

// Create default locale code using appropriate backend
static QLocaleCodePrivate *newBackendLocaleCode()
{
    return new QLocaleCodePrivate();
}

// Create named locale code using appropriate backend
static QLocaleCodePrivate *newBackendLocaleCode(const QString &locale)
{
    return new QLocaleCodePrivate(locale);
}

// Create named locale code using appropriate backend
static QLocaleCodePrivate *newBackendLocaleCode(QLocale::Language language, QLocale::Script script, QLocale::Country region)
{
    return new QLocaleCodePrivate(language, script, region);
}

class QLocaleCodeSingleton
{
public:
    QLocaleCodeSingleton() : backend(newBackendLocaleCode()) {}

    QSharedDataPointer<QLocaleCodePrivate> backend;
};

// App default locale code, used for all app localization, able to be changed by app
Q_GLOBAL_STATIC(QLocaleCodeSingleton, global_lc);

// System locale code, used for available locales, etc, never changed by app
Q_GLOBAL_STATIC(QLocaleCodeSingleton, global_system_lc);

static QLocaleCodePrivate *defaultLocaleCode()
{
    if (global_lc)
        return global_lc->backend;
    return global_system_lc->backend;
}

QLocaleCode::QLocaleCode()
{
    d = defaultLocaleCode();
}

QLocaleCode::QLocaleCode(const QString &locale)
           : d(newBackendLocaleCode(locale))
{
}

QLocaleCode::QLocaleCode(QLocale::Language language, QLocale::Country region)
           : d(newBackendLocaleCode(language, QLocale::AnyScript, region))
{
}

QLocaleCode::QLocaleCode(QLocale::Language language, QLocale::Script script, QLocale::Country region)
           : d(newBackendLocaleCode(language, script, region))
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

bool QLocaleCode::operator==(const QLocaleCode &other) const
{
    if (d && other.d)
        return (*d == *other.d);
    else
        return (d == other.d);
}

bool QLocaleCode::operator!=(const QLocaleCode &other) const
{
    if (d && other.d)
        return (*d != *other.d);
    else
        return (d != other.d);
}

bool QLocaleCode::isValid() const
{
    if (d)
        return d->isValid();
    else
        return false;
}

QString QLocaleCode::locale() const
{
    if (isValid())
        return d->locale();
    else
        return QString();
}

QString QLocaleCode::posixLocale() const
{
    if (isValid())
        return d->posixLocale();
    else
        return QString();
}

QString QLocaleCode::bcp47Locale() const
{
    if (isValid())
        return d->bcp47Locale();
    else
        return QString();
}

QString QLocaleCode::localeDisplayName(const QLocaleCode &inLocale) const
{
    if (isValid())
        return d->localeDisplayName(inLocale);
    else
        return QString();
}

QLocale::Language QLocaleCode::language() const
{
    if (isValid())
        return d->language();
    else
        return QLocale::AnyLanguage;
}

QString QLocaleCode::languageCode() const
{
    if (isValid())
        return d->languageCode();
    else
        return QString();
}

QString QLocaleCode::languageDisplayName(const QLocaleCode &inLocale) const
{
    if (isValid())
        return d->languageDisplayName(inLocale);
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

QString QLocaleCode::scriptCode() const
{
    if (isValid())
        return d->scriptCode();
    else
        return QString();
}

QString QLocaleCode::scriptDisplayName(const QLocaleCode &inLocale) const
{
    if (isValid())
        return d->scriptDisplayName(inLocale);
    else
        return QString();
}

QLocale::Country QLocaleCode::region() const
{
    if (isValid())
        return d->region();
    else
        return QLocale::AnyCountry;
}

QString QLocaleCode::regionCode() const
{
    if (isValid())
        return d->regionCode();
    else
        return QString();
}

QString QLocaleCode::regionDisplayName(const QLocaleCode &inLocale) const
{
    if (isValid())
        return d->regionDisplayName(inLocale);
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

QString QLocaleCode::variantDisplayName(const QLocaleCode &inLocale) const
{
    if (isValid())
        return d->variantDisplayName(inLocale);
    else
        return QString();
}

QString QLocaleCode::keywordValue(const QString &keyword) const
{
    if (isValid())
        return d->keywordValue(keyword);
    else
        return QString();
}

QString QLocaleCode::keywordDisplayName(const QString &keyword, const QLocaleCode &inLocale) const
{
    if (isValid())
        return d->keywordDisplayName(keyword, inLocale);
    else
        return QString();
}

QString QLocaleCode::keywordValueDisplayName(const QString &keyword, const QString &value, const QLocaleCode &inLocale) const
{
    if (isValid())
        return d->keywordValueDisplayName(keyword, value, inLocale);
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

QLocaleCode QLocaleCode::defaultLocale()
{
    return QLocaleCode(*defaultLocaleCode());
}

void QLocaleCode::setDefaultLocale(const QLocaleCode &locale)
{
    global_lc->backend = locale.d;
}

QLocaleCode QLocaleCode::system()
{
    return QLocaleCode(*global_system_lc->backend);
}

QLocaleCode QLocaleCode::c()
{
    return QLocaleCode(*new QLocaleCodePrivate());
}

QList<QLocaleCode> QLocaleCode::availableLocaleCodes()
{
    return global_system_lc->backend->availableLocaleCodes();
}

QList<QLocaleCode> QLocaleCode::matchingLocaleCodes(QLocale::Language language, QLocale::Script script, QLocale::Country country)
{
    return global_system_lc->backend->matchingLocaleCodes(language, script, country);
}

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &ds, const QLocaleCode &lc)
{
    lc.bcp47Locale();
    return ds;
}

QDataStream &operator>>(QDataStream &ds, QLocaleCode &lc)
{
    QString locale;
    ds >> locale;
    lc = QLocaleCode(locale);
    return ds;
}
#endif // QT_NO_DATASTREAM

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QLocaleCode &lc)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "QLocaleCode(" << lc.bcp47Locale() << ')';
    return dbg;
}
#endif
