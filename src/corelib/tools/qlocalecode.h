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

#ifndef QLOCALECODE_H
#define QLOCALECODE_H

#include "qlocale.h"

QT_BEGIN_NAMESPACE

class QLocaleCodePrivate;

class Q_CORE_EXPORT QLocaleCode
{
public:
    QLocaleCode();
    QLocaleCode(const QString &locale);
    QLocaleCode(QLocale::Language language, QLocale::Country country);
    QLocaleCode(QLocale::Language language, QLocale::Script script, QLocale::Country country);
    QLocaleCode(const QLocaleCode &other);
    ~QLocaleCode();

    QLocaleCode &operator=(const QLocaleCode &other);
#ifdef Q_COMPILER_RVALUE_REFS
    QLocaleCode &operator=(QLocaleCode &&other) Q_DECL_NOTHROW { swap(other); return *this; }
#endif

    void swap(QLocaleCode &other) Q_DECL_NOTHROW { d.swap(other.d); }

    bool operator==(const QLocaleCode &other) const;
    bool operator!=(const QLocaleCode &other) const;

    bool isValid() const;

    QString locale() const;
    QString posixLocale() const;
    QString bcp47Locale() const;
    QString localeDisplayName(const QLocaleCode &inLocale = QLocaleCode()) const;

    QLocale::Language language() const;
    QString languageCode() const;
    QString languageDisplayName(const QLocaleCode &inLocale = QLocaleCode()) const;

    QLocale::Script script() const;
    QString scriptCode() const;
    QString scriptDisplayName(const QLocaleCode &inLocale = QLocaleCode()) const;

    QLocale::Country region() const;
    QString regionCode() const;
    QString regionDisplayName(const QLocaleCode &inLocale = QLocaleCode()) const;

    //Do we really need?
    QString variant() const;
    QString variantDisplayName(const QLocaleCode &inLocale = QLocaleCode()) const;

    //Do we really need?
    QString keywordValue(const QString &keyword) const;
    QString keywordDisplayName(const QString &keyword, const QLocaleCode &inLocale = QLocaleCode()) const;
    QString keywordValueDisplayName(const QString &keyword, const QString &value, const QLocaleCode &inLocale = QLocaleCode()) const;

    Qt::LayoutDirection characterOrientation() const;
    Qt::LayoutDirection lineOrientation() const;

    static QLocaleCode defaultLocale();
    static void setDefaultLocale(const QLocaleCode &locale);

    static QLocaleCode system();
    static QLocaleCode c();

    static QList<QLocaleCode> availableLocaleCodes();
    static QList<QLocaleCode> matchingLocaleCodes(QLocale::Language language, QLocale::Script script, QLocale::Country country);

private:
    // Create locale with backend
    QLocaleCode(QLocaleCodePrivate &dd);
#ifndef QT_NO_DATASTREAM
    friend Q_CORE_EXPORT QDataStream &operator<<(QDataStream &ds, const QLocaleCode &lc);
#endif
    friend class QLocaleCodePrivate;
    QSharedDataPointer<QLocaleCodePrivate> d;
};

Q_DECLARE_SHARED(QLocaleCode)

#ifndef QT_NO_DATASTREAM
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &ds, const QLocaleCode &lc);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &ds, QLocaleCode &lc);
#endif

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug dbg, const QLocaleCode &lc);
#endif

QT_END_NAMESPACE

#endif // QLOCALECODE_H
