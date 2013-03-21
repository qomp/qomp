QT       += core gui network phonon

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qomp
TEMPLATE = app
CONFIG += release
#CONFIG += debug

include(src/src.pri)

MOC_DIR = .moc
OBJECTS_DIR = .obj
UI_DIR = .ui

INCLUDEPATH += $$PWD/.ui
