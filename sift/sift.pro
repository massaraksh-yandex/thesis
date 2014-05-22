QT       += testlib core

QT       -= gui

TARGET = sift
TEMPLATE = lib

QMAKE_CXXFLAGS += -std=c++0x

DEFINES += SIFT_LIBRARY

SOURCES += sift.cc \
    sift_math.cc \
    siftdata.cc \
    keypoint.cc

HEADERS += sift.hh\
        sift_global.hh \
    sift_math.hh \
    global.hh \
    siftdata.hh \
    keypoint.hh

unix {
    target.path = /usr/lib
    INSTALLS += target
}
