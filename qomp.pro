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

LANG_PATH = qomp.translations/translations
TRANSLATIONS = $$LANG_PATH/qomp_ru.ts


