
QT       -= gui
QT       += concurrent

TARGET = core
TEMPLATE = lib
CONFIG += staticlib
DEPENDPATH += . ../sift
INCLUDEPATH += ../sift ../spatial/src

QMAKE_CXXFLAGS += -std=c++0x -Wno-sign-compare

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../sift/release/ -lsift
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../sift/debug/ -lsift
else:unix: LIBS += -L$$OUT_PWD/../sift/ -lsift

INCLUDEPATH += $$PWD/../sift
DEPENDPATH += $$PWD/../sift

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../sift/release/libsift.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../sift/debug/libsift.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../sift/release/sift.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../sift/debug/sift.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../sift/libsift.a

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE *= -O3
