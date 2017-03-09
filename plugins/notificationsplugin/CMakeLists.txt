set( PLUGIN notificationsplugin )
cmake_minimum_required( VERSION 2.8.12 )

set( CMAKE_AUTOMOC ON )
add_definitions( -DQT_PLUGIN )
if( UNIX AND NOT (APPLE OR CYGWIN) )
	set(LINUX ON)
endif()
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/include
	)
set( notificationsplugin_HDRS
	${PLUGIN}.h
	)
set( notificationsplugin_SRCS
	${PLUGIN}.cpp
	)
if( LINUX )
	add_definitions( -DHAVE_X11 )
	set( notificationsplugin_HDRS
		${notificationsplugin_HDRS}
		dbusnotifier.h
		)
	set( notificationsplugin_SRCS
		${notificationsplugin_SRCS}
		dbusnotifier.cpp
		)
endif()

find_package( Qt5Widgets REQUIRED )
add_definitions(
	-DHAVE_QT5
	)
set( QT_DEPLIBS
	Qt5::Widgets
	)
if( LINUX )
	find_package( Qt5DBus REQUIRED )
	include_directories( ${Qt5DBus_INCLUDES} )
	add_definitions(
		${Qt5DBus_DEFINITIONS}
		-DHAVE_DBUS
		)
	set( QT_DEPLIBS
		${QT_DEPLIBS}
		Qt5::DBus
		)
endif()

add_library(
	${PLUGIN}
	SHARED
	${notificationsplugin_SRCS}
	)
target_link_libraries(
	${PLUGIN}
	${QT_DEPLIBS}
	libqomp
	)
add_dependencies(${PLUGIN} libqomp qomp )

if( WIN32 )
	install (
		TARGETS
		${PLUGIN}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}/plugins
		RUNTIME DESTINATION
		${CMAKE_INSTALL_PREFIX}/plugins
		)
endif()
if( LINUX )
	install(
		TARGETS
		${PLUGIN}
		LIBRARY
		DESTINATION
		${LIB_INSTALL_DIR}/qomp/plugins
		RUNTIME DESTINATION
		${LIB_INSTALL_DIR}/qomp/plugins
		)
endif()
