equals(QT_MAJOR_VERSION, 4) {
    error("Qt4 support is deprecated!")
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

LANG_PATH = translations/translations
TRANSLATIONS = $$LANG_PATH/qomp_ru.ts


