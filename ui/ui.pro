QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ui
TEMPLATE = app

QMAKE_CXXFLAGS += -Wno-sign-compare -std=c++0x

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

