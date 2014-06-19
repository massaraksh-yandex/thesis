
QT       -= gui
QT       += concurrent

TARGET = core
TEMPLATE = lib
CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++0x

SOURCES += core.cc \
    functionsfortest.cc \
    libraryloader.cc \
    tree.cc \
    algorithm.cc

HEADERS += core.hh \
    global_core.hh \
    functionsfortest.hh \
    logging.hh \
    libraryloader.hh \
    tree.hh \
    algorithm.hh \
    imageactions.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

QMAKE_CXXFLAGS_RELEASE -= -O -01 -O2
QMAKE_CXXFLAGS_RELEASE *= -O3
