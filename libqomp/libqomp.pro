QT += network

TARGET = $$qtLibraryTarget(qomp)
TEMPLATE = lib

DESTDIR = $$PWD/../bin

include($$PWD/../conf.pri)

DEFINES += LIBQOMP_LIBRARY

unix:!android {
    target.path = $$PREFIX/lib
    INSTALLS += target
}

include(src/src.pri)

