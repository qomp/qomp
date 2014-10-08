set( PLUGIN notificationsplugin )
cmake_minimum_required( VERSION 2.8.11 )
if( CMAKE_VERSION VERSION_GREATER 3.0.2 )
	cmake_policy(SET CMP0028 OLD)
endif( CMAKE_VERSION VERSION_GREATER 3.0.2 )
set( CMAKE_AUTOMOC ON )
add_definitions( -Wall -DQT_PLUGIN )
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	../../libqomp/include
)
set( notificationsplugin_HDRS
	${PLUGIN}.h
)
set( notificationsplugin_SRCS
	${PLUGIN}.cpp
)
if( UNIX AND NOT (APPLE OR CYGWIN) )
	add_definitions( -DHAVE_X11 )
	set( notificationsplugin_HDRS
		${notificationsplugin_HDRS}
		dbusnotifier.h
	)
	set( notificationsplugin_SRCS
		${notificationsplugin_SRCS}
		dbusnotifier.cpp
	)
endif( UNIX AND NOT (APPLE OR CYGWIN) )
if( ${USE_QT5} )
	find_package( Qt5Widgets REQUIRED )
	include_directories(
		${Qt5Widgets_INCLUDES}
	)
	set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
	add_definitions(
		${Qt5Widgets_DEFINITIONS}
		-DHAVE_QT5
	)
	set( QT_DEPLIBS
		Qt5::Widgets
	)
	if( UNIX AND NOT (APPLE OR CYGWIN) )
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
	endif( UNIX AND NOT (APPLE OR CYGWIN) )
else( ${USE_QT5} )
	find_package( Qt4 REQUIRED )
	include( ${QT_USE_FILE} )
	include_directories(
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
	)
	set( QT_DEPLIBS
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
	)
	if( UNIX AND NOT (APPLE OR CYGWIN) )
		include_directories( ${QT_QTDBUS_INCLUDE_DIR} )
		set( QT_DEPLIBS
			${QT_DEPLIBS}
			${QT_QTDBUS_LIBRARY}
		)
	endif( UNIX AND NOT (APPLE OR CYGWIN) )
endif( ${USE_QT5} )
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

if( ${WIN32} )
	install (
		TARGETS
		${PLUGIN}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}
		RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins
	)
endif( ${WIN32} )
if( UNIX AND NOT (APPLE OR CYGWIN) )
	install(
		TARGETS
		${PLUGIN}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}/share/qomp/plugins
	)
endif( UNIX AND NOT (APPLE OR CYGWIN) )
