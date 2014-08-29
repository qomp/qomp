include($$PWD/../conf.pri)

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += multimedia
    DEFINES += HAVE_QTMULTIMEDIA
    CONFIG += qtmultimedia_backend
}
else {
    CONFIG += phonon_backend
}

phonon_backend {
    QT += phonon
    DEFINES += HAVE_PHONON
}

TARGET = qomp
DESTDIR = $$OUT_PWD/../bin
TEMPLATE = app

QT += network


include(src.pri)
include($$PWD/../translations.pri)

unix:!android {
	INCLUDEPATH += /usr/include/KDE/
	
	target.path = $$BINDIR
	INSTALLS += target
	dt.path = $$PREFIX/share/applications/
	dt.files = $$PWD/../qomp.desktop
	icon.path = $$PREFIX/share/icons/hicolor/128x128/apps
	icon.extra = cp -f icons/qomp.png $(INSTALL_ROOT)$$icon.path/qomp.png
	icon1.path = $$PREFIX/share/icons/hicolor/16x16/apps
	icon1.extra = cp -f icons/qomp_16.png $(INSTALL_ROOT)$$icon1.path/qomp.png
	icon2.path = $$PREFIX/share/icons/hicolor/24x24/apps
	icon2.extra = cp -f icons/qomp_24.png $(INSTALL_ROOT)$$icon2.path/qomp.png
	icon3.path = $$PREFIX/share/icons/hicolor/48x48/apps
	icon3.extra = cp -f icons/qomp_48.png $(INSTALL_ROOT)$$icon3.path/qomp.png
        icon4.path = $$PREFIX/share/icons/hicolor/64x64/apps
	icon4.extra = cp -f icons/qomp_64.png $(INSTALL_ROOT)$$icon4.path/qomp.png
	icon5.path = $$PREFIX/share/icons/hicolor/32x32/apps
	icon5.extra = cp -f icons/qomp_32.png $(INSTALL_ROOT)$$icon5.path/qomp.png

        INSTALLS += dt icon icon1 icon2 icon3 icon4 icon5
}

win32:RC_FILE += $$PWD/../qomp.rc

macx {
   ICON = icons/qomp.icns
   QMAKE_INFO_PLIST = ../Info.plist
}

android {
    QT += xml
    OTHER_FILES += $$PWD/../android/AndroidManifest.xml
    LIBS += -L$$OUT_PWD/../bin -lqomp-shared
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/../android
    ANDROID_EXTRA_LIBS += $$OUT_PWD/../bin/libqomp-shared.so $$[QT_INSTALL_LIBS]/libtag.so
} else {
    LIBS += -L$$OUT_PWD/../bin -lqomp
}

#LIBS += -ltag

INCLUDEPATH += $$PWD/../libqomp/include
DEPENDPATH += $$PWD/../libqomp/include

OTHER_FILES += \
    ../android/src/com/googlecode/qomp/Qomp.java \
    ../android/src/com/googlecode/qomp/CallbackActivity.java
