set( QOMP_LIB libqomp )
cmake_minimum_required( VERSION 2.8.12 )
if( CMAKE_VERSION VERSION_GREATER 3.0.0 )
	cmake_policy(SET CMP0028 OLD)
endif()
set( CMAKE_AUTOMOC ON )
add_definitions( -Wall -DLIBQOMP_LIBRARY )
set( QOMP_LIB_VERSION_MAJOR 1 )
set( QOMP_LIB_VERSION_MINOR 0 )
set( QOMP_LIB_VERSION_PATCH 0 )
set( QOMP_LIB_VERSION_STRING
	${QOMP_LIB_VERSION_MAJOR}.${QOMP_LIB_VERSION_MINOR}.${QOMP_LIB_VERSION_PATCH}
)
set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}" )
if( UNIX AND NOT (APPLE OR CYGWIN) )
	set(LINUX ON)
endif()
set( plugins_HDRS
	src/plugins/qompplugingettunesdlg.h
	src/plugins/qompplugintreeview.h
	src/plugins/qompplugintreemodel.h
	src/plugins/qompplugincaptchadialog.h
	src/plugins/qompplugincontroller.h
	src/plugins/qomppluginaction.h
)
set( plugins_SRCS
	src/plugins/qompplugingettunesdlg.cpp
	src/plugins/qompplugintreeview.cpp
	src/plugins/qompplugintreemodel.cpp
	src/plugins/qompplugintypes.cpp
	src/plugins/qompplugincaptchadialog.cpp
	src/plugins/qompplugincontroller.cpp
	src/plugins/qomppluginaction.cpp
)
set( options_HDRS
	src/options/qompoptionspage.h
)
set( libqomp_HDRS
	src/options.h
	src/pluginhost.h
	src/pluginmanager.h
	src/qompactionslist.h
	src/qompbusylabel.h
	src/qompmetadataresolver.h
	src/qompnetworkingfactory.h
	src/qompplayer.h
	src/thememanager.h
	src/tune.h
	${plugins_HDRS}
	${options_HDRS}
)

set( libqomp_INCLUDE_HDRS
	src/common.h
	src/pluginhost.h
	src/qompplayer.h
	src/tune.h
	src/gettuneurlhelper.h
	src/options/qompoptionspage.h
	src/pluginmanager.h
	src/libqomp_global.h
	src/qompactionslist.h
	src/thememanager.h
	src/tuneurlchecker.h
	src/qompbusylabel.h
	src/qompnetworkingfactory.h
	src/qompmetadataresolver.h
	src/options.h
	src/defines.h
	src/plugins/qompplugingettunesdlg.h
	src/plugins/qompplayerstatusplugin.h
	src/plugins/qompplugintypes.h
	src/plugins/qomptunepluign.h
	src/plugins/qompplugintreeview.h
	src/plugins/qompplugincaptchadialog.h
	src/plugins/qompplugincontroller.h
	src/plugins/qompplugin.h
	src/plugins/qompplugintreemodel.h
	src/plugins/qomppluginaction.h
)
set( libqomp_SRCS
	src/common.cpp
	src/gettuneurlhelper.cpp
	src/options.cpp
	src/pluginhost.cpp
	src/pluginmanager.cpp
	src/qompactionslist.cpp
	src/qompbusylabel.cpp
	src/qompmetadataresolver.cpp
	src/qompnetworkingfactory.cpp
	src/qompplayer.cpp
	src/thememanager.cpp
	src/tune.cpp
	src/tuneurlchecker.cpp
	${plugins_SRCS}
)
set( libqomp_UIFILES
	src/plugins/qompplugingettunesdlg.ui
)
find_package( TagLib REQUIRED )
#copy include headers
file( COPY ${libqomp_INCLUDE_HDRS} DESTINATION include )
#
if( LINUX )
	set( LIBQOMP_DATADIR ${CMAKE_INSTALL_PREFIX}/share/qomp )
	set( LIBQOMP_PLUGSDIR ${LIB_INSTALL_DIR}/qomp/plugins )
	add_definitions(
		-DQOMP_DATADIR="${LIBQOMP_DATADIR}"
		-DQOMP_PLUGSDIR="${LIBQOMP_PLUGSDIR}"
	 )
endif()
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${TAGLIB_INCLUDE_DIR}
	include
	src
	src/plugins
	src/options
)
if( USE_QT5 )
	find_package( Qt5 REQUIRED Widgets Network Concurrent )
	qt5_wrap_ui( libqomp_UIS ${libqomp_UIFILES} )
	add_definitions(
		-DHAVE_QT5
	)
	set( QT_DEPLIBS
		Qt5::Widgets
		Qt5::Network
		Qt5::Concurrent
	)
else()
	find_package( Qt4 REQUIRED )
	include( ${QT_USE_FILE} )
	include_directories(
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		${QT_QTNETWORK_INCLUDE_DIR}
	)
	QT4_WRAP_UI( libqomp_UIS ${libqomp_UIFILES} )
	set( QT_DEPLIBS
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTNETWORK_LIBRARY}
	)
endif()

if( DEV_MODE )
	set( LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/qomp" )
endif()

add_library(
	${QOMP_LIB}
	SHARED
	${libqomp_SRCS}
	${libqomp_UIS}
	${options_HDRS}
)
set_target_properties(
	${QOMP_LIB}
	PROPERTIES 
	OUTPUT_NAME qomp
	CLEAN_DIRECT_OUTPUT 1
	VERSION
	${QOMP_LIB_VERSION_STRING}
)
target_link_libraries(
	${QOMP_LIB}
	${QT_DEPLIBS}
	${TAGLIB_LIBRARY}
)

if( LINUX )
	install(
		TARGETS
		${QOMP_LIB}
		LIBRARY DESTINATION
		${LIB_INSTALL_DIR}
	)
endif()

if( WIN32 )
	install(
		TARGETS
		${QOMP_LIB}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}
		RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
		#ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}
	)
endif()
