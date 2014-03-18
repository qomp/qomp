project ( filesystemplugin )
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
		../../libqomp/include
	)
	set ( filesystemplugin_HDRS
		filesystemplugin.h
	)
	set ( filesystemplugin_SRCS
		filesystemplugin.cpp
	)
	add_library (
		${PROJECT_NAME}
		SHARED
		${filesystemplugin_SRCS}
	)
	target_link_libraries (
		${PROJECT_NAME}
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
	)
	install (
		TARGETS
		${PROJECT_NAME}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}/share/qomp/plugins
	)
endif( UNIX )