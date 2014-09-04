QT       -= gui

TARGET = comparator-kdtree
TEMPLATE = lib

DEFINES += COMPARATOR_LIBRARY
QMAKE_CXXFLAGS += -std=c++0x
INCLUDEPATH += ../core/include/ /usr/local/include /usr/include/c++/4.8/
QMAKE_CXXFLAGS_RELEASE -= -O -01 -O2
QMAKE_CXXFLAGS_RELEASE *= -O3

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
