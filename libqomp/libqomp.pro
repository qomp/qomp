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
    target.path = $$PREFIX/lib
    INSTALLS += target
}

include(src/src.pri)

for(FILE, HEADERS) {
        DDIR = $$PWD/include

        QMAKE_POST_LINK += $$QMAKE_COPY $$shell_path($$FILE) $$shell_path($$DDIR) $$escape_expand(\\n\\t)
 }


