include($$PWD/../conf.pri)

QT += multimedia concurrent network
DEFINES += HAVE_QTMULTIMEDIA
CONFIG += qtmultimedia_backend


TARGET = qomp
TEMPLATE = app


CONFIG(debug, debug|release) {
#    win32: CONFIG += console
    macx:  CONFIG -= app_bundle
}

include($$PWD/src.pri)
include($$PWD/../translations.pri)
include($$PWD/../themes.pri)

unix:!android {
	INCLUDEPATH += /usr/include/KDE/
        QMAKE_RPATHDIR += $$LIBSDIR $$OUT_PWD/../bin
	
	target.path = $$BINDIR
	INSTALLS += target
	dt.path = $$PREFIX/share/applications/
	dt.files = $$PWD/../qomp.desktop
	icon.path = $$PREFIX/share/icons/hicolor/128x128/apps
        icon.extra = $$QMAKE_COPY $$PWD/icons/qomp.png $(INSTALL_ROOT)$$icon.path/qomp.png
	icon1.path = $$PREFIX/share/icons/hicolor/16x16/apps
        icon1.extra = $$QMAKE_COPY $$PWD/icons/qomp_16.png $(INSTALL_ROOT)$$icon1.path/qomp.png
	icon2.path = $$PREFIX/share/icons/hicolor/24x24/apps
        icon2.extra = $$QMAKE_COPY $$PWD/icons/qomp_24.png $(INSTALL_ROOT)$$icon2.path/qomp.png
	icon3.path = $$PREFIX/share/icons/hicolor/48x48/apps
        icon3.extra = $$QMAKE_COPY $$PWD/icons/qomp_48.png $(INSTALL_ROOT)$$icon3.path/qomp.png
        icon4.path = $$PREFIX/share/icons/hicolor/64x64/apps
        icon4.extra = $$QMAKE_COPY $$PWD/icons/qomp_64.png $(INSTALL_ROOT)$$icon4.path/qomp.png
	icon5.path = $$PREFIX/share/icons/hicolor/32x32/apps
        icon5.extra = $$QMAKE_COPY $$PWD/icons/qomp_32.png $(INSTALL_ROOT)$$icon5.path/qomp.png

        INSTALLS += dt icon icon1 icon2 icon3 icon4 icon5
}

win32 {
    RC_FILE += $$PWD/../qomp.rc
    lessThan(QT_MAJOR_VERSION, 6): QT += winextras
}

macx {
   ICON = icons/qomp.icns
   QMAKE_INFO_PLIST = ../Info.plist
   QMAKE_LFLAGS += -framework CoreFoundation -framework Carbon -lobjc
}

