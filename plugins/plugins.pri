include($$PWD/../conf.pri)

CONFIG += plugin

DESTDIR = $$PWD/../bin/plugins

QT += network

TEMPLATE = lib

INCLUDEPATH += $$PWD/../libqomp/include
DEPENDPATH += $$PWD/../libqomp/include

LIBS += -L$$PWD/../bin -lqomp

unix:!android {
    target.path = $$DATADIR/plugins
    INSTALLS += target
}

