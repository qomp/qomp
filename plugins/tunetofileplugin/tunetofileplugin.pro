include($$PWD/../plugins.pri)

HEADERS += $$PWD/tune2fileplugin.h \
    tunetofilesettings.h

SOURCES += $$PWD/tune2fileplugin.cpp \
    tunetofilesettings.cpp

FORMS += $$PWD/tunetofilesettings.ui

OTHER_FILES += \
   $$PWD/metadata.json
