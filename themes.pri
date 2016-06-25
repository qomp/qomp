!android {
    include(conf.pri)

    THEMES_PATH = $$PWD/themes
    THEMES_FILE = $$THEMES_PATH/themes.rcc
    THEMESDESTDIR = $$shell_path($$OUT_PWD/../bin/themes)


    !exists($$THEMESDESTDIR) {
        system( $$QMAKE_MKDIR $$shell_path($$THEMESDESTDIR) )
    }

    QMAKE_PRE_LINK += $$shell_path($$[QT_INSTALL_BINS]/rcc) -o $$shell_path($$THEMES_FILE) \
                                                    $$shell_path($$THEMES_PATH/themes.qrc) \
                      $$escape_expand(\\n\\t)

    win32 {
        QMAKE_POST_LINK += $$QMAKE_COPY $$shell_path($$THEMES_FILE) $$THEMESDESTDIR \
                           $$escape_expand(\\n\\t)
    }
    unix {
        themes.path = $$DATADIR/themes
        themes.extra = $$QMAKE_COPY $$shell_path($$THEMES_FILE) $(INSTALL_ROOT)$$themes.path
        INSTALLS += themes
    }
}
