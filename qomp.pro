!equals(QT_MAJOR_VERSION, 5) {
    error("Qt5 only supported!")
}

TEMPLATE = subdirs

sub_libqomp.subdir = libqomp

sub_plugins.subdir = plugins
sub_plugins.depends = sub_libqomp

sub_src.subdir = src
sub_src.depends = sub_libqomp

SUBDIRS += \
	sub_libqomp \
	sub_src \
        sub_plugins

TRANSLATIONS = $$files($$PWD/translations/translations/*.ts)
