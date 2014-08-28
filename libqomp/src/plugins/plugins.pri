INCLUDEPATH += $$PWD

android {
    SOURCES += $$PWD/qompplugingettunesdlg_mobile.cpp
}
else {
    HEADERS +=  $$PWD/qompplugintreeview.h

    SOURCES +=  $$PWD/qompplugintreeview.cpp \
                $$PWD/qompplugingettunesdlg.cpp

    FORMS   += $$PWD/qompplugingettunesdlg.ui
}

HEADERS += $$PWD/qompplugingettunesdlg.h \
    $$PWD/qompplugintreemodel.h \
    $$PWD/qompplugin.h \
    $$PWD/qompplugintypes.h \
    $$PWD/qomptunepluign.h \
    $$PWD/qompplayerstatusplugin.h \
    $$PWD/qompplugincontroller.h \
    $$PWD/qomppluginaction.h

SOURCES += $$PWD/qompplugintreemodel.cpp \
    $$PWD/qompplugintypes.cpp \
    $$PWD/qompplugincontroller.cpp \
    $$PWD/qomppluginaction.cpp


