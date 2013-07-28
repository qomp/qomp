HEADERS += $$PWD/lastfmplugin.h \
    $$PWD/lastfmsettings.h \
    $$PWD/lastfmdefines.h \
    $$PWD/lastfmauthdlg.h

SOURCES += $$PWD/lastfmplugin.cpp \
    $$PWD/lastfmsettings.cpp \
    $$PWD/lastfmauthdlg.cpp

FORMS += $$PWD/lastfmsettings.ui \
    $$PWD/lastfmauthdlg.ui

QT += xml webkit

LIBS += -llastfm

