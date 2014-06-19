QT       -= gui

TARGET = algorithm-sift
TEMPLATE = lib

DEFINES += SIFT_LIBRARY

QMAKE_CXXFLAGS += -std=c++0x

SOURCES += \
    sift.cc \
    siftdata.cc \
    sift_math.cc

HEADERS += \
    sift.hh \
    siftdata.hh \
    sift_math.hh \
    sift_global.hh \
    siftkeypoint.hh
unix {
    target.path = /usr/lib
    INSTALLS += target
}

QMAKE_CXXFLAGS_RELEASE -= -O -01 -O2
QMAKE_CXXFLAGS_RELEASE *= -O3
