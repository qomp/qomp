INCLUDEPATH += $$PWD

HEADERS += $$PWD/qompplugintracksmodel.h \
    $$PWD/qompplugintracksview.h \
    $$PWD/qompplugingettunesdlg.h \
    src/plugins/qomppluginalbumsview.h \
    src/plugins/qomppluginalbumsmodel.h \
    src/plugins/qompplugintypes.h

SOURCES += $$PWD/qompplugintracksmodel.cpp \
    $$PWD/qompplugintracksview.cpp \
    $$PWD/qompplugingettunesdlg.cpp \
    src/plugins/qomppluginalbumsview.cpp \
    src/plugins/qomppluginalbumsmodel.cpp

FORMS += $$PWD/qompplugingettunesdlg.ui


include (filesystemplugin/filesystemplugin.pri)
include (urlplugin/urlplugin.pri)
include (prostopleerplugin/prostopleerplugin.pri)
include (myzukaruplugin/myzukaruplugin.pri)
