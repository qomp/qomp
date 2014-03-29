include($$PWD/../plugins.pri)

HEADERS += $$PWD/yandexmusicplugin.h \
    $$PWD/yandexmusicgettunsdlg.h \
    $$PWD/yandexmusicurlresolvestrategy.h \
    $$PWD/yandexmusiccontroller.h

SOURCES += $$PWD/yandexmusicplugin.cpp \
    $$PWD/yandexmusicgettunsdlg.cpp \
    $$PWD/yandexmusicurlresolvestrategy.cpp \
    $$PWD/yandexmusiccontroller.cpp

OTHER_FILES += \
    $$PWD/metadata.json