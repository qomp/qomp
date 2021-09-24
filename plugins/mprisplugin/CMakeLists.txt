set( PLUGIN mprisplugin )
cmake_minimum_required( VERSION 3.7.0 )

include(../../cmake/modules/policyRules.cmake)

if( UNIX AND NOT (APPLE OR CYGWIN) )
	set(LINUX ON)
endif()
if( LINUX )
	set( CMAKE_AUTOMOC TRUE )
	add_definitions( -DQT_PLUGIN -DHAVE_DBUS )
	set( mprisplugin_HDRS
		mprisadapter.h
		mpriscontroller.h
		rootadapter.h
		${PLUGIN}.h
	)
	set( mprisplugin_SRCS
		mprisadapter.cpp
		mpriscontroller.cpp
		rootadapter.cpp
		${PLUGIN}.cpp
	)
	include_directories(
		${CMAKE_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		${PROJECT_BINARY_DIR}/include
	)
	find_package( Qt5 COMPONENTS Widgets DBus REQUIRED )
	set( QT_DEPLIBS
		Qt5::Widgets
		Qt5::DBus
	)
	add_library(
		${PLUGIN}
		MODULE
		${mprisplugin_SRCS}
	)
	target_link_libraries(
		${PLUGIN}
		${QT_DEPLIBS}
		libqomp
	)
	add_dependencies( ${PLUGIN} libqomp qomp )
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
