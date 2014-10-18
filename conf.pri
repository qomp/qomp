CONFIG -= debug_and_release debug_and_release_target

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


unix:!mac:!android:DEFINES += HAVE_X11
#mac {
#    QMAKE_MAC_SDK = macosx10.8
#    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.8
#}

CONFIG(debug, debug|release) {
    DEFINES += DEBUG_OUTPUT
}

greaterThan(QT_MAJOR_VERSION, 4) {
    !android:  QT += widgets

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

android {
    QT += quick qml androidextras
    DEFINES += QOMP_MOBILE
}
