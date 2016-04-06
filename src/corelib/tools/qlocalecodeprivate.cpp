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
#include "qlocale_p.h"

#include <QtCore/qdatastream.h>

#include <qdebug.h>

QT_BEGIN_NAMESPACE

QLocaleCodePrivate::QLocaleCodePrivate()
    : m_locale(QStringLiteral("C")),
      m_language(QLocale::C),
      m_script(QLocale::AnyScript),
      m_region(QLocale::AnyCountry)
{
}

QLocaleCodePrivate::QLocaleCodePrivate(const QString &locale)
    : m_locale(locale),
      m_language(QLocale::C),
      m_script(QLocale::AnyScript),
      m_region(QLocale::AnyCountry)
{
}

QLocaleCodePrivate::QLocaleCodePrivate(QLocale::Language language, QLocale::Script script, QLocale::Country region)
    : m_locale(QStringLiteral("C")),
      m_language(language),
      m_script(script),
      m_region(region)
{
}

QLocaleCodePrivate::QLocaleCodePrivate(const QLocaleCodePrivate &other)
    : QSharedData(other),
      m_locale(other.m_locale),
      m_language(other.m_language),
      m_script(other.m_script),
      m_region(other.m_region)
{
}

QLocaleCodePrivate::~QLocaleCodePrivate()
{
}

QLocaleCodePrivate *QLocaleCodePrivate::clone()
{
    return new QLocaleCodePrivate(*this);
}

bool QLocaleCodePrivate::operator==(const QLocaleCodePrivate &other) const
{
    return (m_language == other.m_language and m_script == other.m_script and m_region == other.m_region);
}

bool QLocaleCodePrivate::operator!=(const QLocaleCodePrivate &other) const
{
    return !(*this == other);
}

bool QLocaleCodePrivate::isValid() const
{
    return true;
}

QString QLocaleCodePrivate::locale() const
{
    return m_locale;
}

QString QLocaleCodePrivate::posixLocale() const
{
    return m_locale;
}

QString QLocaleCodePrivate::bcp47Locale() const
{
    return m_locale;
}

QString QLocaleCodePrivate::localeDisplayName(const QLocaleCode &inLocale) const
{
    Q_UNUSED(inLocale)
    return m_locale;
}

QLocale::Language QLocaleCodePrivate::language() const
{
    return m_language;
}

QString QLocaleCodePrivate::languageCode() const
{
    return QLocalePrivate::languageToCode(m_language);
}

QString QLocaleCodePrivate::languageDisplayName(const QLocaleCode &inLocale) const
{
    Q_UNUSED(inLocale)
    return QStringLiteral("C");
}

QLocale::Script QLocaleCodePrivate::script() const
{
    return m_script;
}

QString QLocaleCodePrivate::scriptCode() const
{
    return QLocalePrivate::scriptToCode(m_script);
}

QString QLocaleCodePrivate::scriptDisplayName(const QLocaleCode &inLocale) const
{
    Q_UNUSED(inLocale)
    return QStringLiteral("C");
}

QLocale::Country QLocaleCodePrivate::region() const
{
    return m_region;
}

QString QLocaleCodePrivate::regionCode() const
{
    return QLocalePrivate::countryToCode(m_region);
}

QString QLocaleCodePrivate::regionDisplayName(const QLocaleCode &inLocale) const
{
    Q_UNUSED(inLocale)
    return QStringLiteral("C");
}

QString QLocaleCodePrivate::variant() const
{
    return QString();
}

QString QLocaleCodePrivate::variantDisplayName(const QLocaleCode &inLocale) const
{
    Q_UNUSED(inLocale)
    return QString();
}

QString QLocaleCodePrivate::keywordValue(const QString &keyword) const
{
    Q_UNUSED(keyword)
    return QString();
}

QString QLocaleCodePrivate::keywordDisplayName(const QString &keyword, const QLocaleCode &inLocale) const
{
    Q_UNUSED(keyword)
    Q_UNUSED(inLocale)
    return QString();
}

QString QLocaleCodePrivate::keywordValueDisplayName(const QString &keyword, const QString &value, const QLocaleCode &inLocale) const
{
    Q_UNUSED(keyword)
    Q_UNUSED(value)
    Q_UNUSED(inLocale)
    return QString();
}

Qt::LayoutDirection QLocaleCodePrivate::characterOrientation() const
{
    return Qt::LeftToRight;
}

Qt::LayoutDirection QLocaleCodePrivate::lineOrientation() const
{
    return Qt::LeftToRight;
}

QList<QLocaleCode> QLocaleCodePrivate::availableLocaleCodes()
{
    QList<QLocaleCode> list;
    list.append(QLocaleCode(* new QLocaleCodePrivate()));
    return list;
}

QList<QLocaleCode> QLocaleCodePrivate::matchingLocaleCodes(QLocale::Language language, QLocale::Script script, QLocale::Country region)
{
    if (language == QLocale::C && script == QLocale::AnyScript && region == QLocale::AnyCountry)
        return availableLocaleCodes();
    return QList<QLocaleCode>();
}

// Define template for derived classes to reimplement so QSharedDataPointer clone() works correctly
template<> QLocaleCodePrivate *QSharedDataPointer<QLocaleCodePrivate>::clone()
{
    return d->clone();
}
