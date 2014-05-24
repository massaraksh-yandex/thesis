#-------------------------------------------------
#
# Project created by QtCreator 2014-05-24T19:45:29
#
#-------------------------------------------------

QT       += testlib gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = tst_sift_testtest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x


SOURCES += tst_sift_testtest.cc \
    ../../sift/FeatureExtraction/Math.cpp \
    ../../sift/FeatureExtraction/SIFT.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

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

HEADERS += \
    ../../sift/FeatureExtraction/CImg.h \
    ../../sift/FeatureExtraction/Math.h \
    ../../sift/FeatureExtraction/SIFT.h