android {
    greaterThan(QT_MINOR_VERSION, 12) {
        ANDROID_MIN_SDK_VERSION = 21
        ANDROID_TARGET_SDK_VERSION = 28
    }
    LIBS += -L$$OUT_PWD/../libqomp
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/../android
    ANDROID_EXTRA_LIBS += \
        $$TAG_LIB/libtag.so \
        $$CUE_LIB/libcue.so

    greaterThan(QT_MINOR_VERSION, 13) {
        LIBS += -lqomp-shared_$${QT_ARCH}
        ANDROID_EXTRA_LIBS +=$$OUT_PWD/../libqomp/libqomp-shared_$${QT_ARCH}.so
    }
    else {
        LIBS += -lqomp-shared
        ANDROID_EXTRA_LIBS +=$$OUT_PWD/../libqomp/libqomp-shared.so
    }

    isEmpty(SSL_PATH) {
        include($$PWD/../android_openssl/openssl.pri)
    }
    else {
        ANDROID_EXTRA_LIBS += \
        $$SSL_PATH/lib/libcrypto.so \
        $$SSL_PATH/lib/libssl.so
    }

    !CONFIG(debug, debug|release) {
        greaterThan(QT_MINOR_VERSION, 10) {
            greaterThan(QT_MINOR_VERSION, 13) {
                ANDROID_BUNDLED_JAR_DEPENDENCIES = \
                    jar/QtAndroid.jar \
                    jar/QtAndroidBearer.jar \
                    jar/QtAndroidExtras.jar \
                    jar/QtMultimedia.jar
                    jar/QtAndroidNetwork.jar
            }
            else {
                ANDROID_DEPLOYMENT_DEPENDENCIES = \
                    jar/QtAndroid.jar \
                    jar/QtAndroidBearer.jar \
                    jar/QtAndroidExtras.jar \
                    jar/QtMultimedia.jar
            }
        }
        else {
            ANDROID_DEPLOYMENT_DEPENDENCIES = \
                jar/QtAndroid-bundled.jar \
                jar/QtAndroidBearer-bundled.jar \
                jar/QtMultimedia-bundled.jar
        }

        greaterThan(QT_MINOR_VERSION, 13) {
            ANDROID_DEPLOYMENT_DEPENDENCIES += \
                lib/libQt5Core_$${QT_ARCH}.so \
                lib/libQt5Concurrent_$${QT_ARCH}.so \
                lib/libQt5AndroidExtras_$${QT_ARCH}.so \
                lib/libQt5Network_$${QT_ARCH}.so \
                lib/libQt5Qml_$${QT_ARCH}.so \
                lib/libQt5Gui_$${QT_ARCH}.so \
                lib/libQt5Quick_$${QT_ARCH}.so \
                lib/libQt5Multimedia_$${QT_ARCH}.so \
                lib/libQt5Xml_$${QT_ARCH}.so

            ANDROID_DEPLOYMENT_DEPENDENCIES += \
                plugins/bearer/libplugins_bearer_qandroidbearer_$${QT_ARCH}.so \
                plugins/platforms/libplugins_platforms_qtforandroid_$${QT_ARCH}.so \
                plugins/mediaservice/libplugins_mediaservice_qtmedia_android_$${QT_ARCH}.so \
                plugins/audio/libplugins_audio_qtaudio_opensles_$${QT_ARCH}.so \
                plugins/imageformats/libplugins_imageformats_qgif_$${QT_ARCH}.so \
                plugins/imageformats/libplugins_imageformats_qjpeg_$${QT_ARCH}.so

            ANDROID_DEPLOYMENT_DEPENDENCIES += \
                qml/QtQuick/Dialogs/libqml_QtQuick_Dialogs_dialogplugin_$${QT_ARCH}.so \
                qml/QtQuick/Dialogs/Private/libqml_QtQuick_Dialogs_Private_dialogsprivateplugin_$${QT_ARCH}.so

            ANDROID_DEPLOYMENT_DEPENDENCIES += \
                qml/QtQuick/Window.2/libqml_QtQuick_Window.2_windowplugin_$${QT_ARCH}.so

            ANDROID_DEPLOYMENT_DEPENDENCIES += \
                qml/QtQuick/Layouts/libqml_QtQuick_Layouts_qquicklayoutsplugin_$${QT_ARCH}.so

            ANDROID_DEPLOYMENT_DEPENDENCIES += \
                qml/QtQuick.2/libqml_QtQuick.2_qtquick2plugin_$${QT_ARCH}.so

            ANDROID_DEPLOYMENT_DEPENDENCIES += \
                qml/Qt/labs/folderlistmodel/libqml_Qt_labs_folderlistmodel_qmlfolderlistmodelplugin_$${QT_ARCH}.so
        }
        else {
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
                plugins/audio/libqtaudio_opensles.so \
                plugins/imageformats/libqgif.so \
                plugins/imageformats/libqjpeg.so

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
                qml/Qt/labs/folderlistmodel/qmldir \
                qml/Qt/labs/folderlistmodel/libqmlfolderlistmodelplugin.so \
                qml/Qt/labs/folderlistmodel/plugins.qmltypes

            ANDROID_DEPLOYMENT_DEPENDENCIES += \
                qml/QtQuick/Layouts/libqquicklayoutsplugin.so \
                qml/QtQuick/Layouts/qmldir \
                qml/QtQuick/Layouts/plugins.qmltypes
        }
    }
    else {
        QT += xml #for lastfm plugin
    }

    DISTFILES += \
        $$PWD/../android/AndroidManifest.xml \
        $$PWD/../android/gradle/wrapper/gradle-wrapper.jar \
        $$PWD/../android/gradlew \
        $$PWD/../android/res/values/libs.xml \
        $$PWD/../android/res/values/strings.xml \
        $$PWD/../android/res/values-ru/strings.xml \
        $$PWD/../android/build.gradle \
        $$PWD/../android/gradle/wrapper/gradle-wrapper.properties \
        $$PWD/../android/gradlew.bat \
        $$PWD/../android/gradle.properties

    OTHER_FILES += \
        $$PWD/../android/src/net/sourceforge/qomp/Qomp.java \
        $$PWD/../android/src/net/sourceforge/qomp/QompService.java \
        $$PWD/../android/src/net/sourceforge/qomp/MediaButtonReceiver.java
}
else {
    LIBS += -L$$OUT_PWD/../bin -lqomp
    DESTDIR = $$OUT_PWD/../bin
}

win32|android {
    LIBS += -L$$TAG_LIB
    INCLUDEPATH += $$TAG_INCLUDE
}

mac {
    QMAKE_LFLAGS += -framework tag
}
else {
    LIBS += -ltag
}

win32 {
    QMAKE_POST_LINK += $$QMAKE_COPY $$shell_path($$TAG_BIN/libtag.dll) $$shell_path($$OUT_PWD/../bin) \
                $$escape_expand(\\n\\t)

    QMAKE_POST_LINK += $$QMAKE_COPY $$shell_path($$CUE_BIN/libcue.dll) $$shell_path($$OUT_PWD/../bin) \
                $$escape_expand(\\n\\t)
}

INCLUDEPATH += $$PWD/../libqomp/include
DEPENDPATH += $$PWD/../libqomp/include
