QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ui
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../sift ../spatial/src

SOURCES += main.cc\
        mainwindow.cc \
    descriptorwidget.cc \
    comparingwidget.cc \
    messageswidget.cc \
    testingwidget.cc

HEADERS  += mainwindow.hh \
    descriptorwidget.hh \
    comparingwidget.hh \
    messageswidget.hh \
    testingwidget.hh

FORMS    += mainwindow.ui \
    descriptorwidget.ui \
    comparingwidget.ui \
    messageswidget.ui \
    testingwidget.ui

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
