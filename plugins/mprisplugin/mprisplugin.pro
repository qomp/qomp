include($$PWD/../plugins.pri)

unix:!mac:!android {
    QT += dbus
    DEFINES += HAVE_DBUS

    HEADERS += $$PWD/mprisadapter.h \
               $$PWD/mpriscontroller.h \
               $$PWD/rootadapter.h \
               $$PWD/signalhandler.h \
               $$PWD/mprisplugin.h

    SOURCES += $$PWD/mprisadapter.cpp \
               $$PWD/mpriscontroller.cpp \
               $$PWD/rootadapter.cpp \
               $$PWD/signalhandler.cpp \
               $$PWD/mprisplugin.cpp
}

OTHER_FILES += $$PWD/metadata.json

