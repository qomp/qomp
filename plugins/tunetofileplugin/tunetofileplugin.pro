include($$PWD/../plugins.pri)

HEADERS += $$PWD/tunetofileplugin.h \
    $$PWD/tunetofilesettings.h

SOURCES += $$PWD/tunetofileplugin.cpp \
    $$PWD/tunetofilesettings.cpp

FORMS += $$PWD/tunetofilesettings.ui

OTHER_FILES += $$PWD/metadata.json
