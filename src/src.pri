INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
SOURCES += $$PWD/main.cpp\
    $$PWD/qompmainwin.cpp \
    $$PWD/qomp.cpp \
    $$PWD/options.cpp \
    $$PWD/qompplayer.cpp \
    $$PWD/qompplaylistmodel.cpp \
    $$PWD/pluginmanager.cpp \
    $$PWD/tune.cpp \
    $$PWD/qomptrayicon.cpp \
    $$PWD/common.cpp \
    $$PWD/qompnetworkingfactory.cpp \
    $$PWD/aboutdlg.cpp \
    $$PWD/qomptunedownloader.cpp \
    $$PWD/qompplaylistdelegate.cpp \
    $$PWD/qompbusylabel.cpp \
    $$PWD/translator.cpp \
    $$PWD/updateschecker.cpp \
    $$PWD/qompplaylistview.cpp \
    $$PWD/qompmetadataresolver.cpp

HEADERS  += $$PWD/qompmainwin.h \
    $$PWD/qomp.h \
    $$PWD/options.h \
    $$PWD/qompplayer.h \
    $$PWD/qompplaylistmodel.h \
    $$PWD/tune.h \
    $$PWD/pluginmanager.h \
    $$PWD/qomptrayicon.h \
    $$PWD/defines.h \
    $$PWD/common.h \
    $$PWD/qompnetworkingfactory.h \
    $$PWD/qompmetadataresolver.h \
    $$PWD/aboutdlg.h \
    $$PWD/qomptunedownloader.h \
    $$PWD/qompplaylistdelegate.h \
    $$PWD/qompbusylabel.h \
    $$PWD/translator.h \
    $$PWD/updateschecker.h \
    $$PWD/qompplaylistview.h

FORMS    += $$PWD/qompmainwin.ui \
    $$PWD/aboutdlg.ui

phonon_backend {
    HEADERS += $$PWD/qompphononplayer.h \
    $$PWD/qompphononmetadataresolver.h

    SOURCES += $$PWD/qompphononplayer.cpp \
    $$PWD/qompphononmetadataresolver.cpp
}

qtmultimedia_backend {
    HEADERS += $$PWD/qompqtmultimediaplayer.h \
        $$PWD/qompqtmultimediametadataresolver.h

    SOURCES += $$PWD/qompqtmultimediaplayer.cpp \
        $$PWD/qompqtmultimediametadataresolver.cpp
}

include (plugins/plugins.pri)
include (options/options.pri)

RESOURCES += $$PWD/qomp.qrc
