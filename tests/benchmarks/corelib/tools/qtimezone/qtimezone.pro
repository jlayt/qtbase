TARGET = tst_bench_qtimezone
QT = core-private testlib

SOURCES += main.cpp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
contains(QT_CONFIG,icu) {
    DEFINES += QT_USE_ICU
}