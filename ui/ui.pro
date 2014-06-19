QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ui
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

LIBS += -lX11

INCLUDEPATH += ../sift ../core

SOURCES += main.cc\
        mainwindow.cc \
    descriptorwidget.cc \
    comparingwidget.cc \
    messageswidget.cc \
    testingwidget.cc \
    noisewidget.cc \
    showdiffwidget.cc

HEADERS  += mainwindow.hh \
    descriptorwidget.hh \
    comparingwidget.hh \
    messageswidget.hh \
    testingwidget.hh \
    noisewidget.hh \
    showdiffwidget.hh

FORMS    += mainwindow.ui \
    descriptorwidget.ui \
    comparingwidget.ui \
    messageswidget.ui \
    testingwidget.ui \
    noisewidget.ui

RESOURCES += \

QMAKE_CXXFLAGS_RELEASE -= -O -01 -O2
QMAKE_CXXFLAGS_RELEASE *= -O3

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lcore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lcore
else:unix: LIBS += -L$$OUT_PWD/../core/ -lcore

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/release/libcore.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/debug/libcore.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/release/core.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/debug/core.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../core/libcore.a
