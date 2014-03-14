project ( lastfmplugin )
cmake_minimum_required( VERSION 2.8.6 )
if ( UNIX )
	set ( CMAKE_BUILD_TYPE Release )
	set ( CMAKE_AUTOMOC TRUE )
	add_definitions ( -Wall )
	find_package ( Qt4 REQUIRED )
	include ( ${QT_USE_FILE} )
	include_directories (
		${CMAKE_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		${QT_QTXML_INCLUDE_DIR}
		${QT_QTNETWORK_INCLUDE_DIR}
		../../libqomp/include
	)
	set ( lastfmplugin_HDRS
		lastfmplugin.h
		lastfmsettings.h
		lastfmdefines.h
		lastfmauthdlg.h
	)
	set ( lastfmplugin_SRCS
		lastfmplugin.cpp
		lastfmsettings.cpp
		lastfmauthdlg.cpp
	)
	set ( lastfmplugin_UIS
		lastfmsettings.ui
		lastfmauthdlg.ui
	)
	QT4_WRAP_UI(UIS ${lastfmplugin_UIS})
	add_library ( ${PROJECT_NAME} SHARED ${lastfmplugin_SRCS} ${UIS} )
	target_link_libraries (
		${PROJECT_NAME}
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTXML_LIBRARY}
		${QT_QTNETWORK_LIBRARY}
	)
	install (
		TARGETS
		${PROJECT_NAME}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}/share/qomp/plugins
	)
endif( UNIX )