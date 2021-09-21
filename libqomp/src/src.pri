INCLUDEPATH += $$PWD

SOURCES += $$PWD/common.cpp \
        $$PWD/options.cpp \
        $$PWD/qompmetadataresolver.cpp \
        $$PWD/qompnetworkingfactory.cpp \
        $$PWD/qompplayer.cpp \
        $$PWD/tune.cpp \
        $$PWD/pluginmanager.cpp \
        $$PWD/pluginhost.cpp \
        $$PWD/tuneurlchecker.cpp \
        $$PWD/gettuneurlhelper.cpp \
        $$PWD/covercache.cpp \
        $$PWD/taghelpers.cpp \
        $$PWD/playlistparser.cpp

HEADERS += $$PWD/libqomp_global.h \
        $$PWD/common.h \
        $$PWD/options.h \        
        $$PWD/qompmetadataresolver.h \
        $$PWD/qompnetworkingfactory.h \
        $$PWD/qompplayer.h \
        $$PWD/tune.h \
        $$PWD/defines.h \
        $$PWD/pluginmanager.h \
        $$PWD/pluginhost.h \
        $$PWD/tuneurlchecker.h \
        $$PWD/gettuneurlhelper.h \
        $$PWD/covercache.h \
        $$PWD/taghelpers.h \
        $$PWD/playlistparser.h \
        $$PWD/qompplayercontrol.h


RESOURCES += $$PWD/icons.qrc

android {
    HEADERS += $$PWD/qompqmlengine.h  \
         $$PWD/scaler.h \
         $$PWD/filesystemhelper.h

    SOURCES += $$PWD/qompqmlengine.cpp  \
         $$PWD/scaler.cpp \
         $$PWD/filesystemhelper.cpp

    RESOURCES += $$PWD/qml.qrc
}
else {
    HEADERS += $$PWD/qompbusylabel.h \
               $$PWD/qompactionslist.h \
               $$PWD/thememanager.h \
               $$PWD/extendedcombobox.h

    SOURCES += $$PWD/qompbusylabel.cpp \
               $$PWD/qompactionslist.cpp \
               $$PWD/thememanager.cpp \
               $$PWD/extendedcombobox.cpp


    include($$PWD/../../ga/qt-google-analytics.pri)
}

lupdate_only {
    SOURCES += $$PWD/qmlshared/*.qml
}

include(plugins/plugins.pri)
include(options/options.pri)

!android: include(frameless/frameless.pri)
