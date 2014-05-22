
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
    noise.cc

HEADERS += core.hh \
    noise.hh
unix {
    target.path = /usr/lib
    INSTALLS += target
}
