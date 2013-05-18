INCLUDEPATH += $$PWD

HEADERS += $$PWD/qompplugingettunesdlg.h \
    $$PWD/qompplugintreeview.h \
    $$PWD/qompplugintreemodel.h \
    $$PWD/qompplugin.h \
    $$PWD/qompplugintypes.h

SOURCES += $$PWD/qompplugingettunesdlg.cpp \
    $$PWD/qompplugintreeview.cpp \
    $$PWD/qompplugintreemodel.cpp \
    $$PWD/qompplugintypes.cpp

FORMS += $$PWD/qompplugingettunesdlg.ui


include (filesystemplugin/filesystemplugin.pri)
include (urlplugin/urlplugin.pri)
include (prostopleerplugin/prostopleerplugin.pri)
include (myzukaruplugin/myzukaruplugin.pri)
include (yandexmusicplugin/yandexmusicplugin.pri)
