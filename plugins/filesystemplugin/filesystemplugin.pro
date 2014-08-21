include($$PWD/../plugins.pri)

SOURCES += $$PWD/filesystemplugin.cpp
HEADERS  += $$PWD/filesystemplugin.h

mac {
    INCLUDEPATH += /Library/Frameworks/tag.framework/Headers
    LIBS += -framework tag
}
else {
    LIBS += -ltag
}

OTHER_FILES += \
    $$PWD/metadata.json
