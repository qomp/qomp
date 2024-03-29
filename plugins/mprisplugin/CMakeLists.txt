set( PLUGIN mprisplugin )
cmake_minimum_required( VERSION 3.7.0 )

if( LINUX )
	include(pluginsCommon)
	add_definitions( -DHAVE_DBUS )
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

	set(QT_LIB_LIST Widgets DBus)
	find_qt_package("${QT_LIB_LIST}" REQUIRED )
	add_qt_library(QT_DEPLIBS "${QT_LIB_LIST}")

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
		LIBRARY DESTINATION ${PLUGINS_INSTALL_PATH}
		RUNTIME DESTINATION ${PLUGINS_INSTALL_PATH}
	)
endif()
