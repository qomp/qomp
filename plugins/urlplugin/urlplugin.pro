include($$PWD/../plugins.pri)

HEADERS += $$PWD/urlplugin.h
SOURCES += $$PWD/urlplugin.cpp

OTHER_FILES += $$PWD/metadata.json

android {
    RESOURCES += qml.qrc
}

lupdate_only {
    SOURCES += $$PWD/metadata.json
}
