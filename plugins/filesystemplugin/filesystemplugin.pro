include($$PWD/../plugins.pri)

SOURCES += $$PWD/filesystemplugin.cpp
HEADERS  += $$PWD/filesystemplugin.h

mac {
    QMAKE_LFLAGS += -framework tag
}
else {
    LIBS += -ltag
}

OTHER_FILES += \
    $$PWD/metadata.json
