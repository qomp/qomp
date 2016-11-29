CONFIG -= debug_and_release debug_and_release_target

MOC_DIR = .moc
OBJECTS_DIR = .obj
UI_DIR = .ui
RCC_DIR = .rcc

INCLUDEPATH += $$UI_DIR $$MOC_DIR $$RCC_DIR


isEmpty(PREFIX) {
    PREFIX = /usr/local
}

BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX/share/qomp

DEFINES += QOMP_DATADIR='\\"$$DATADIR\\"'

unix:!android {
    !mac:DEFINES += HAVE_X11
    contains(QMAKE_HOST.arch, x86_64):LIB_SUFFIX = 64
    LIBSDIR = $$PREFIX/lib$$LIB_SUFFIX
    PLUGSDIR = $$LIBSDIR/qomp/plugins
    DEFINES += QOMP_PLUGSDIR='\\"$$PLUGSDIR\\"'
}

#mac {
#    QMAKE_MAC_SDK = macosx10.8
#    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.8
#}

CONFIG(debug, debug|release) {
    DEFINES += DEBUG_OUTPUT
}

!android:  QT += widgets

DEFINES += HAVE_QT5
CONFIG += c++11

android {
    QT += quick qml androidextras
    DEFINES += QOMP_MOBILE
    CONFIG(debug, debug|release) {
        DEFINES += TEST_ANDROID
    }
}

CONFIG += warn_on

win32|android|macx {
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

    CUE_INCLUDE = $$shell_path($$CUE_PATH/include)
    CUE_LIB     = $$shell_path($$CUE_PATH/lib)
}
