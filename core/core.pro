
QT       -= gui
QT       += concurrent

TARGET = core
TEMPLATE = lib
CONFIG += staticlib
DEPENDPATH += . ../sift
INCLUDEPATH += ../sift ../spatial/src
LIBS += -L../build-thesis/sift -libsift

QMAKE_CXXFLAGS += -std=c++0x

SOURCES += core.cc \
    noise.cc \
    functionsfortest.cc

HEADERS += core.hh \
    noise.hh \
    global_core.hh \
    functionsfortest.hh
unix {
    target.path = /usr/lib
    INSTALLS += target
}
