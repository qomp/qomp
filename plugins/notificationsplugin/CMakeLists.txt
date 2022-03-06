set( PLUGIN notificationsplugin )
cmake_minimum_required( VERSION 3.7.0 )

include(pluginsCommon)

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

set(QT_LIB_LIST Widgets)
find_qt_package("${QT_LIB_LIST}" REQUIRED )
add_qt_library(QT_DEPLIBS "${QT_LIB_LIST}")

if( LINUX )
	add_definitions( -DHAVE_X11 )
	list( APPEND notificationsplugin_HDRS
		dbusnotifier.h
	)
	list( APPEND notificationsplugin_SRCS
		dbusnotifier.cpp
	)
	find_qt_package(DBus REQUIRED)
	include_directories( ${${QT_PKG_VER}DBus_INCLUDES} )
	add_definitions(
		${Qt${QT_PKG_VER}DBus_DEFINITIONS}
		-DHAVE_DBUS
	)
	add_qt_library(QT_DEPLIBS DBus)
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

install(
	TARGETS
	${PLUGIN}
	LIBRARY DESTINATION ${PLUGINS_INSTALL_PATH}
	RUNTIME DESTINATION ${PLUGINS_INSTALL_PATH}
)
