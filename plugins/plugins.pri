include($$PWD/../conf.pri)

DESTDIR = $$PWD/../bin/plugins

QT += network

TEMPLATE = lib

INCLUDEPATH += $$PWD/../libqomp/include
DEPENDPATH += $$PWD/../libqomp/include

CONFIG(debug, debug|release) {
    win32: LIBS += -L$$PWD/../bin -lqompd
    unix: win32: LIBS += -L$$PWD/../bin -lqomp
} else {
    LIBS += -L$$PWD/../bin -lqomp
}

