INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
SOURCES += $$PWD/main.cpp\
    $$PWD/qompmainwin.cpp \
    $$PWD/qompcon.cpp \
    $$PWD/qompplaylistmodel.cpp \
    $$PWD/qomptrayicon.cpp \
    $$PWD/aboutdlg.cpp \
    $$PWD/qomptunedownloader.cpp \
    $$PWD/qompplaylistdelegate.cpp \
    $$PWD/translator.cpp \
    $$PWD/updateschecker.cpp \
    $$PWD/qompplaylistview.cpp \
    $$PWD/qompmenu.cpp \
    $$PWD/thememanager.cpp \
    $$PWD/qomptaglibmetadataresolver.cpp

HEADERS  += $$PWD/qompmainwin.h \
    $$PWD/qompcon.h \
    $$PWD/qompplaylistmodel.h \
    $$PWD/qomptrayicon.h \
    $$PWD/aboutdlg.h \
    $$PWD/qomptunedownloader.h \
    $$PWD/qompplaylistdelegate.h \
    $$PWD/translator.h \
    $$PWD/updateschecker.h \
    $$PWD/qompplaylistview.h \
    $$PWD/qompmenu.h \
    $$PWD/thememanager.h \
    $$PWD/qomptaglibmetadataresolver.h

FORMS    += $$PWD/qompmainwin.ui \
    $$PWD/aboutdlg.ui

phonon_backend {
    HEADERS += $$PWD/qompphononplayer.h #\
    #$$PWD/qompphononmetadataresolver.h

    SOURCES += $$PWD/qompphononplayer.cpp #\
    #$$PWD/qompphononmetadataresolver.cpp
}

qtmultimedia_backend {
    HEADERS += $$PWD/qompqtmultimediaplayer.h #\
       # $$PWD/qompqtmultimediametadataresolver.h

    SOURCES += $$PWD/qompqtmultimediaplayer.cpp #\
       # $$PWD/qompqtmultimediametadataresolver.cpp
}

include (options/options.pri)

RESOURCES += $$PWD/qomp.qrc \
    $$PWD/themes.qrc
