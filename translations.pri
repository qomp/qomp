include(conf.pri)

LANG_PATH = $$PWD/qomp.translations/translations

android {
    TRDESTDIR = $$system_path($$PWD/android/assets/translations)
} else {
    TRDESTDIR = $$system_path($$OUT_PWD/../bin/translations)
}
#mkpath($$TRDESTDIR)
!exists($$TRDESTDIR) {
    system( $$QMAKE_MKDIR $$shell_path($$TRDESTDIR) )
}

QMAKE_PRE_LINK = $$shell_path($$[QT_INSTALL_BINS]/lrelease) $$shell_path($$PWD/qomp.pro)

unix:!android {
        translations.path = $$DATADIR/translations
        translations.extra = $$QMAKE_COPY $$shell_path($$LANG_PATH/qomp_*.qm) $(INSTALL_ROOT)$$translations.path
        INSTALLS += translations
} else {
    QMAKE_POST_LINK = $$QMAKE_COPY $$shell_path($$[QT_INSTALL_TRANSLATIONS]/*_ru.qm) $$TRDESTDIR \
                        $$escape_expand(\\n\\t) \
                        $$QMAKE_COPY $$shell_path($$LANG_PATH/*.qm) $$TRDESTDIR
}
