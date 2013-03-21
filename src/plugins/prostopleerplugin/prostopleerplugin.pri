INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += $$PWD/prostopleerplugin.h \
    plugins/prostopleerplugin/prostopleerplugingettunesdialog.h \
    plugins/prostopleerplugin/prostopleermodel.h \
    plugins/prostopleerplugin/prostopleerpluginsettings.h \
    plugins/prostopleerplugin/prostopleerpluginsettingsdlg.h \
    plugins/prostopleerplugin/prostopleerplugindefines.h

SOURCES += $$PWD/prostopleerplugin.cpp \
    plugins/prostopleerplugin/prostopleerplugingettunesdialog.cpp \
    plugins/prostopleerplugin/prostopleermodel.cpp \
    plugins/prostopleerplugin/prostopleerpluginsettings.cpp \
    plugins/prostopleerplugin/prostopleerpluginsettingsdlg.cpp

FORMS += \
    plugins/prostopleerplugin/prostopleerplugingettunesdialog.ui \
    plugins/prostopleerplugin/prostopleerpluginsettings.ui \
    plugins/prostopleerplugin/prostopleerpluginsettingsdlg.ui
