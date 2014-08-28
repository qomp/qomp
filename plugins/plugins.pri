include($$PWD/../conf.pri)

CONFIG += plugin

DESTDIR = $$OUT_PWD/../../bin/plugins

QT += network

TEMPLATE = lib

INCLUDEPATH += $$PWD/../libqomp/include
DEPENDPATH += $$PWD/../libqomp/include

android {
    LIBS += -L$$OUT_PWD/../../bin -lqomp-shared

    target.path = /libs/$$ANDROID_TARGET_ARCH
    INSTALLS += target
} else {
    LIBS += -L$$OUT_PWD/../../bin -lqomp
}

unix:!android {
    target.path = $$DATADIR/plugins
    INSTALLS += target
}

lupdate_only {
    SOURCES += $$PWD/qml/*.qml
}

