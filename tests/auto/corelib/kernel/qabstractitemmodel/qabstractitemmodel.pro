load(qttest_p4)

INCLUDEPATH += $$PWD/../../../modeltest
QT += widgets
SOURCES         += tst_qabstractitemmodel.cpp ../../../modeltest/dynamictreemodel.cpp ../../../modeltest/modeltest.cpp
HEADERS         += ../../../modeltest/dynamictreemodel.h ../../../modeltest/modeltest.h



