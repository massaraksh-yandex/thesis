
QT       -= gui
QT       += concurrent

TARGET = core
TEMPLATE = lib
CONFIG += staticlib
DEPENDPATH += . ../sift
INCLUDEPATH += ../sift

QMAKE_CXXFLAGS += -Wno-sign-compare -std=c++0x

SOURCES += core.cc \
    functionsfortest.cc

HEADERS += core.hh \
    global_core.hh \
    functionsfortest.hh \
    logging.hh
unix {
    target.path = /usr/lib
    INSTALLS += target
}

QMAKE_CXXFLAGS_RELEASE -= -O -01 -O2
QMAKE_CXXFLAGS_RELEASE *= -O3

LIBS += -lkdtree

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../comparator/release/ -lcomparator
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../comparator/debug/ -lcomparator
else:unix: LIBS += -L$$OUT_PWD/../comparator/ -lcomparator

INCLUDEPATH += $$PWD/../comparator
DEPENDPATH += $$PWD/../comparator
