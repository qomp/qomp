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
    LIBS += -L$$OUT_PWD/../bin -lqomp-shared
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/../android
    ANDROID_EXTRA_LIBS += $$OUT_PWD/../bin/libqomp-shared.so # $$[QT_INSTALL_LIBS]/libtag.so

#    ANDROID_DEPLOYMENT_DEPENDENCIES += \
#        lib/libQt5Core.so \
#        lib/libQt5Gui.so \
#        lib/libQt5Network.so \
#        lib/libQt5Multimedia.so \
#        lib/libQt5Qml.so \
#        lib/libQt5Quick.so \
#        lib/libQt5Xml.so \
#        lib/libQt5AndroidExtras.so \
#        lib/libQt5Concurrent.so \
#        lib/libQt5MultimediaQuick_p.so \
#        lib/libQt5QuickParticles.so \
#        plugins/accessible/libqtaccessiblequick.so \
#        plugins/audio/libqtaudio_opensles.so \
#        plugins/bearer/libqgenericbearer.so \
#        plugins/generic/libqevdevkeyboardplugin.so \
#        plugins/generic/libqevdevmouseplugin.so \
#        plugins/generic/libqevdevtabletplugin.so \
#        plugins/generic/libqevdevtouchplugin.so \
#        plugins/mediaservice/libqtmedia_android.so \
#        plugins/platforms/android/libqtforandroid.so \
#        plugins/platforms/libqminimal.so \
#        plugins/platforms/libqoffscreen.so \
#        plugins/playlistformats/libqtmultimedia_m3u.so \
#        qml/QtQuick/Dialogs/libdialogplugin.so \
#        qml/QtQuick/Dialogs/Private/libdialogsprivateplugin.so \
#        qml/QtQuick/Controls/libqtquickcontrolsplugin.so \
#        qml/QtQuick/Window.2/libwindowplugin.so \
#        qml/QtMultimedia/libdeclarative_multimedia.so \
#        qml/QtQml/Models.2/libmodelsplugin.so \
#        qml/QtQuick.2/libqtquick2plugin.so \
#        qml/QtQuick/Controls/qmldir \
#        qml/QtQuick/Controls/plugins.qmltypes \
#        qml/QtQuick/Dialogs/qmldir \
#        qml/QtQuick/Dialogs/plugins.qmltypes \
#        qml/QtQuick/Dialogs/Private/qmldir \
#        qml/QtQuick/Window.2/qmldir \
#        qml/QtQuick/Window.2/plugins.qmltypes \
#        qml/QtQuick/Particles.2/libparticlesplugin.so \
#        qml/QtQuick/Layouts/libqquicklayoutsplugin.so \
#        qml/QtQuick.2/qmldir \
#        qml/QtQuick.2/plugins.qmltypes \
#        qml/QtMultimedia/qmldir \
#        qml/QtMultimedia/plugins.qmltypes \
#        qml/QtQml/Models.2/qmldir \
#        qml/QtQuick/Particles.2/qmldir \
#        qml/QtQuick/Particles.2/plugins.qmltypes \
#        qml/QtQuick/Layouts/qmldir \
#        qml/QtQuick/Layouts/plugins.qmltypes \
#        qml/QtQuick/Controls/CheckBox.qml \
#        qml/QtQuick/Controls/ComboBox.qml \
#        qml/QtQuick/Controls/Menu.qml \
#        qml/QtQuick/Controls/Slider.qml \
#        qml/QtQuick/Controls/StackView.qml \
#        qml/QtQuick/Controls/StackViewDelegate.qml \
#        qml/QtQuick/Controls/StackViewTransition.qml \
#        qml/QtQuick/Controls/Private/qmldir \
#        qml/QtQuick/Controls/Private/StackView.js \
#        qml/QtQuick/Controls/Private/style.js \
#        qml/QtQuick/Controls/Private/AbstractCheckable.qml \
#        qml/QtQuick/Controls/Private/BasicButton.qml \
#        qml/QtQuick/Controls/Private/ColumnMenuContent.qml \
#        qml/QtQuick/Controls/Private/ContentItem.qml \
#        qml/QtQuick/Controls/Private/Control.qml \
#        qml/QtQuick/Controls/Private/FastGlow.qml \
#        qml/QtQuick/Controls/Private/FocusFrame.qml \
#        qml/QtQuick/Controls/Private/MenuContentItem.qml \
#        qml/QtQuick/Controls/Private/HoverButton.qml \
#        qml/QtQuick/Controls/Private/MenuContentScroller.qml \
#        qml/QtQuick/Controls/Private/ModalPopupBehavior.qml \
#        qml/QtQuick/Controls/Private/ScrollBar.qml \
#        qml/QtQuick/Controls/Private/ScrollViewHelper.qml \
#        qml/QtQuick/Controls/Private/SourceProxy.qml \
#        qml/QtQuick/Controls/Private/StackViewSlideDelegate.qml \
#        qml/QtQuick/Controls/Private/Style.qml \
#        qml/QtQuick/Controls/Private/SystemPaletteSingleton.qml \
#        qml/QtQuick/Controls/Styles/qmldir \
#        qml/QtQuick/Controls/Styles/Base/CheckBoxStyle.qml \
#        qml/QtQuick/Controls/Styles/Base/MenuStyle.qml \
#        qml/QtQuick/Controls/Styles/Base/ComboBoxStyle.qml \
#        qml/QtQuick/Controls/Styles/Base/SliderStyle.qml \
#        qml/QtQuick/Controls/Styles/Base/images/arrow-down.png \
#        qml/QtQuick/Controls/Styles/Base/images/arrow-down@2x.png \
#        qml/QtQuick/Controls/Styles/Base/images/check.png \
#        qml/QtQuick/Controls/Styles/Base/images/check@2x.png \
#        qml/QtQuick/Controls/Styles/Base/images/slider-groove.png \
#        qml/QtQuick/Controls/Styles/Base/images/slider-handle.png \
#        jar/QtAndroid-bundled.jar \
#        jar/QtMultimedia-bundled.jar \
#        jar/QtAndroidAccessibility-bundled.jar

} else {
    LIBS += -L$$OUT_PWD/../bin -lqomp
}

#LIBS += -ltag

INCLUDEPATH += $$PWD/../libqomp/include
DEPENDPATH += $$PWD/../libqomp/include
