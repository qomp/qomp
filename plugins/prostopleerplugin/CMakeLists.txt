set( PLUGIN prostopleerplugin )
cmake_minimum_required( VERSION 2.8.12 )
if( CMAKE_VERSION VERSION_GREATER 3.0.0 )
	cmake_policy(SET CMP0028 OLD)
endif()
set( CMAKE_AUTOMOC ON )
add_definitions( -Wall -DQT_PLUGIN )
set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}" )
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
	${PROJECT_BINARY_DIR}/libqomp/include
)
if( USE_QT5 )
	find_package( Qt5 REQUIRED Widgets Network )
	add_definitions(
		-DHAVE_QT5
	)
	qt5_wrap_ui( UIS ${prostopleerplugin_UIS} )
	set( QT_DEPLIBS
		Qt5::Widgets
		Qt5::Network
	)
else()
	find_package( Qt4 REQUIRED )
	include( ${QT_USE_FILE} )
	include_directories(
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		${QT_QTNETWORK_INCLUDE_DIR}
	)
	QT4_WRAP_UI( UIS ${prostopleerplugin_UIS} )
	set( QT_DEPLIBS
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTNETWORK_LIBRARY}
	)
endif()

add_library(
	${PLUGIN}
	SHARED
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
