set ( PLUGIN yandexmusicplugin )
cmake_minimum_required( VERSION 2.8.6 )
if ( ${UNIX} )
	set ( CMAKE_AUTOMOC TRUE )
	add_definitions ( -Wall )
	find_package ( Qt4 REQUIRED )
	include ( ${QT_USE_FILE} )
	include_directories (
		${CMAKE_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		${QT_QTNETWORK_INCLUDE_DIR}
		../../libqomp/include
	)
	set ( yandexmusicplugin_HDRS
		${PLUGIN}.h
		yandexmusicgettunsdlg.h
		yandexmusicurlresolvestrategy.h
		yandexmusiccontroller.h
	)
	set ( yandexmusicplugin_SRCS
		${PLUGIN}.cpp
		yandexmusicgettunsdlg.cpp
		yandexmusicurlresolvestrategy.cpp
		yandexmusiccontroller.cpp
	)
	add_library ( ${PLUGIN} SHARED ${yandexmusicplugin_SRCS} )
	target_link_libraries (
		${PLUGIN}
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTNETWORK_LIBRARY}
	)
	install (
		TARGETS
		${PLUGIN}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}/share/qomp/plugins
	)
endif( ${UNIX} )