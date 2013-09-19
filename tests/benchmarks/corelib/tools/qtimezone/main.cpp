/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#include <QDateTime>
#include <QTimeZone>
#include <private/qtimezoneprivate_p.h>
#include <QTest>
#include <qdebug.h>

class tst_QTimeZone : public QObject
{
    Q_OBJECT

    enum {
        SECS_PER_DAY = 86400,
        MSECS_PER_DAY = 86400000,
        JULIAN_DAY_1950 = 2433283,
        JULIAN_DAY_1960 = 2436935,
        JULIAN_DAY_2010 = 2455198,
        JULIAN_DAY_2011 = 2455563,
        JULIAN_DAY_2020 = 2458850,
        JULIAN_DAY_2050 = 2469808,
        JULIAN_DAY_2060 = 2473460
    };

private Q_SLOTS:
    void createUtcId();
    void createUtcIdPrivate();
    void createUtcOffsetPrivate();
    void availableUtcPrivate();
#ifdef QT_USE_ICU
    void createIcuIdPrivate();
    void dataIcuPrivate();
    void nextIcuPrivate();
    void prevIcuPrivate();
    void systemIcuPrivate();
    void availableIcuPrivate();
#endif // QT_USE_ICU
#if defined Q_OS_UNIX && !defined Q_OS_MAC
    void createTzIdPrivate();
    void dataTzPrivate();
    void nextTzPrivate();
    void prevTzPrivate();
    void systemTzPrivate();
    void availableTzPrivate();
#endif // Q_OS_UNIX
#ifdef Q_OS_MAC
    void createMacIdPrivate();
    void dataMacPrivate();
    void nextMacPrivate();
    void prevMacPrivate();
    void systemMacPrivate();
    void availableMacPrivate();
#endif // Q_OS_MAC
#ifdef Q_OS_WIN
    void createWinIdPrivate();
    void dataWinPrivate();
    void nextWinPrivate();
    void prevWinPrivate();
    void systemWinPrivate();
    void availableWinPrivate();
#endif // Q_OS_WIN;
};

void tst_QTimeZone::createUtcId()
{
    QBENCHMARK {
        QTimeZone tz("UTC+01:00");
        Q_UNUSED(tz)
    }
}

void tst_QTimeZone::createUtcIdPrivate()
{
    QBENCHMARK {
        QUtcTimeZonePrivate tzp("UTC+01:00");
        Q_UNUSED(tzp)
    }
}

void tst_QTimeZone::createUtcOffsetPrivate()
{
    QBENCHMARK {
        QUtcTimeZonePrivate tzp("UTC+01:00");
        Q_UNUSED(tzp)
    }
}

void tst_QTimeZone::availableUtcPrivate()
{
    QUtcTimeZonePrivate tzp("UTC+01:00");
    QBENCHMARK {
        tzp.availableTimeZoneIds();
    }
}

#ifdef QT_USE_ICU
void tst_QTimeZone::createIcuIdPrivate()
{
    QBENCHMARK {
        QIcuTimeZonePrivate tzp("Europe/Oslo");
        Q_UNUSED(tzp)
    }
}

void tst_QTimeZone::dataIcuPrivate()
{
    qint64 msecs = QDateTime(QDate(2015, 1, 1), QTime(0, 0, 0)).toMSecsSinceEpoch();
    QIcuTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.data(msecs);
    }
}

void tst_QTimeZone::nextIcuPrivate()
{
    qint64 msecs = QDateTime(QDate(2015, 1, 1), QTime(0, 0, 0)).toMSecsSinceEpoch();
    QIcuTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.nextTransition(msecs);
    }
}

void tst_QTimeZone::prevIcuPrivate()
{
    qint64 msecs = QDateTime(QDate(2015, 1, 1), QTime(0, 0, 0)).toMSecsSinceEpoch();
    QIcuTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.previousTransition(msecs);
    }
}

void tst_QTimeZone::systemIcuPrivate()
{
    QIcuTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.systemTimeZoneId();
    }
}

void tst_QTimeZone::availableIcuPrivate()
{
    QIcuTimeZonePrivate tzp("UTC+01:00");
    QBENCHMARK {
        tzp.availableTimeZoneIds();
    }
}
#endif // QT_USE_ICU

