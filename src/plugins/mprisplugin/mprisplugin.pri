
HEADERS += $$PWD/mpris.h \
    $$PWD/mprisplugin.h

SOURCES += $$PWD/mpris.cpp \
    $$PWD/mprisplugin.cpp

QT += dbus

DEFINES += HAVE_DBUS
