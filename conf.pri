CONFIG += release
CONFIG -= debug
CONFIG -= debug_and_release debug_and_release_target

unix:!mac:!android:DEFINES += HAVE_X11

debug {
    DEFINES += DEBUG_OUTPUT
}

MOC_DIR = .moc
OBJECTS_DIR = .obj
UI_DIR = .ui
RCC_DIR = .rcc

INCLUDEPATH += $$PWD/.ui

isEmpty(PREFIX) {
        PREFIX = /usr/local
}

BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX/share/qomp


DEFINES += QOMP_DATADIR='\\"$$DATADIR\\"'

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += HAVE_QT5
    CONFIG += c++11
}
else {
    defineReplace(shell_path) {
        var = $$1
        win32: var ~= s,/,\\,g
        return ($$quote($$var))
    }

    defineReplace(system_path) {
        return ($$shell_path($$1))
    }
}
