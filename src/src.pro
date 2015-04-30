include($$PWD/../conf.pri)

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += multimedia concurrent
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

CONFIG(debug, debug|release) {
    greaterThan(QT_MAJOR_VERSION, 4):win32: CONFIG += console
    macx: CONFIG -= app_bundle
}

include(src.pri)
include($$PWD/../translations.pri)

unix:!android {
	INCLUDEPATH += /usr/include/KDE/
        QMAKE_RPATHDIR += $$LIBSDIR $$OUT_PWD/../bin
	
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
   QMAKE_LFLAGS += -framework CoreFoundation -framework Carbon -lobjc
}

android {
    LIBS += -L$$OUT_PWD/../bin -lqomp-shared
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/../android
    ANDROID_EXTRA_LIBS += $$[QT_INSTALL_LIBS]/libtag.so \
        $$OUT_PWD/../bin/libqomp-shared.so

    ANDROID_DEPLOYMENT_DEPENDENCIES = \
        jar/QtAndroid-bundled.jar \
        jar/QtAndroidAccessibility-bundled.jar \
        jar/QtAndroidBearer-bundled.jar \
        jar/QtMultimedia-bundled.jar

    ANDROID_DEPLOYMENT_DEPENDENCIES += \
        lib/libQt5Core.so \
        lib/libQt5Concurrent.so \
        lib/libQt5AndroidExtras.so \
        lib/libQt5Network.so \
        lib/libQt5Qml.so \
        lib/libQt5Gui.so \
        lib/libQt5Quick.so \
        lib/libQt5Multimedia.so \
        lib/libQt5Xml.so

    ANDROID_DEPLOYMENT_DEPENDENCIES += \
        plugins/bearer/libqandroidbearer.so \
        plugins/platforms/android/libqtforandroid.so \
        plugins/mediaservice/libqtmedia_android.so \
        plugins/audio/libqtaudio_opensles.so

    ANDROID_DEPLOYMENT_DEPENDENCIES += \
        qml/QtQuick/Dialogs/qmldir \
        qml/QtQuick/Dialogs/libdialogplugin.so \
        qml/QtQuick/Dialogs/plugins.qmltypes \
        qml/QtQuick/Dialogs/Private/qmldir \
        qml/QtQuick/Dialogs/Private/libdialogsprivateplugin.so

    ANDROID_DEPLOYMENT_DEPENDENCIES += \
        qml/QtQuick/Window.2/plugins.qmltypes \
        qml/QtQuick/Window.2/qmldir \
        qml/QtQuick/Window.2/libwindowplugin.so

    ANDROID_DEPLOYMENT_DEPENDENCIES += \
        qml/QtQuick.2/plugins.qmltypes \
        qml/QtQuick.2/qmldir \
        qml/QtQuick.2/libqtquick2plugin.so

    ANDROID_DEPLOYMENT_DEPENDENCIES += \
        qml/QtQuick/Controls/libqtquickcontrolsplugin.so \
        qml/QtQuick/Controls/qmldir \
        qml/QtQuick/Controls/plugins.qmltypes \
        qml/QtQuick/Controls/Styles/qmldir \
        qml/QtQuick/Controls/Styles/Android/qmldir \
        qml/QtQuick/Controls/Styles/Android/libqtquickcontrolsandroidstyleplugin.so

    CONFIG(debug, debug|release) {
        ANDROID_DEPLOYMENT_DEPENDENCIES += \
            plugins/generic/libqevdevkeyboardplugin.so \
            plugins/generic/libqevdevmouseplugin.so \
            plugins/generic/libqevdevtabletplugin.so \
            plugins/generic/libqevdevtouchplugin.so \
            plugins/qmltooling/libqmldbg_qtquick2.so \
            plugins/qmltooling/libqmldbg_tcp.so
    }

    ANDROID_DEPLOYMENT_DEPENDENCIES += \
        qml/Qt/labs/folderlistmodel/qmldir \
        qml/Qt/labs/folderlistmodel/libqmlfolderlistmodelplugin.so \
        qml/Qt/labs/folderlistmodel/plugins.qmltypes

#    ANDROID_DEPLOYMENT_DEPENDENCIES += \
#        qml/QtQml/Models.2/qmldir \
#        qml/QtQml/Models.2/libmodelsplugin.so \
#        qml/Qt/labs/settings/qmldir \
#        qml/Qt/labs/settings/libqmlsettingsplugin.so \
#        qml/Qt/labs/settings/plugins.qmltypes \
#        qml/QtQuick/Layouts/libqquicklayoutsplugin.so \
#        qml/QtQuick/Layouts/qmldir \
#        qml/QtQuick/Layouts/plugins.qmltypes \
#        qml/QtQml/StateMachine/qmldir \
#        qml/QtQml/StateMachine/plugins.qmltypes \
#        qml/QtQml/StateMachine/libqtqmlstatemachine.so

#    ANDROID_DEPLOYMENT_DEPENDENCIES += \
#        qml/QtMultimedia/libdeclarative_multimedia.so \
#        qml/QtMultimedia/plugins.qmltypes \
#        qml/QtMultimedia/qmldir \
#        qml/QtMultimedia/Video.qml \

#    ANDROID_DEPLOYMENT_DEPENDENCIES += \
#        plugins/video/videonode/libqtsgvideonode_android.so \
#        plugins/playlistformats/libqtmultimedia_m3u.so \
#        plugins/platforms/libqminimal.so \
#        plugins/platforms/libqoffscreen.so \
} else {
    LIBS += -L$$OUT_PWD/../bin -lqomp
}

INCLUDEPATH += $$PWD/../libqomp/include
DEPENDPATH += $$PWD/../libqomp/include
