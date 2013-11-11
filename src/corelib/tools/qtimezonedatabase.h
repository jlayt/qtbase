/****************************************************************************
**
** Copyright (C) 2014 John Layt <jlayt@kde.org>
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

#ifndef QTIMEZONEDATABASE_H
#define QTIMEZONEDATABASE_H

#include <QtCore/qsharedpointer.h>
#include <QtCore/qlocale.h>
#include <QtCore/qtimezone.h>

QT_BEGIN_NAMESPACE

class QTimeZoneDatabasePrivate;

class Q_CORE_EXPORT QTimeZoneDatabase
{
public:
    QTimeZoneDatabase();
    explicit QTimeZoneDatabase(const QString &databasePath);
    QTimeZoneDatabase(const QTimeZoneDatabase &other);
    ~QTimeZoneDatabase();

    QTimeZoneDatabase &operator=(const QTimeZoneDatabase &other);
 #ifdef Q_COMPILER_RVALUE_REFS
    QTimeZoneDatabase &operator=(QTimeZoneDatabase &&other) { std::swap(d, other.d); return *this; }
#endif

    bool operator==(const QTimeZoneDatabase &other) const;
    bool operator!=(const QTimeZoneDatabase &other) const;

    bool isValid() const;

    QString databasePath() const;

    QTimeZone createTimeZone(const QByteArray &ianaId);

    bool isTimeZoneIdAvailable(const QByteArray &ianaId);

    QList<QByteArray> availableTimeZoneIds() const;
    QList<QByteArray> availableTimeZoneIds(QLocale::Country country) const;

    static QString systemDatabasePath();
    static QTimeZoneDatabase systemDatabase();

private:
    friend class QTimeZoneDatabasePrivate;
    QSharedDataPointer<QTimeZoneDatabasePrivate> d;
};

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug dbg, const QTimeZoneDatabase &tz);
#endif

QT_END_NAMESPACE

#endif // QTIMEZONEDATABASE_H
