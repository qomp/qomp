set( PLUGIN lastfmplugin )
cmake_minimum_required( VERSION 3.1.0 )

set( CMAKE_AUTOMOC ON )
add_definitions( -DQT_PLUGIN )
if( UNIX AND NOT (APPLE OR CYGWIN) )
	set(LINUX ON)
endif()
set( lastfmplugin_HDRS
	${PLUGIN}.h
	lastfmsettings.h
	lastfmdefines.h
	lastfmauthdlg.h
)
set( lastfmplugin_SRCS
	${PLUGIN}.cpp
	lastfmsettings.cpp
	lastfmauthdlg.cpp
)
set( lastfmplugin_UIS
	lastfmsettings.ui
	lastfmauthdlg.ui
)
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/include
)

find_package( Qt5 REQUIRED Widgets Network Xml)
add_definitions(
	-DHAVE_QT5
)
qt5_wrap_ui(UIS ${lastfmplugin_UIS})
set( QT_DEPLIBS
	Qt5::Widgets
	Qt5::Network
	Qt5::Xml
)

add_library( ${PLUGIN} MODULE ${lastfmplugin_SRCS} ${UIS} )
target_link_libraries(
	${PLUGIN}
	${QT_DEPLIBS}
	libqomp
)
add_dependencies( ${PLUGIN} libqomp qomp )
if( WIN32 )
	install(
		TARGETS
		${PLUGIN}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}/plugins
		RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins
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
