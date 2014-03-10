CONFIG += release
CONFIG -= debug

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
}

android: ANDROID_PACKAGE_SOURCE_DIR = $$PWD/../android
