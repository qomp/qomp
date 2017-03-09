set( PLUGIN yandexmusicplugin )
cmake_minimum_required( VERSION 2.8.12 )

set( CMAKE_AUTOMOC ON )
add_definitions( -DQT_PLUGIN )
if( UNIX AND NOT (APPLE OR CYGWIN) )
	set(LINUX ON)
endif()
set( yandexmusicplugin_HDRS
	${PLUGIN}.h
	yandexmusicgettunsdlg.h
	yandexmusicurlresolvestrategy.h
	yandexmusiccontroller.h
)
set( yandexmusicplugin_SRCS
	${PLUGIN}.cpp
	yandexmusicgettunsdlg.cpp
	yandexmusicurlresolvestrategy.cpp
	yandexmusiccontroller.cpp
)
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/include
)
set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}" )

find_package( Qt5 REQUIRED Widgets Network )
add_definitions(
	-DHAVE_QT5
)
qt5_wrap_ui( UIS ${lastfmplugin_UIS} )
set( QT_DEPLIBS
	Qt5::Widgets
	Qt5::Network
)

add_library( ${PLUGIN} SHARED ${yandexmusicplugin_SRCS} )
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
