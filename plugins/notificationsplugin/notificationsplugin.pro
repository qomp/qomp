include($$PWD/../plugins.pri)

HEADERS += $$PWD/notificationsplugin.h

SOURCES += $$PWD/notificationsplugin.cpp 

OTHER_FILES +=  $$PWD/metadata.json

android {
    QT += androidextras
}
else: windows {
}
else: mac {
    include($$PWD/growlnotifier/growlnotifier.pri)
    INCLUDEPATH += $$PWD/growlnotifier
    QMAKE_LFLAGS += -framework Growl
}
else: unix {
    QT += dbus
}