#if defined Q_OS_UNIX && !defined Q_OS_MAC
void tst_QTimeZone::createTzIdPrivate()
{
    QBENCHMARK {
        QTzTimeZonePrivate tzp("Europe/Oslo");
        Q_UNUSED(tzp)
    }
}

void tst_QTimeZone::dataTzPrivate()
{
    qint64 msecs = QDateTime(QDate(2015, 1, 1), QTime(0, 0, 0)).toMSecsSinceEpoch();
    QTzTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.data(msecs);
    }
}

void tst_QTimeZone::nextTzPrivate()
{
    qint64 msecs = QDateTime(QDate(2015, 1, 1), QTime(0, 0, 0)).toMSecsSinceEpoch();
    QTzTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.nextTransition(msecs);
    }
}

void tst_QTimeZone::prevTzPrivate()
{
    qint64 msecs = QDateTime(QDate(2015, 1, 1), QTime(0, 0, 0)).toMSecsSinceEpoch();
    QTzTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.previousTransition(msecs);
    }
}

void tst_QTimeZone::systemTzPrivate()
{
    qint64 msecs = QDateTime(QDate(2015, 1, 1), QTime(0, 0, 0)).toMSecsSinceEpoch();
    QTzTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.systemTimeZoneId();
    }
}

void tst_QTimeZone::availableTzPrivate()
{
    QTzTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.availableTimeZoneIds();
    }
}
#endif // Q_OS_UNIX

#ifdef Q_OS_MAC
void tst_QTimeZone::createMacIdPrivate()
{
    QBENCHMARK {
        QMacTimeZonePrivate tzp("Europe/Oslo");
        Q_UNUSED(tzp)
    }
}

void tst_QTimeZone::dataMacPrivate()
{
    qint64 msecs = QDateTime(QDate(2015, 1, 1), QTime(0, 0, 0)).toMSecsSinceEpoch();
    QMacTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.data(msecs);
    }
}

void tst_QTimeZone::nextMacPrivate()
{
    qint64 msecs = QDateTime(QDate(2015, 1, 1), QTime(0, 0, 0)).toMSecsSinceEpoch();
    QMacTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.nextTransition(msecs);
    }
}

void tst_QTimeZone::prevMacPrivate()
{
    qint64 msecs = QDateTime(QDate(2015, 1, 1), QTime(0, 0, 0)).toMSecsSinceEpoch();
    QMacTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.previousTransition(msecs);
    }
}

void tst_QTimeZone::systemMacPrivate()
{
    QMacTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.systemTimeZoneId();
    }
}

void tst_QTimeZone::availableMacPrivate()
{
    QMacTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.availableTimeZoneIds();
    }
}
#endif // Q_OS_MAC

#ifdef Q_OS_WIN
void tst_QTimeZone::createWinIdPrivate()
{
    QBENCHMARK {
        QWinTimeZonePrivate tzp("Europe/Oslo");
        Q_UNUSED(tzp)
    }
}

void tst_QTimeZone::dataWinPrivate()
{
    qint64 msecs = QDateTime(QDate(2015, 1, 1), QTime(0, 0, 0)).toMSecsSinceEpoch();
    QWinTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.data(msecs);
    }
}

void tst_QTimeZone::nextWinPrivate()
{
    qint64 msecs = QDateTime(QDate(2015, 1, 1), QTime(0, 0, 0)).toMSecsSinceEpoch();
    QWinTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.nextTransition(msecs);
    }
}

void tst_QTimeZone::prevWinPrivate()
{
    qint64 msecs = QDateTime(QDate(2015, 1, 1), QTime(0, 0, 0)).toMSecsSinceEpoch();
    QWinTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.previousTransition(msecs);
    }
}

void tst_QTimeZone::systemWinPrivate()
{
    QWinTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.systemTimeZoneId();
    }
}

void tst_QTimeZone::availableWinPrivate()
{
    QWinTimeZonePrivate tzp("Europe/Oslo");
    QBENCHMARK {
        tzp.availableTimeZoneIds();
    }
}
#endif // Q_OS_WIN;


QTEST_MAIN(tst_QTimeZone)

#include "main.moc"
