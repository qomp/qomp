# Changelog

## 1.5 - 2021-09-23
  * fixed Yandex.Music plugin
  + Yandex.Music plugin now supports authentication (via settings)
  * updated Qt libs to 5.12.11 (Android)
  * updated Qt libs to 5.15.2 (OS X, Windows)
  * other minor fixes

## 1.4 - 2018-11-14
  + added new Poiskm plugin (poiskm.me)
  - dropped Prostopleer plugin (site is unavailable)
  + added integration with some google services (analytics, firebase)
  * updated Notifications plugin (added 'Album art' support)
  * optimizations for MPRIS plugin
  * android: pausing playback on audio focus loss
  * updated Qt libs to 5.9.7 (Android)
  * updated Qt libs to 5.11.2 (OS X, Windows)
  * other minor fixes

## 1.3.1 - 2017-12-19
  + added support for native linux icon themes (new linux_native theme)
  * updated Myzuka plugin url (myzuka.club)
  * fixed suggestions on android
  * fixed shuffle play on android
  * updated Qt libs to 5.9.3 (OS X, Windows, Android)
  * other minor fixes

## 1.3 - 2017-11-01
  + added cuteRadio plugin (Windows and Linux only). http://marxoft.co.uk/projects/cuteradio
  * improved playlists support
  + added 'Open at browser' action to tune popup menu
  * improved tooltips
  * fixes for Myzuka.ru plugin. For now links to songs do not become obsolete
  + added headset media button support (Android only)
  * Filesystem plugin improved
  * added genre to Yandex.music plugin
  * update Qt libs to 5.9.2 (OS X, Windows, Android). For now minimal supported versions are: OS X 10.10, Windows 7, Android 4.1
  * other minor fixes and improvements



## 1.2.1 - 2017-06-30
  * fix Myzuka.ru plugin (fixed url)
  * improvement for Mpris plugin (Linux only)
  * other minor fixes
  + add Italian translation (tnx to @symbianflo)
  + add Czech translation (tnx to Pavel Fric)
  * update Qt libs to 5.9.0 (OS X, Windows).  For now OS X 10.10 is minimal supported version.



## 1.2 - 2017-04-14
  + add m3u playlists support
  + add drag-n-drop files into playlist support
  + add thumbnail toolbar (Windows only)
  * improved Filesystem Plugin
  * fix scaling UI on Android
  + add Cover art (album art) support. If picture present it will displayed at song tooltip
  + add Track info dialog (for Android only)
  + improved themes: add frameless windows support (desktop only)
  * improved MPRIS Plugin (Linux only)
  + add multiselect support at tune search dialogs, add context menu at tune search dialogs
  * update Qt libs to 5.8.0 (OS X, Windows). For now OS X 10.9 and Windows 7 are minimal supported OS versions
  * update TagLib to 1.11.1 (Windows, OS X)
  * various fixes and improvements


## 1.1.1 - 2016-12-15
  + add mediakeys support
  * fixed Shuffle when click "Next" button
  + add Windows TaskBar progress support
  + add hotkey for delete tunes from playlist
  + add context menu on group of tunes at playlist
  * improve tune downloader
  * fix Prostopleer URL (now http://pleer.net)
  * fix audio device selection (Windows only)
  * improved Yandex.Music plugin
  * updated MPRIS plugin. Added player control via dbus (Linux only)
  * other fixes
  * update Qt libs to 5.6.2 (Windows, Android)
  * update Qt libs to 5.7.0 (OS X)



## 1.1 - 2016-07-05
 + add CUE SHEET support (*.cue files)
 + add "Open With..." dialog support
 + add "Allow one copy" option
 + add last position remembering (option at settings)
 + add compact playlist support
 * major improvement for Themes: now themed icons are supported 
 + add new green theme
 + add Launchpad support ( https://launchpad.net/~qomp/+archive/ubuntu/ppa )
 * update Qt libs to 5.6.1 (Windows, Android, OS X)
 * update TagLib to 1.11 (Windows, Android, OS X)
 - dropped Qt4 support
 * various fixes and improvements



## 1.0 - 2016-04-18
 + add CAPTCHA support for Yandex.Music plugin
 * update Myzuka.ru url (now Myzuka.fm)
 + add shuffle support
 * update Qt libs to 5.6.0 (Windows, Android, OS X)



## 0.9 beta - 2015-07-10
 * fixes for yandex.music plugin
 + add "repeat last search" option
 * fix dock click detection on OS X
 + add ability select several tunes with shift key pressed at tunes search dialog (Desktop only)
 * various fixes
 * update Qt libs to 5.5.0 (Windows, Android, OS X)



## 0.8.1 beta - 2015-03-30
 * fixes for myzuka.ru and yandex.music plugins



## 0.8 beta - 2015-03-15
 * change myzuka.ru link to new site location (myzuka.org)
 * update Qt libs to 5.4.1 (Windows, Android, OS X)
 * windows package now installs some third party codecs



## 0.7.1 beta - 2014-11-26
 * adapt myzuka.ru plugin for new site design
 * some minor fixes



## 0.7 beta - 2014-10-31
 + add QML interface for android
 + add full android support
 + add Notifications plugin
 + add shortcuts support
 + new black theme
 * better OS X integration
 * Standard places for resources on Linux
 * various UI fixes
 * fixes for plugins
 * update translations
 * various fixes
 * new homepage at http://qomp.sourceforge.net/



## 0.6 beta - 2014-08-23
 * update Yandex.Music plugin
 * update Myzuka.ru plugin
 * update filesystem plugin: add resolving tags on loading tunes with taglib
 * restore OS X builds
 + add cmake Win32 support
 - get rid of metadata resolving for online tunes
 * various fixes and improvements



## 0.5 beta - 2014-05-02
 * fixes for translations system
 * improvements for android builds
 * fix myzuka.ru plugin
 * update filesystem plugin
 + add qomp shared library, move to dynamic plugins loading
 * update russian translation
 * improvements for plugins system
 + add cmake build support
 * some UI improvements
 * fix MPRIS plugin
 * fix filesystem plugin
 + add theme manager



## 0.4 beta - 2014-02-27
 + add android compilling support
 * fix myzuka.ru plugin
 * plugins improvements
 * improve LastFM plugin
 * update russian translation
 * add css theming support
 * some UI fixes
 * fixes for MPRIS plugin
 * some fixes and optiomizations



## 0.3 beta - 2013-09-09
 * fix prostopleer plugins url after thair moving to another hosting
 + add drag-and-drop at playlist
 + add LastFm scrobbling
 + add tune-to-file plugin
 + add MPRIS plugin
 + add repeat all button
 * fix yandex.music plugin
 * update russian translation
 * different fixes and improvements



## 0.2 beta - 2013-06-15
 + add tray icon
 + better linux integration
 + add ability load and save playlist
 + many UI improvements
 + plugin system improvements
 + add ability save tunes
 * fixes for prostopleer plugin
 + add myzuka.ru plugin
 + add tag resolving
 + add search suggestions
 + add Qt5 support
 + add "About qomp" dialog
 + add localizations support
 + add OS X support
 + add Yandex.Music plugin



## 0.1 beta - 2013-03-21
 * Start work
