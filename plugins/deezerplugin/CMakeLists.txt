set ( PLUGIN deezerplugin )
cmake_minimum_required( VERSION 3.1.0 )
set ( CMAKE_AUTOMOC ON )
add_definitions ( -Wall )
set ( deezerplugin_HDRS
	${PLUGIN}.h
	deezergettunesdlg.h
	deezerplugindefines.h
	deezerauth.h
)

set ( deezerplugin_SRCS
	${PLUGIN}.cpp
	deezergettunesdlg.cpp
	deezerauth.cpp
)

find_package ( Qt5Widgets REQUIRED )
find_package ( Qt5Xml REQUIRED )
include_directories (
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${Qt5Widgets_INCLUDES}
	${Qt5Xml_INCLUDES}
	../../libqomp/include
)

add_definitions(
	${Qt5Widgets_DEFINITIONS}
	${Qt5Xml_DEFINITIONS}
)
set (QT_DEPLIBS
	Qt5::Widgets
	${Qt5Xml_LIBRARIES}
)

add_library ( ${${PLUGIN}}
	SHARED
	${deezerplugin_SRCS}
	${deezerplugin_HDRS}
)
target_link_libraries ( ${${PLUGIN}}
	${QT_DEPLIBS}
)
add_dependencies( ${PLUGIN} libqomp qomp )
if ( ${WIN32} )
	install (
		TARGETS
		${PLUGIN}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}
		RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins
	)
endif ()

if ( ${UNIX} )
	install (
		TARGETS
		${PLUGIN}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}/share/qomp/plugins
	)
endif()
