include(conf.pri)

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

unix:!android {
        translations.path = $$DATADIR/translations
        LRELEASE = "lrelease"
        !exists($$LRELEASE)
            LRELEASE = "lrelease-qt4"

        translations.extra = lrelease qomp.pro && cp -f $$LANG_PATH/qomp_*.qm  $(INSTALL_ROOT)$$translations.path
        INSTALLS += translations
}
