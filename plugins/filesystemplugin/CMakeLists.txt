set ( PLUGIN filesystemplugin )
cmake_minimum_required( VERSION 2.8.11)
set ( CMAKE_AUTOMOC ON )
add_definitions ( -Wall -DQT_PLUGIN )
set ( filesystemplugin_HDRS
	${PLUGIN}.h
)
set ( filesystemplugin_SRCS
	${PLUGIN}.cpp
)
find_package ( TagLib REQUIRED )
include_directories (
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${TAGLIB_INCLUDE_DIR}
	../../libqomp/include
)
if ( ${USE_QT5} )
	find_package ( Qt5Widgets REQUIRED )
	include_directories (
		${Qt5Widgets_INCLUDES}
		${TAGLIB_INCLUDE_DIR}
	)
	set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
	add_definitions(
		${Qt5Widgets_DEFINITIONS}
		-DHAVE_QT5
	)
	set (QT_DEPLIBS
		Qt5::Widgets
	)
else ( ${USE_QT5} )
	find_package ( Qt4 REQUIRED )
	include ( ${QT_USE_FILE} )
	include_directories (
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		${TAGLIB_INCLUDE_DIR}
	)
	set (QT_DEPLIBS
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
	)
endif ( ${USE_QT5} )

add_library (
	${PLUGIN}
	SHARED
	${filesystemplugin_SRCS}
)
target_link_libraries (
	${PLUGIN}
	${TAGLIB_LIBRARY}
	${QT_DEPLIBS}
	libqomp
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
endif ( ${WIN32} )

if ( ${UNIX} )
	install (
		TARGETS
		${PLUGIN}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}/share/qomp/plugins
	)
endif( ${UNIX} )
