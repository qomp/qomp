include($$PWD/../plugins.pri)

INCLUDEPATH += $$PWD

android {
    SOURCES += $$PWD/cuteradioplugingettunesdialog_mobile.cpp

    RESOURCES += $$PWD/qml.qrc
    QT += quick
}
else {
    SOURCES +=  $$PWD/cuteradioplugingettunesdialog.cpp
}

HEADERS +=  $$PWD/cuteradioplugin.h \
            $$PWD/cuteradioplugingettunesdialog.h \
            $$PWD/cuteradioplugindefines.h \
            $$PWD/cuteradiocontroller.h \
            $$PWD/cuteradiomodel.h

SOURCES +=  $$PWD/cuteradioplugin.cpp \
            $$PWD/cuteradiocontroller.cpp \
            $$PWD/cuteradiomodel.cpp


OTHER_FILES += $$PWD/metadata.json

FORMS += \
    $$PWD/cuteradioplugingettunesdialog.ui
