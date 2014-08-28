INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += $$PWD/main.cpp \
    $$PWD/qompcon.cpp \
    $$PWD/qompplaylistmodel.cpp \    
    $$PWD/qomptunedownloader.cpp \
    $$PWD/qompmenu.cpp \
    $$PWD/translator.cpp
#    $$PWD/qomptaglibmetadataresolver.cpp \


HEADERS  += $$PWD/qompmainwin.h \
    $$PWD/qompcon.h \
    $$PWD/qompplaylistmodel.h \
    $$PWD/qomptunedownloader.h \
    $$PWD/qompmenu.h \
    $$PWD/translator.h
#    $$PWD/qomptaglibmetadataresolver.h \

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
        $$PWD/updateschecker.cpp \        
        $$PWD/thememanager.cpp

    HEADERS += $$PWD/qomptrayicon.h \
        $$PWD/aboutdlg.h \
        $$PWD/qompplaylistdelegate.h \
        $$PWD/updateschecker.h \
        $$PWD/qompplaylistview.h \       
        $$PWD/thememanager.h

    FORMS   += \
        $$PWD/qompmainwin.ui \
        $$PWD/aboutdlg.ui

    RESOURCES   += $$PWD/themes.qrc
}

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

lupdate_only {
    SOURCES += $$PWD/qml/*.qml
}
include (options/options.pri)
