include($$PWD/../plugins.pri)

HEADERS += \
    deezerplugin.h \
    deezergettunesdlg.h \
    deezerplugindefines.h \
    deezerauth.h

SOURCES += \
    deezerplugin.cpp \
    deezergettunesdlg.cpp \
    deezerauth.cpp

QT += xml

OTHER_FILES += $$PWD/metadata.json
