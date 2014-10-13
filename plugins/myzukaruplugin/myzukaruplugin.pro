include($$PWD/../plugins.pri)

HEADERS += $$PWD/myzukaruplugin.h \
    $$PWD/myzukarudefines.h \
    $$PWD/myzukarugettunesdlg.h \
    $$PWD/myzukarucontroller.h \
    $$PWD/myzukaruresolvestrategy.h

SOURCES += $$PWD/myzukaruplugin.cpp \
    $$PWD/myzukarucontroller.cpp \
    $$PWD/myzukaruresolvestrategy.cpp \


OTHER_FILES += $$PWD/metadata.json

android {
    RESOURCES += qml.qrc
    SOURCES += $$PWD/myzukarugettunesdlg_mobile.cpp
}
else {
    SOURCES += $$PWD/myzukarugettunesdlg.cpp
}


