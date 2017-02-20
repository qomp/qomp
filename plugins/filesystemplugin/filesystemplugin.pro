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

    LIBS += -L$$CUE_LIB
    INCLUDEPATH += $$CUE_INCLUDE
}
else {
    LIBS += -ltag
}

win32|android {
    LIBS += -L$$CUE_LIB -L$$TAG_LIB
    INCLUDEPATH += $$CUE_INCLUDE $$TAG_INCLUDE
}

QT += multimedia

LIBS += -lcue

linux:!mac: INCLUDEPATH += /usr/include/libcue-1.4/libcue

