set( PLUGIN prostopleerplugin )
cmake_minimum_required( VERSION 3.1.0 )

include(../../cmake/modules/policyRules.cmake)

set( CMAKE_AUTOMOC ON )
add_definitions( -DQT_PLUGIN )
if( UNIX AND NOT (APPLE OR CYGWIN) )
	set(LINUX ON)
endif()
set( prostopleerplugin_HDRS
	${PLUGIN}.h
	prostopleerplugingettunesdialog.h
	prostopleerpluginsettings.h
	prostopleerplugindefines.h
	prostopleercontroller.h
)
set( prostopleerplugin_SRCS
	${PLUGIN}.cpp
	prostopleerplugingettunesdialog.cpp
	prostopleerpluginsettings.cpp
	prostopleercontroller.cpp
)
set( prostopleerplugin_UIS
	prostopleerpluginresultswidget.ui
	prostopleerpluginsettings.ui
)
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/include
)

find_package( Qt5 COMPONENTS Widgets Network REQUIRED )
qt5_wrap_ui( UIS ${prostopleerplugin_UIS} )
set( QT_DEPLIBS
	Qt5::Widgets
	Qt5::Network
)

add_library(
	${PLUGIN}
	MODULE
	${prostopleerplugin_SRCS}
	${UIS}
)
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
