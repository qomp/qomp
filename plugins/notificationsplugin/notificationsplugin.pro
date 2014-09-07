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
}
else: unix {
    QT += dbus
}
