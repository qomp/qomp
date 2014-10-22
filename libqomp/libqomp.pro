include($$PWD/../conf.pri)

QT += network

android {
    TARGET = qomp-shared
} else {
    TARGET = qomp
}
TEMPLATE = lib

DESTDIR = $$OUT_PWD/../bin

DEFINES += LIBQOMP_LIBRARY

unix:!android {
    contains(QMAKE_HOST.arch, x86_64): LIB_SUFFIX = 64
    target.path = $$PREFIX/lib$$LIB_SUFFIX
    INSTALLS += target
}

include(src/src.pri)

DDIR = $$system_path($$PWD/include)

!exists($$DDIR) {
    system( $$QMAKE_MKDIR $$shell_path($$DDIR) )
}

for(FILE, HEADERS) {
    QMAKE_POST_LINK += $$QMAKE_COPY $$shell_path($$FILE) $$shell_path($$DDIR) $$escape_expand(\\n\\t)
}

#LIBS += -ltag


