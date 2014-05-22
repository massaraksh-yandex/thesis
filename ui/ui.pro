QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ui
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cc\
        mainwindow.cc \
    descriptorwidget.cc \
    comparingwidget.cc \
    testingwidget.cc \
    messageswidget.cc

HEADERS  += mainwindow.hh \
    descriptorwidget.hh \
    comparingwidget.hh \
    testingwidget.hh \
    messageswidget.hh

FORMS    += mainwindow.ui \
    descriptorwidget.ui \
    comparingwidget.ui \
    testingwidget.ui \
    messageswidget.ui
