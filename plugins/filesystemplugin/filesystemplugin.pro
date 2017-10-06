include($$PWD/../plugins.pri)

SOURCES += $$PWD/filesystemplugin.cpp \
    $$PWD/cueparser.cpp
HEADERS  += $$PWD/filesystemplugin.h \
    $$PWD/cueparser.h

OTHER_FILES += $$PWD/metadata.json

win32|android|mac {
    LIBS += -L$$CUE_LIB
    INCLUDEPATH += $$CUE_INCLUDE
}

QT += multimedia

LIBS += -lcue
