include($$PWD/../conf.pri)

QT += network

android {
    TARGET = qomp-shared
} else {
    TARGET = qomp
}
TEMPLATE = lib

DESTDIR = $$PWD/../bin

DEFINES += LIBQOMP_LIBRARY

unix:!android {
    target.path = $$PREFIX/lib
    INSTALLS += target
}

include(src/src.pri)

#for(file, HEADERS) {
#    item = item$${file}
#    $${item}.target = $${file}
#    $${item}.commands = $(COPY_FILE) $${file} $$PWD/include
#    QMAKE_EXTRA_TARGETS += $$item
#    PRE_TARGETDEPS += $$item
#}


for(FILE, HEADERS) {
        DDIR = $$PWD/include

        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
 }


