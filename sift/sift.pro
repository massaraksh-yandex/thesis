QT       -= gui

TARGET = sift
TEMPLATE = lib

QMAKE_CXXFLAGS += -Wno-sign-compare -std=c++0x

SOURCES += \
    main.cpp \
    sift.cc \
    siftdata.cc \
    sift_math.cc

HEADERS += \
    keypoint.hh \
    sift.hh \
    siftdata.hh \
    sift_math.hh \
    sift_global.hh
unix {
    target.path = /usr/lib
    INSTALLS += target
}

QMAKE_CXXFLAGS_RELEASE -= -O -01 -O2
QMAKE_CXXFLAGS_RELEASE *= -O3
