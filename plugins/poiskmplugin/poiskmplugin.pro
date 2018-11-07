include($$PWD/../plugins.pri)

INCLUDEPATH += $$PWD

android {
    SOURCES += $$PWD/poiskmplugingettunesdialog_mobile.cpp

    RESOURCES += $$PWD/qml.qrc
}
else {
    SOURCES +=  $$PWD/poiskmplugingettunesdialog.cpp

    FORMS   +=  $$PWD/poiskmpluginresultswidget.ui
}

HEADERS +=  $$PWD/poiskmplugin.h \
            $$PWD/poiskmplugingettunesdialog.h \
            $$PWD/poiskmplugindefines.h \
            $$PWD/poiskmcontroller.h \
            $$PWD/poiskmurlresolvestrategy.h

SOURCES +=  $$PWD/poiskmplugin.cpp \
            $$PWD/poiskmcontroller.cpp \
            $$PWD/poiskmurlresolvestrategy.cpp


OTHER_FILES += $$PWD/metadata.json
