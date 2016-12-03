include($$PWD/../plugins.pri)

INCLUDEPATH += $$PWD

android {
    SOURCES += $$PWD/prostopleerplugingettunesdialog_mobile.cpp \
               $$PWD/prostopleerpluginsettings_mobile.cpp

    RESOURCES += $$PWD/qml.qrc
}
else {
    SOURCES +=  $$PWD/prostopleerplugingettunesdialog.cpp \
                $$PWD/prostopleerpluginsettings.cpp \

    FORMS   +=  $$PWD/prostopleerpluginresultswidget.ui \
                $$PWD/prostopleerpluginsettings.ui
}

HEADERS +=  $$PWD/prostopleerpluginsettings.h \
            $$PWD/prostopleerplugin.h \
            $$PWD/prostopleerplugingettunesdialog.h \
            $$PWD/prostopleerplugindefines.h \
            $$PWD/prostopleercontroller.h

SOURCES +=  $$PWD/prostopleerplugin.cpp \
            $$PWD/prostopleercontroller.cpp


OTHER_FILES += $$PWD/metadata.json
