set( PLUGIN yandexmusicplugin )
cmake_minimum_required( VERSION 3.7.0 )

include(../../cmake/modules/policyRules.cmake)

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
	yandexmusiccaptcha.h
	yandexmusicoauth.h
	yandexmusicsettings.h
)
set( yandexmusicplugin_SRCS
	${PLUGIN}.cpp
	yandexmusicgettunsdlg.cpp
	yandexmusicurlresolvestrategy.cpp
	yandexmusiccontroller.cpp
	yandexmusiccaptcha.cpp
	yandexmusicoauth.cpp
	yandexmusicsettings.cpp
)

set( yandexmusicplugin_UIS
	yandexmusicsettings.ui
)

include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/include
)

find_package( Qt5 COMPONENTS Widgets Network Xml REQUIRED )
set( QT_DEPLIBS
	Qt5::Widgets
	Qt5::Network
	Qt5::Xml
)

qt5_wrap_ui( UIS ${yandexmusicplugin_UIS} )

add_library( ${PLUGIN} MODULE ${yandexmusicplugin_SRCS} ${UIS} )
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
