INCLUDEPATH += $$PWD

HEADERS += $$PWD/qompoptionsdlg.h \
    $$PWD/qompoptionsmain.h \
    $$PWD/qompoptionsplugins.h

android {
    SOURCES += \
        $$PWD/qompoptionsdlg_mobile.cpp \
        $$PWD/qompoptionsmain_mobile.cpp \
        $$PWD/qompoptionsplugins_mobile.cpp
}
else {
    SOURCES += $$PWD/qompoptionsdlg.cpp \
        $$PWD/qompoptionsmain.cpp \
        $$PWD/qompoptionsplugins.cpp

    FORMS += $$PWD/qompoptionsdlg.ui \
        $$PWD/qompoptionsmain.ui \
        $$PWD/qompoptionsplugins.ui
}
