include($$PWD/../plugins.pri)

HEADERS += $$PWD/tune2fileplugin.h \
    $$PWD/tunetofilesettings.h

SOURCES += $$PWD/tune2fileplugin.cpp \
    $$PWD/tunetofilesettings.cpp

FORMS += $$PWD/tunetofilesettings.ui

OTHER_FILES += $$PWD/metadata.json
