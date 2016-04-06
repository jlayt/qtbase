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

#ifndef QLOCALECODEPRIVATE_P_H
#define QLOCALECODEPRIVATE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of internal files.  This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.
//

#include "qlocalecode.h"

QT_BEGIN_NAMESPACE

class Q_CORE_EXPORT QLocaleCodePrivate : public QSharedData
{
public:
    QLocaleCodePrivate();
    QLocaleCodePrivate(const QString &locale);
    QLocaleCodePrivate(QLocale::Language language, QLocale::Script script, QLocale::Country country);
    QLocaleCodePrivate(const QLocaleCodePrivate &other);
    virtual ~QLocaleCodePrivate();

    virtual QLocaleCodePrivate *clone();

    bool operator==(const QLocaleCodePrivate &other) const;
    bool operator!=(const QLocaleCodePrivate &other) const;


    virtual QString locale() const;
    virtual QString posixLocale() const;
    virtual QString bcp47Locale() const;
    virtual QString localeDisplayName(const QLocaleCode &inLocale = QLocaleCode()) const;

    virtual bool isValid() const;

    virtual QLocale::Language language() const;
    virtual QString languageCode() const;
    virtual QString languageDisplayName(const QLocaleCode &inLocale = QLocaleCode()) const;

    virtual QLocale::Script script() const;
    virtual QString scriptCode() const;
    virtual QString scriptDisplayName(const QLocaleCode &inLocale = QLocaleCode()) const;

    virtual QLocale::Country region() const;
    virtual QString regionCode() const;
    virtual QString regionDisplayName(const QLocaleCode &inLocale = QLocaleCode()) const;

    virtual QString variant() const;
    virtual QString variantDisplayName(const QLocaleCode &inLocale = QLocaleCode()) const;

    virtual QString keywordValue(const QString &keyword) const;
    virtual QString keywordDisplayName(const QString &keyword, const QLocaleCode &inLocale = QLocaleCode()) const;
    virtual QString keywordValueDisplayName(const QString &keyword, const QString &value, const QLocaleCode &inLocale = QLocaleCode()) const;

    virtual Qt::LayoutDirection characterOrientation() const;
    virtual Qt::LayoutDirection lineOrientation() const;

    virtual QList<QLocaleCode> availableLocaleCodes();
    virtual QList<QLocaleCode> matchingLocaleCodes(QLocale::Language language, QLocale::Script script, QLocale::Country country);

protected:
    QString m_locale;
    QLocale::Language m_language;
    QLocale::Script m_script;
    QLocale::Country m_region;
};

template<> QLocaleCodePrivate *QSharedDataPointer<QLocaleCodePrivate>::clone();

QT_END_NAMESPACE

#endif // QLOCALECODEPRIVATE_P_H
