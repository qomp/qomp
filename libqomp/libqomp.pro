include($$PWD/../conf.pri)

QT += network

TARGET = qomp
TEMPLATE = lib

DESTDIR = $$PWD/../bin

DEFINES += LIBQOMP_LIBRARY

unix:!android {
    target.path = $$PREFIX/lib
    INSTALLS += target
}

include(src/src.pri)

