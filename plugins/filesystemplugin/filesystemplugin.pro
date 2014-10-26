include($$PWD/../plugins.pri)

SOURCES += $$PWD/filesystemplugin.cpp
HEADERS  += $$PWD/filesystemplugin.h

OTHER_FILES += $$PWD/metadata.json

mac {
    QMAKE_LFLAGS += -framework tag
}
else {
    LIBS += -ltag
}
