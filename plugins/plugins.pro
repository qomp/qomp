TEMPLATE = subdirs

sub_urlplugin.subdir = urlplugin
sub_lastfmplugin.subdir = lastfmplugin
sub_myzukaruplugin.subdir = myzukaruplugin
sub_notificationsplugin.subdir = notificationsplugin
sub_yandexmusicplugin.subdir = yandexmusicplugin
sub_filesystemplugin.subdir = filesystemplugin
sub_poiskmplugin.subdir = poiskmplugin

SUBDIRS +=  \
        sub_myzukaruplugin \
        sub_lastfmplugin \
        sub_urlplugin \
        sub_notificationsplugin \
        sub_yandexmusicplugin \
        sub_filesystemplugin \
        sub_poiskmplugin

#sub_prostopleerplugin.subdir = prostopleerplugin
#SUBDIRS += sub_prostopleerplugin

#sub_deezerplugin.subdir = deezerplugin
#SUBDIRS += sub_deezerplugin

!android {   
    sub_tunetofileplugin.subdir = tunetofileplugin
    SUBDIRS += sub_tunetofileplugin
}

unix:!mac:!android: {
    sub_mprisplugin.subdir = mprisplugin
    SUBDIRS += sub_mprisplugin
}

win32|unix:!mac:!android: {
    sub_cuteradioplugin.subdir = cuteradioplugin
    SUBDIRS += sub_cuteradioplugin
}
