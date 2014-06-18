QT       -= gui

TARGET = comparator
TEMPLATE = lib

DEFINES += COMPARATOR_LIBRARY

INCLUDEPATH += $$PWD/../

SOURCES += \
    kdtreewrapper.cc

HEADERS +=\
        comparator_global.hh \
    kdtreewrapper.hh

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix|win32: LIBS += -lkdtree

QMAKE_CXXFLAGS_RELEASE -= -O -01 -O2
QMAKE_CXXFLAGS_RELEASE *= -O3
