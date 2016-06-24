win32 {
    include(conf.pri)

    THEMES_PATH = $$PWD/themes
    THEMES_FILE = $$THEMES_PATH/themes.rcc
    THEMESDESTDIR = $$shell_path($$OUT_PWD/../bin/themes)


    !exists($$THEMESDESTDIR) {
        system( $$QMAKE_MKDIR $$shell_path($$THEMESDESTDIR) )
    }

    QMAKE_PRE_LINK = $$shell_path($$[QT_INSTALL_BINS]/rcc) -o $$shell_path($$THEMES_FILE) $$shell_path($$THEMES_PATH/themes.qrc) \
                     $$escape_expand(\\n\\t) \
                     $$QMAKE_COPY $$shell_path($$THEMES_FILE) $$THEMESDESTDIR
}
