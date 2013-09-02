
HEADERS += $$PWD/mprisadapter.h \
	   $$PWD/mpriscontroller.h \
	   $$PWD/mprisplugin.h

SOURCES += $$PWD/mprisadapter.cpp \
	   $$PWD/mpriscontroller.cpp \
	   $$PWD/mprisplugin.cpp

QT += dbus

DEFINES += HAVE_DBUS
