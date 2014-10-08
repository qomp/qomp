set ( PLUGIN mprisplugin )
cmake_minimum_required( VERSION 2.8.11 )
if( CMAKE_VERSION VERSION_GREATER 3.0.0 )
	cmake_policy(SET CMP0028 OLD)
endif( CMAKE_VERSION VERSION_GREATER 3.0.0 )
if ( ${UNIX} )
	set ( CMAKE_AUTOMOC TRUE )
	add_definitions ( -Wall -DQT_PLUGIN -DHAVE_DBUS )
	set ( mprisplugin_HDRS
		mprisadapter.h
		mpriscontroller.h
		${PLUGIN}.h
	)
	set ( mprisplugin_SRCS
		mprisadapter.cpp
		mpriscontroller.cpp
		${PLUGIN}.cpp
	)
	include_directories (
		${CMAKE_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		../../libqomp/include
	)
	if ( ${USE_QT5} )
		find_package ( Qt5Widgets REQUIRED )
		find_package ( Qt5DBus REQUIRED )
		include_directories (
			${Qt5Widgets_INCLUDES}
			${Qt5DBus_INCLUDES}
		)
		set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
		add_definitions(
			${Qt5Widgets_DEFINITIONS}
			${Qt5DBus_DEFINITIONS}
			-DHAVE_QT5
		)
		set (QT_DEPLIBS
			Qt5::Widgets
			Qt5::DBus
		)
	else ( ${USE_QT5} )
		find_package ( Qt4 REQUIRED )
		include ( ${QT_USE_FILE} )
		include_directories (
			${QT_QTCORE_INCLUDE_DIR}
			${QT_QTGUI_INCLUDE_DIR}
			${QT_QTDBUS_INCLUDE_DIR}
		)
		set (QT_DEPLIBS
			${QT_QTCORE_LIBRARY}
			${QT_QTGUI_LIBRARY}
			${QT_QTDBUS_LIBRARY}
		)
	endif ( ${USE_QT5} )
	add_library (
		${PLUGIN}
		SHARED
		${mprisplugin_SRCS}
	)
	target_link_libraries (
		${PLUGIN}
		${QT_DEPLIBS}

	)
	add_dependencies(${PLUGIN} libqomp qomp )
	install (
		TARGETS
		${PLUGIN}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}/share/qomp/plugins
	)
endif( ${UNIX} )
