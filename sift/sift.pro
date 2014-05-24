#-------------------------------------------------
#
# Project created by QtCreator 2014-05-24T16:19:10
#
#-------------------------------------------------

QT       -= gui

TARGET = sift
TEMPLATE = lib
CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++0x


SOURCES += \
    keypoint.cc \
    main.cpp \
    sift.cc \
    siftdata.cc \
    sift_math.cc

HEADERS += \
    global.hh \
    keypoint.hh \
    sift.hh \
    siftdata.hh \
    sift_math.hh
unix {
    target.path = /usr/lib
    INSTALLS += target
}
