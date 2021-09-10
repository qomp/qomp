include($$PWD/../plugins.pri)

HEADERS += $$PWD/yandexmusicplugin.h \
    $$PWD/yandexmusicgettunsdlg.h \
    $$PWD/yandexmusicurlresolvestrategy.h \
    $$PWD/yandexmusiccontroller.h \
    $$PWD/yandexmusicoauth.h \
    $$PWD/yandexmusicsettings.h \
    $$PWD/yandexmusiccaptcha.h

SOURCES += $$PWD/yandexmusicplugin.cpp \
    $$PWD/yandexmusicurlresolvestrategy.cpp \
    $$PWD/yandexmusiccontroller.cpp \
    $$PWD/yandexmusicoauth.cpp \
    $$PWD/yandexmusicsettings.cpp \
    $$PWD/yandexmusiccaptcha.cpp


OTHER_FILES += $$PWD/metadata.json

android {
    RESOURCES += $$PWD/qml.qrc
    SOURCES += $$PWD/yandexmusicgettunsdlg_mobile.cpp
}
else {
    SOURCES += $$PWD/yandexmusicgettunsdlg.cpp
}

QT += xml

FORMS += \
    $$PWD/yandexmusicsettings.ui
