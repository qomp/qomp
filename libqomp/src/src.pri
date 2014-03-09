INCLUDEPATH += $$PWD
SOURCES += \
        $$PWD/common.cpp \
        $$PWD/options.cpp \
        $$PWD/qompbusylabel.cpp \
        $$PWD/qompmetadataresolver.cpp \
        $$PWD/qompnetworkingfactory.cpp \
        $$PWD/qompplayer.cpp \
        $$PWD/tune.cpp \
        $$PWD/pluginmanager.cpp

HEADERS +=\
        $$PWD/libqomp_global.h \
        $$PWD/common.h \
        $$PWD/options.h \
        $$PWD/qompbusylabel.h \
        $$PWD/qompmetadataresolver.h \
        $$PWD/qompnetworkingfactory.h \
        $$PWD/qompplayer.h \
        $$PWD/tune.h \
        $$PWD/defines.h \
        $$PWD/pluginmanager.h

include(plugins/plugins.pri)
include(options/options.pri)
