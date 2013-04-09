INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
SOURCES += $$PWD/main.cpp\
    $$PWD/qompmainwin.cpp \
    $$PWD/qomp.cpp \
    $$PWD/options.cpp \
    $$PWD/qompplayer.cpp \
    $$PWD/playlistmodel.cpp \
    $$PWD/pluginmanager.cpp \
    $$PWD/tune.cpp \
    $$PWD/qomptrayicon.cpp \
    $$PWD/common.cpp \
    $$PWD/qompnetworkingfactory.cpp \
    $$PWD/qompmetadataresolver.cpp \
    $$PWD/aboutdlg.cpp


HEADERS  += $$PWD/qompmainwin.h \
    $$PWD/qomp.h \
    $$PWD/options.h \
    $$PWD/qompplayer.h \
    $$PWD/playlistmodel.h \
    $$PWD/tune.h \
    $$PWD/qompplugin.h \
    $$PWD/pluginmanager.h \
    $$PWD/qomptrayicon.h \
    $$PWD/defines.h \
    $$PWD/common.h \
    $$PWD/qompnetworkingfactory.h \
    $$PWD/qompmetadataresolver.h \
    $$PWD/aboutdlg.h


FORMS    += $$PWD/qompmainwin.ui \
    $$PWD/aboutdlg.ui


include (plugins/plugins.pri)
include (options/options.pri)

RESOURCES += $$PWD/qomp.qrc
