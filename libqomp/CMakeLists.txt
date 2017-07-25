set( QOMP_LIB libqomp )
cmake_minimum_required( VERSION 3.1.0 )

set( CMAKE_AUTOMOC ON )
add_definitions( -DLIBQOMP_LIBRARY )
set(VER_FILE "${PROJECT_SOURCE_DIR}/libqomp/src/defines.h")
if(EXISTS "${VER_FILE}")
	message(STATUS "Found qomp version in file: ${VER_FILE}")
	file(STRINGS "${VER_FILE}" VER_LINES)
	string(REGEX MATCHALL "#define APPLICATION_VERSION \"([0-9])+\\.([0-9])+\\.?([0-9])*" FVER_LINE ${VER_LINES})
	if(${CMAKE_MATCH_COUNT} EQUAL 3 OR (${CMAKE_MATCH_COUNT} EQUAL 2))
		if(CMAKE_MATCH_1)
			set(QOMP_LIB_VERSION_MAJOR ${CMAKE_MATCH_1})
		endif()
		if(CMAKE_MATCH_2)
			set(QOMP_LIB_VERSION_MINOR ${CMAKE_MATCH_2})
		endif()
		if(CMAKE_MATCH_3)
			set(QOMP_LIB_VERSION_PATCH ${CMAKE_MATCH_3})
		else()
			set(QOMP_LIB_VERSION_PATCH 0)
		endif()
	endif()
endif()

set( QOMP_LIB_VERSION_STRING
	${QOMP_LIB_VERSION_MAJOR}.${QOMP_LIB_VERSION_MINOR}.${QOMP_LIB_VERSION_PATCH}
)
message(STATUS "Qomp version is: ${QOMP_LIB_VERSION_STRING}")

set( LIB_SUFFIX "" CACHE STRING "Define suffix of lib directory name (32/64)" )
set( LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE PATH "Directory where libs will be installed" )
if( WIN32 )
	set( TAGLIB_ROOT "C:\\build\\taglib" CACHE STRING "TagLib root path" )
endif()
set( CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}" )
message(STATUS "CXX_FLAGS ${CMAKE_CXX_FLAGS}")
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
	${plugins_HDRS}
	${options_HDRS}
	${frameless_HDRS}
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
	${plugins_SRCS}
	${frameless_SRCS}
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
)

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
qt5_add_resources( RSCS ${libqomp_RSCS} )
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
endif()

if( WIN32 )
	install(
		TARGETS
		${QOMP_LIB}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}
		RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
	)
endif()
