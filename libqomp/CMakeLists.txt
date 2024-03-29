set( QOMP_LIB libqomp )
cmake_minimum_required( VERSION 3.7.0 )

include(policyRules)
include(qtvars)

set( CMAKE_AUTOMOC ON )
add_definitions( -DLIBQOMP_LIBRARY )

set( QOMP_LIB_VERSION_STRING "${QOMP_VERSION_STRING}" )

set( LIB_SUFFIX "" CACHE STRING "Define suffix of lib directory name (32/64)" )
set( LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE PATH "Directory where libs will be installed" )
if( WIN32 )
	set( TAGLIB_ROOT "C:\\build\\taglib" CACHE STRING "TagLib root path" )
endif()
set(CMAKE_CXX_STANDARD 11)
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
set( frameless_HDRS
	src/frameless/advwidget.h
	src/frameless/windowheader.h
)
set( frameless_SRCS
	src/frameless/advwidget.cpp
	src/frameless/windowheader.cpp
)
set( g_analytics_HDRS
	${CMAKE_SOURCE_DIR}/ga/ganalytics.h
)
set( g_analytics_SRCS
	${CMAKE_SOURCE_DIR}/ga/ganalytics.cpp
)
set( libqomp_HDRS
	src/covercache.h
	src/extendedcombobox.h
	src/options.h
	src/pluginhost.h
	src/pluginmanager.h
	src/qompactionslist.h
	src/qompbusylabel.h
	src/qompmetadataresolver.h
	src/qompnetworkingfactory.h
	src/qompplayer.h
	src/qompplayercontrol.h
	src/taghelpers.h
	src/thememanager.h
	src/tune.h
	src/playlistparser.h
)

list(APPEND libqomp_HDRS
	${plugins_HDRS}
	${options_HDRS}
	${frameless_HDRS}
	${g_analytics_HDRS}
)

set( libqomp_INCLUDE_HDRS
	src/common.h
	src/extendedcombobox.h
	src/pluginhost.h
	src/qompplayer.h
	src/tune.h
	src/gettuneurlhelper.h
	src/options/qompoptionspage.h
	src/pluginmanager.h
	src/libqomp_global.h
	src/qompactionslist.h
	src/taghelpers.h
	src/thememanager.h
	src/tuneurlchecker.h
	src/qompbusylabel.h
	src/qompnetworkingfactory.h
	src/qompmetadataresolver.h
	src/qompplayercontrol.h
	src/options.h
	src/defines.h
	src/playlistparser.h
	src/frameless/advwidget.h
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
	src/covercache.cpp
	src/extendedcombobox.cpp
	src/gettuneurlhelper.cpp
	src/options.cpp
	src/pluginhost.cpp
	src/pluginmanager.cpp
	src/qompactionslist.cpp
	src/qompbusylabel.cpp
	src/qompmetadataresolver.cpp
	src/qompnetworkingfactory.cpp
	src/qompplayer.cpp
	src/taghelpers.cpp
	src/thememanager.cpp
	src/tune.cpp
	src/tuneurlchecker.cpp
	src/playlistparser.cpp
)
list(APPEND libqomp_SRCS
	${plugins_SRCS}
	${frameless_SRCS}
	${g_analytics_SRCS}
)
set( libqomp_UIFILES
	src/plugins/qompplugingettunesdlg.ui
	src/frameless/windowheader.ui
)
set( libqomp_RSCS
	src/icons.qrc
)
find_package( TagLib REQUIRED )
#copy include headers
file( COPY ${libqomp_INCLUDE_HDRS} DESTINATION ${PROJECT_BINARY_DIR}/include )
#
if( LINUX )
	if(DEV_MODE)
		set( LIBQOMP_DATADIR ${PROJECT_BINARY_DIR}/qomp )
	else()
		set( LIBQOMP_DATADIR ${CMAKE_INSTALL_PREFIX}/share/qomp )
	endif()
	set( LIBQOMP_PLUGSDIR ${LIB_INSTALL_DIR}/qomp/plugins )
	add_definitions(
		-DHAVE_X11
		-DQOMP_DATADIR="${LIBQOMP_DATADIR}"
		-DQOMP_PLUGSDIR="${LIBQOMP_PLUGSDIR}"
	 )
endif()
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${TAGLIB_INCLUDE_DIR}
	${PROJECT_BINARY_DIR}/include
	src
	src/frameless
	src/plugins
	src/options
	${CMAKE_SOURCE_DIR}/ga
)

set(QT_LIB_LIST Core Widgets Network Concurrent)
find_qt_package( "${QT_LIB_LIST}" REQUIRED )
add_qt_library(QT_DEPLIBS "${QT_LIB_LIST}")

if(USE_QT6)
	find_qt_package(Core5Compat REQUIRED)
	add_qt_library(QT_DEPLIBS Core5Compat)
endif()
wrap_qt_ui(libqomp_UIS "${libqomp_UIFILES}")
add_qt_resources( RSCS "${libqomp_RSCS}" )

if( DEV_MODE )
	set( LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/qomp" )
endif()

add_library(
	${QOMP_LIB}
	SHARED
	${libqomp_SRCS}
	${libqomp_UIS}
	${options_HDRS}
	${RSCS}
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
	PRIVATE
	${QT_DEPLIBS}
	${TAGLIB_LIBRARY}
)

if( LINUX )
	install(
		TARGETS
		${QOMP_LIB}
		LIBRARY DESTINATION
		${LIB_INSTALL_DIR}
		NAMELINK_SKIP
	)
elseif( WIN32 )
	install(
		TARGETS
		${QOMP_LIB}
		LIBRARY	DESTINATION ${CMAKE_INSTALL_PREFIX}
		RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
	)
endif()
