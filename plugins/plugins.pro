TEMPLATE = subdirs

sub_prostopleerplugin.subdir = prostopleerplugin
sub_urlplugin.subdir = urlplugin
sub_lastfmplugin.subdir = lastfmplugin
sub_myzukaruplugin.subdir = myzukaruplugin
sub_notificationsplugin.subdir = notificationsplugin
sub_yandexmusicplugin.subdir = yandexmusicplugin
sub_filesystemplugin.subdir = filesystemplugin

SUBDIRS +=  sub_prostopleerplugin \
        sub_myzukaruplugin \
        sub_lastfmplugin \
        sub_urlplugin \
        sub_notificationsplugin \
        sub_yandexmusicplugin \
        sub_filesystemplugin

!android {   
    sub_tunetofileplugin.subdir = tunetofileplugin
    SUBDIRS += sub_tunetofileplugin
}

unix:!mac:!android: {
    sub_mprisplugin.subdir = mprisplugin
    SUBDIRS += sub_mprisplugin
}
