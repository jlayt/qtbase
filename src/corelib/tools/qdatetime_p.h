/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDATETIME_P_H
#define QDATETIME_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qplatformdefs.h"
#include "qdatetime.h"

//TODO Remove these once no longer needed in QDateTimeParser and QAbstractSpinBox
#define QDATETIME_TIME_MIN QTime(0, 0, 0, 0)
#define QDATETIME_TIME_MAX QTime(23, 59, 59, 999)
#define QDATETIME_DATE_MIN QDate(100, 1, 1)
#define QDATETIME_DATE_MAX QDate(7999, 12, 31)
#define QDATETIME_DATETIME_MIN QDateTime(QDATETIME_DATE_MIN, QDATETIME_TIME_MIN)
#define QDATETIME_DATETIME_MAX QDateTime(QDATETIME_DATE_MAX, QDATETIME_TIME_MAX)

QT_BEGIN_NAMESPACE

class QDateTimePrivate : public QSharedData
{
public:
    enum Spec { LocalUnknown = -1, LocalStandard = 0, LocalDST = 1, UTC = 2, OffsetFromUTC = 3};

    QDateTimePrivate() : spec(LocalUnknown), utcOffset(0) {}
    QDateTimePrivate(const QDateTimePrivate &other)
        : QSharedData(other), date(other.date), time(other.time), spec(other.spec), utcOffset(other.utcOffset)
    {}

    QDate date;
    QTime time;
    Spec spec;
    /*!
      \internal
      The offset in seconds. Applies only when timeSpec() is OffsetFromUTC.
     */
    int utcOffset;

    Spec getLocal(QDate &outDate, QTime &outTime) const;
    void getUTC(QDate &outDate, QTime &outTime) const;
    static QDateTime addMSecs(const QDateTime &dt, qint64 msecs);
    static void addMSecs(QDate &utcDate, QTime &utcTime, qint64 msecs);
};

#ifndef QT_BOOTSTRAPPED

#ifdef Q_OS_SYMBIAN
QDateTime qt_symbian_TTime_To_QDateTime(const TTime& time);
TTime qt_symbian_QDateTime_To_TTime(const QDateTime& datetime);
time_t qt_symbian_TTime_To_time_t(const TTime& time);
TTime qt_symbian_time_t_To_TTime(time_t time);
#endif //Q_OS_SYMBIAN

#endif // QT_BOOTSTRAPPED

QT_END_NAMESPACE

#endif // QDATETIME_P_H
