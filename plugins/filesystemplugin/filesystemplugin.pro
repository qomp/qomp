include($$PWD/../plugins.pri)

SOURCES += $$PWD/filesystemplugin.cpp
HEADERS  += $$PWD/filesystemplugin.h

LIBS += -ltag

OTHER_FILES += \
    $$PWD/metadata.json
