TEMPLATE = subdirs

android {
sub_prostopleerplugin.subdir = prostopleerplugin
SUBDIRS +=  sub_prostopleerplugin
}
else {
sub_prostopleerplugin.subdir = prostopleerplugin
sub_filesystemplugin.subdir = filesystemplugin
sub_urlplugin.subdir = urlplugin
sub_myzukaruplugin.subdir = myzukaruplugin
sub_yandexmusicplugin.subdir = yandexmusicplugin
sub_lastfmplugin.subdir = lastfmplugin
sub_tunetofileplugin.subdir = tunetofileplugin

SUBDIRS += sub_prostopleerplugin \
        sub_filesystemplugin \
        sub_urlplugin \
        sub_myzukaruplugin \
        sub_yandexmusicplugin \
        sub_lastfmplugin \
        sub_tunetofileplugin
}

unix:!mac:!android: {
    sub_mprisplugin.subdir = mprisplugin
    SUBDIRS += sub_mprisplugin
}
