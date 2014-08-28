INCLUDEPATH += $$PWD

HEADERS += $$PWD/qompoptionsdlg.h \
    $$PWD/qompoptionsmain.h

android {
    SOURCES += \
        $$PWD/qompoptionsdlg_mobile.cpp \
        $$PWD/qompoptionsmain_mobile.cpp
}
else {
    SOURCES += $$PWD/qompoptionsdlg.cpp \
        $$PWD/qompoptionsmain.cpp \
        $$PWD/qompoptionsplugins.cpp

    HEADERS += $$PWD/qompoptionsplugins.h

    FORMS += $$PWD/qompoptionsdlg.ui \
        $$PWD/qompoptionsmain.ui \
        $$PWD/qompoptionsplugins.ui
}
