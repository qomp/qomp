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
    src/qomptrayicon.cpp

HEADERS  += $$PWD/qompmainwin.h \
    $$PWD/qomp.h \
    $$PWD/options.h \
    $$PWD/qompplayer.h \
    $$PWD/playlistmodel.h \
    $$PWD/tune.h \
    $$PWD/qompplugin.h \
    $$PWD/pluginmanager.h \
    src/qomptrayicon.h \
    src/defines.h

FORMS    += $$PWD/qompmainwin.ui

include (plugins/plugins.pri)

RESOURCES += $$PWD/qomp.qrc
