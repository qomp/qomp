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

win32|android {
    #libtag staff
    isEmpty(TAG_PATH) {
        TAG_PATH = $$[QT_INSTALL_PREFIX]
    }

    TAG_INCLUDE = $$shell_path($$TAG_PATH/include)
    TAG_LIB     = $$shell_path($$TAG_PATH/lib)

    #libcue staff
    isEmpty(CUE_PATH) {
        CUE_PATH = $$[QT_INSTALL_PREFIX]
    }

    CUE_INCLUDE = $$CUE_PATH/include
    CUE_LIB     = $$CUE_PATH/lib

    LIBS += -L$$CUE_LIB -L$$TAG_LIB
    INCLUDEPATH += $$CUE_INCLUDE $$TAG_INCLUDE
}

linux:!mac: INCLUDEPATH += /usr/include/libcue-1.4/libcue

