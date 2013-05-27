QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets multimedia
    DEFINES += HAVE_QT5 HAVE_QTMULTIMEDIA
    CONFIG += qtmultimedia_backend
}
else {
    CONFIG += phonon_backend
}

phonon_backend {
    QT += phonon
    DEFINES += HAVE_PHONON
}

unix:!mac:DEFINES += HAVE_X11

TARGET = qomp
TEMPLATE = app
CONFIG += release
#CONFIG += debug

isEmpty(PREFIX) {
	PREFIX = /usr/local
}
BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX/share/qomp

DEFINES += QOMP_DATADIR='\\"$$DATADIR\\"'

include(src/src.pri)

MOC_DIR = .moc
OBJECTS_DIR = .obj
UI_DIR = .ui
RCC_DIR = .rcc

INCLUDEPATH += $$PWD/.ui


LANG_PATH = qomp.translations/translations
TRANSLATIONS = $$LANG_PATH/qomp_ru.ts

unix {
	target.path = $$BINDIR
	INSTALLS += target
	dt.path = $$PREFIX/share/applications/
	dt.files = qomp.desktop
	icon.path = $$PREFIX/share/icons/hicolor/128x128/apps
	icon.extra = cp -f src/icons/qomp.png $(INSTALL_ROOT)$$icon.path/qomp.png
	icon1.path = $$PREFIX/share/icons/hicolor/16x16/apps
	icon1.extra = cp -f src/icons/qomp_16.png $(INSTALL_ROOT)$$icon1.path/qomp.png
	icon2.path = $$PREFIX/share/icons/hicolor/24x24/apps
	icon2.extra = cp -f src/icons/qomp_24.png $(INSTALL_ROOT)$$icon2.path/qomp.png
	icon3.path = $$PREFIX/share/icons/hicolor/48x48/apps
	icon3.extra = cp -f src/icons/qomp_48.png $(INSTALL_ROOT)$$icon3.path/qomp.png
        icon4.path = $$PREFIX/share/icons/hicolor/64x64/apps
	icon4.extra = cp -f src/icons/qomp_64.png $(INSTALL_ROOT)$$icon4.path/qomp.png
	icon5.path = $$PREFIX/share/icons/hicolor/32x32/apps
	icon5.extra = cp -f src/icons/qomp_32.png $(INSTALL_ROOT)$$icon5.path/qomp.png

	translations.path = $$DATADIR/translations
	translations.extra = lrelease qomp.pro && cp -f $$LANG_PATH/qomp_*.qm  $(INSTALL_ROOT)$$translations.path
	INSTALLS += translations

	INSTALLS += dt icon icon1 icon2 icon3 icon4 icon5
}

win32:RC_FILE += qomp.rc

macx {
   ICON = $$PWD/src/icons/qomp.icns
   QMAKE_INFO_PLIST = $$PWD/Info.plist
}
