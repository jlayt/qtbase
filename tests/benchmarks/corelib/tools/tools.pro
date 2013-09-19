TEMPLATE = subdirs
SUBDIRS = \
        containers-associative \
        containers-sequential \
        qbytearray \
        qcontiguouscache \
        qdatetime \
        qlist \
        qlocale \
        qmap \
        qrect \
        qregexp \
        qstring \
        qstringbuilder \
        qstringlist \
        qtimezone \
        qvector \
        qalgorithms

!*g++*: SUBDIRS -= qstring
