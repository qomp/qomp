include($$PWD/../conf.pri)

QT += network concurrent

android {
    TARGET = qomp-shared
}
else {
    TARGET = qomp
    DESTDIR = $$OUT_PWD/../bin
}
TEMPLATE = lib

DEFINES += LIBQOMP_LIBRARY

unix:!android {
    target.path = $$LIBSDIR
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

win32|android {
    LIBS += -L$$TAG_LIB
    INCLUDEPATH += $$TAG_INCLUDE
}

mac {
    QMAKE_LFLAGS += -framework tag
}
else {
    LIBS += -ltag
}
