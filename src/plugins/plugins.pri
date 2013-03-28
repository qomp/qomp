INCLUDEPATH += $$PWD

HEADERS += $$PWD/qompplugintracksmodel.h \
    $$PWD/qompplugintracksview.h \
    $$PWD/qompplugingettunesdlg.h

SOURCES += $$PWD/qompplugintracksmodel.cpp \
    $$PWD/qompplugintracksview.cpp \
    $$PWD/qompplugingettunesdlg.cpp

FORMS += $$PWD/qompplugingettunesdlg.ui


include (filesystemplugin/filesystemplugin.pri)
include (urlplugin/urlplugin.pri)
include (prostopleerplugin/prostopleerplugin.pri)
include (myzukaruplugin/myzukaruplugin.pri)
