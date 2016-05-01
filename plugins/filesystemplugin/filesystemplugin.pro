include($$PWD/../plugins.pri)

SOURCES += $$PWD/filesystemplugin.cpp \
    $$PWD/cueparser.cpp \
    $$PWD/filesystemcommon.cpp
HEADERS  += $$PWD/filesystemplugin.h \
    $$PWD/cueparser.h \
    $$PWD/filesystemcommon.h

OTHER_FILES += $$PWD/metadata.json

mac {
    QMAKE_LFLAGS += -framework tag
}
else {
    LIBS += -ltag
}

LIBS += -lcue

linux:!mac: INCLUDEPATH += /usr/include/libcue-1.4/libcue
