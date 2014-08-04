set ( PLUGIN tunetofileplugin )
cmake_minimum_required( VERSION 2.8.11 )
set ( CMAKE_AUTOMOC ON )
add_definitions ( -Wall -DQT_PLUGIN )
set ( tunetofileplugin_HDRS
	tune2fileplugin.h
)
set ( tunetofileplugin_SRCS
	tune2fileplugin.cpp
)
set ( tunetofileplugin_UIS
	tunetofilesettings.ui
)

find_package ( Qt5Widgets QUIET )
if ( ${Qt5Widgets_FOUND} )
set (USE_QT5 TRUE)
else ( ${Qt5Widgets_FOUND} )
set (USE_QT5 FALSE)
endif ( ${Qt5Widgets_FOUND} )

if ( ${USE_QT5} )
	find_package ( Qt5Widgets REQUIRED )
	include_directories (
		${CMAKE_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		${Qt5Widgets_INCLUDES}
		../../libqomp/include
	)
	set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
	add_definitions(
		${Qt5Widgets_DEFINITIONS}
		-DHAVE_QT5
	)
	qt5_wrap_ui(UIS ${tunetofileplugin_UIS})
	set (QT_DEPLIBS
		Qt5::Widgets
	)
else ( ${USE_QT5} )
	find_package ( Qt4 REQUIRED )
	include ( ${QT_USE_FILE} )
	include_directories (
		${CMAKE_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		../../libqomp/include
	)
	QT4_WRAP_UI(UIS ${tunetofileplugin_UIS})
	set (QT_DEPLIBS
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
	)
endif ( ${USE_QT5} )

add_library (
	${PLUGIN}
	SHARED
	${tunetofileplugin_SRCS}
	${UIS}
)
target_link_libraries (
	${PLUGIN}
	${QT_DEPLIBS}
	libqomp
)
add_dependencies(${PLUGIN} libqomp qomp )
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
