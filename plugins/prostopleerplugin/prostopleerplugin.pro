include($$PWD/../plugins.pri)

INCLUDEPATH += $$PWD

HEADERS += $$PWD/prostopleerplugin.h \
    $$PWD/prostopleerplugingettunesdialog.h \
    $$PWD/prostopleerpluginsettings.h \
    $$PWD/prostopleerplugindefines.h \
    $$PWD/prostopleercontroller.h

SOURCES += $$PWD/prostopleerplugin.cpp \
    $$PWD/prostopleerplugingettunesdialog.cpp \
    $$PWD/prostopleerpluginsettings.cpp \
    $$PWD/prostopleercontroller.cpp

FORMS += $$PWD/prostopleerpluginresultswidget.ui \
    $$PWD/prostopleerpluginsettings.ui

OTHER_FILES += \
    $$PWD/metadata.json
