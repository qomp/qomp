include($$PWD/../plugins.pri)

HEADERS += $$PWD/yandexmusicplugin.h \
    $$PWD/yandexmusicgettunsdlg.h \
    $$PWD/yandexmusicurlresolvestrategy.h \
    $$PWD/yandexmusiccontroller.h

SOURCES += $$PWD/yandexmusicplugin.cpp \
    $$PWD/yandexmusicurlresolvestrategy.cpp \
    $$PWD/yandexmusiccontroller.cpp


OTHER_FILES += $$PWD/metadata.json

android {
    RESOURCES += $$PWD/qml.qrc
    SOURCES += $$PWD/yandexmusicgettunsdlg_mobile.cpp
}
else {
    SOURCES += $$PWD/yandexmusicgettunsdlg.cpp
}

lupdate_only {
    SOURCES += $$PWD/metadata.json
}
