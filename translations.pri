include(conf.pri)

LANG_PATH = $$PWD/translations/translations

android {
    TRDESTDIR = $$system_path($$PWD/android/assets/translations)
} else {
    TRDESTDIR = $$system_path($$OUT_PWD/../bin/translations)
}
#mkpath($$TRDESTDIR)
!exists($$TRDESTDIR) {
    system( $$QMAKE_MKDIR $$shell_path($$TRDESTDIR) )
}

#CONFIG += lrelease
#QM_FILES_INSTALL_PATH = $$LANG_PATH
#QMAKE_LRELEASE_FLAGS =

QMAKE_PRE_LINK += $$shell_path($$[QT_INSTALL_BINS]/lrelease) $$shell_path($$PWD/qomp.pro) \
                  $$escape_expand(\\n\\t)

unix:!android {
        translations.path = $$DATADIR/translations
        translations.extra = $$QMAKE_COPY $$shell_path($$LANG_PATH/qomp_*.qm) $(INSTALL_ROOT)$$translations.path
        INSTALLS += translations
} else {
    QMAKE_POST_LINK +=$$QMAKE_COPY $$shell_path($$LANG_PATH/*.qm) $$TRDESTDIR \
                        $$escape_expand(\\n\\t)

    TRS = $$files($$LANG_PATH/*.qm)
    for(TRANS, TRS) {
        FILENAME = $$basename(TRANS)
        fname = $$section(FILENAME, ., 0, 0)
        lang = $$sprintf(%1/qt*_%2.qm, $$[QT_INSTALL_TRANSLATIONS], $$section(fname, qomp_, 1, 1))

        QMAKE_POST_LINK += $$QMAKE_COPY $$shell_path($$lang) $$TRDESTDIR \
                        $$escape_expand(\\n\\t)

    }
}
