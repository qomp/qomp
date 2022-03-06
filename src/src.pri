INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += $$PWD/main.cpp \
    $$PWD/qompcon.cpp \
    $$PWD/qompplaylistmodel.cpp \
    $$PWD/updateschecker.cpp \
    $$PWD/qomptunedownloader.cpp \
    $$PWD/translator.cpp \
    $$PWD/qomptaglibmetadataresolver.cpp \
    $$PWD/qompcommandline.cpp


HEADERS  += $$PWD/qompmainwin.h \
    $$PWD/qompcon.h \
    $$PWD/qompplaylistmodel.h \   
    $$PWD/updateschecker.h \
    $$PWD/qomptunedownloader.h \
    $$PWD/translator.h \
    $$PWD/qomptaglibmetadataresolver.h \
    $$PWD/qompcommandline.h

RESOURCES+= $$PWD/qomp.qrc

android {
    SOURCES     +=  $$PWD/qompmainwin_mobile.cpp

    HEADERS     +=

    RESOURCES   += $$PWD/qml.qrc
}
else {
    SOURCES += $$PWD/qomptrayicon.cpp \
        $$PWD/qompmainwin.cpp \
        $$PWD/aboutdlg.cpp \
        $$PWD/qompplaylistdelegate.cpp \
        $$PWD/qompplaylistview.cpp \
        $$PWD/qompmenu.cpp \
        $$PWD/qompinstancewatcher.cpp

    HEADERS += $$PWD/qomptrayicon.h \
        $$PWD/aboutdlg.h \
        $$PWD/qompplaylistdelegate.h \
        $$PWD/qompplaylistview.h \
        $$PWD/qompmenu.h \
        $$PWD/qompinstancewatcher.h

    FORMS   += \
        $$PWD/qompmainwin.ui \
        $$PWD/aboutdlg.ui

    RESOURCES   += $$PWD/themes.qrc

    include ($$PWD/singleapplication/singleapplication.pri)
    include($$PWD/../thirdparty/QHotkey/qhotkey.pri)
}

qtmultimedia_backend {
    lessThan(QT_MAJOR_VERSION, 6) {
        HEADERS += $$PWD/qompqtmultimediaplayer.h #\
           # $$PWD/qompqtmultimediametadataresolver.h

        SOURCES += $$PWD/qompqtmultimediaplayer.cpp #\
           # $$PWD/qompqtmultimediametadataresolver.cpp
    }
    else {
        HEADERS += $$PWD/qompqt6multimediaplayer.h

        SOURCES += $$PWD/qompqt6multimediaplayer.cpp
    }
}

win32: lessThan(QT_MAJOR_VERSION, 6) {
    SOURCES += $$PWD/qompthumbnailtoolbar.cpp

    HEADERS += $$PWD/qompthumbnailtoolbar.h
}

mac {
    HEADERS += $$PWD/CocoaTrayClick.h

    SOURCES += $$PWD/CocoaTrayClick.cpp
}
lupdate_only {
    SOURCES += $$PWD/qml/*.qml
}
include (options/options.pri)

DEFINES += QAPPLICATION_CLASS=QApplication
