set( PLUGIN yandexmusicplugin )
cmake_minimum_required( VERSION 2.8.11 )
if( CMAKE_VERSION VERSION_GREATER 3.0.0 )
	cmake_policy(SET CMP0028 OLD)
endif( CMAKE_VERSION VERSION_GREATER 3.0.0 )
set( CMAKE_AUTOMOC ON )
add_definitions( -Wall -DQT_PLUGIN )
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
	${PROJECT_BINARY_DIR}/libqomp/include
)
if( ${USE_QT5} )
	find_package( Qt5Widgets REQUIRED )
	find_package( Qt5Network REQUIRED )
	include_directories(
		${Qt5Widgets_INCLUDES}
		${Qt5Network_INCLUDES}
		${Qt5Xml_INCLUDES}
	)
	set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}" )
	add_definitions(
		${Qt5Widgets_DEFINITIONS}
		${Qt5Network_DEFINITIONS}
		-DHAVE_QT5
	)
	qt5_wrap_ui( UIS ${lastfmplugin_UIS} )
	set( QT_DEPLIBS
		Qt5::Widgets
		Qt5::Network
	)
else( ${USE_QT5} )
	find_package( Qt4 REQUIRED )
	find_package( QJSON REQUIRED )
	message("-- Found QJSON: ${QJSON_LIBRARY}")
	include( ${QT_USE_FILE} )
	include_directories(
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		${QT_QTNETWORK_INCLUDE_DIR}
		${QJSON_INCLUDE_DIR}
	)
	QT4_WRAP_UI( UIS ${lastfmplugin_UIS} )
	set( QT_DEPLIBS
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTNETWORK_LIBRARY}
		${QJSON_LIBRARY}
	)
	add_definitions(
		${QJSON_DEFINITIONS}
	)
endif( ${USE_QT5} )

add_library( ${PLUGIN} SHARED ${yandexmusicplugin_SRCS} )
target_link_libraries(
	${PLUGIN}
	${QT_DEPLIBS}
	libqomp
)
add_dependencies( ${PLUGIN} libqomp qomp )
if( ${WIN32} )
	install(
		TARGETS
		${PLUGIN}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}
		RUNTIME DESTINATION
		${CMAKE_INSTALL_PREFIX}/plugins
	)
endif( ${WIN32} )

if( ${UNIX} )
	install(
		TARGETS
		${PLUGIN}
		LIBRARY
		DESTINATION
		${LIB_INSTALL_DIR}/qomp/plugins
	)
endif( ${UNIX} )
