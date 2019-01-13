set( PLUGIN notificationsplugin )
cmake_minimum_required( VERSION 3.1.0 )

if( POLICY CMP0071 )
        cmake_policy(SET CMP0071 OLD)
endif()

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
	list( APPEND notificationsplugin_HDRS
		dbusnotifier.h
	)
	list( APPEND notificationsplugin_SRCS
		dbusnotifier.cpp
	)
endif()

find_package( Qt5 COMPONENTS Widgets REQUIRED )
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
	list( APPEND QT_DEPLIBS
		Qt5::DBus
	)
endif()

add_library(
	${PLUGIN}
	MODULE
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
