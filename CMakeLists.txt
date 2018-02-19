cmake_minimum_required( VERSION 3.1.0 )

if ( NOT CMAKE_VERSION VERSION_LESS 3.10 )
	cmake_policy(SET CMP0071 OLD)
endif ()

#Hack to determine Qt by qtchooser utility
option(USE_QTCHOOSER "Use qtchooser utility to determine Qt environment" OFF)
if(USE_QTCHOOSER)
    include( ${CMAKE_CURRENT_LIST_DIR}/cmake/modules/qtchooser.cmake )
endif()

project( qomp )

#common part
set( CMAKE_AUTOMOC ON )
set( CMAKE_INCLUDE_CURRENT_DIR ON )

option( USE_QT5 "Use Qt5" ON ) #Was left for compatibility with build scripts
option( DEV_MODE "Enable development mode" OFF )
option( BUILD_THEMES "Build qomp themes" ON )
option( USE_CCACHE "Use ccache utility" OFF )
set( BUILD_PLUGINS "ALL" CACHE STRING "List of plugins to build" )
set( LIB_SUFFIX "" CACHE STRING "Define suffix of lib directory name (32/64)" )
set( LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE PATH "Directory where libs will be installed" )
option(USE_MXE "Use MXE toolkit" OFF)
option( USE_CPACK "Enable CPack" OFF )

if( NOT USE_QT5 )
	message(AUTHOR_WARNING "Qt4 support stopped since 1.1 version. Flag USE_QT5 always enabled")
	set(USE_QT5 ON)
endif()

set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" )

if( NOT CMAKE_BUILD_TYPE )
	set(CMAKE_BUILD_TYPE Release)
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
	add_definitions( -DDEBUG_OUTPUT )
else()
	add_definitions( -DQT_NO_DEBUG )
endif()
message(STATUS "Build type - ${CMAKE_BUILD_TYPE}")

include( ${PROJECT_SOURCE_DIR}/cmake/modules/version.cmake )
set( LIBQOMP_LIBRARY libqomp )
add_subdirectory( ${LIBQOMP_LIBRARY} )
add_subdirectory( plugins )

add_definitions( -DQAPPLICATION_CLASS=QApplication )

if( (CMAKE_CROSSCOMPILING) AND (DEFINED MSYS) )
	message(STATUS "MXE environment detected")
	set(USE_MXE ON)
endif()

if( UNIX AND NOT (APPLE OR CYGWIN) )
	set(LINUX ON)
	find_package( X11 REQUIRED )
	include_directories(
		${X11_INCLUDE_DIR}
	)
	set(EXTRA_LIBS
		${X11_LIBRARIES}
	)
endif()

if( WIN32 )
	set(EXTRA_LIBS
		-luser32
	)
	set( TAGLIB_ROOT "C:\\build\\taglib" CACHE STRING "TagLib root path" )
	set( LIBCUE_ROOT "C:\\build\\libcue" CACHE STRING "LibCue root path" )
endif()

find_package( TagLib REQUIRED )
list(APPEND EXTRA_LIBS ${TAGLIB_LIBRARY} )

set( CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}" )
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/include
	src/options
	src
	thirdparty/QHotkey
	${TAGLIB_INCLUDE_DIR}
)

set( qomp_RSCS
	src/${PROJECT_NAME}.qrc
	src/themes.qrc
)
set( qomp_TRS
	translations/translations/${PROJECT_NAME}_ru.ts
	translations/translations/${PROJECT_NAME}_cs.ts
	translations/translations/${PROJECT_NAME}_it.ts
)
set( options_SRCS
	src/options/qompoptionsdlg.cpp
	src/options/qompoptionskeys.cpp
	src/options/qompoptionsmain.cpp
	src/options/qompoptionsplugins.cpp
)
set( options_UIS
	src/options/qompoptionsdlg.ui
	src/options/qompoptionskeys.ui
	src/options/qompoptionsmain.ui
	src/options/qompoptionsplugins.ui
)
set( singleapp_SRCS
	src/singleapplication/singleapplication.cpp
)
set( hotkey_SRCS
	thirdparty/QHotkey/qhotkey.cpp
)
if( WIN32 )
	list(APPEND hotkey_SRCS
		thirdparty/QHotkey/qhotkey_win.cpp
	)
endif()
if( APPLE )
	list(APPEND hotkey_SRCS
		thirdparty/QHotkey/qhotkey_mac.cpp
	)
endif()
if( LINUX )
	list(APPEND hotkey_SRCS
		thirdparty/QHotkey/qhotkey_x11.cpp
	)
endif()
set( hotkey_HDRS
	thirdparty/QHotkey/qhotkey.h
	thirdparty/QHotkey/qhotkey_p.h
)
set( qomp_SRCS
	src/main.cpp
	src/qompmainwin.cpp
	src/qompcon.cpp
	src/qompplaylistmodel.cpp
	src/qomptrayicon.cpp
	src/aboutdlg.cpp
	src/qompcommandline.cpp
	src/qompinstancewatcher.cpp
	src/qomptaglibmetadataresolver.cpp
	src/qomptunedownloader.cpp
	src/qompplaylistdelegate.cpp
	src/translator.cpp
	src/updateschecker.cpp
	src/qompplaylistview.cpp
	src/qompmenu.cpp
	src/qompqtmultimediaplayer.cpp
	${options_SRCS}
	${singleapp_SRCS}
	${hotkey_SRCS}
	${hotkey_HDRS}
)
if( WIN32 )
	list(APPEND qomp_SRCS
		src/qompthumbnailtoolbar.cpp
	)
	set( qomp_RC ${PROJECT_NAME}.rc )
endif()
set( qomp_UIS
	src/qompmainwin.ui
	src/aboutdlg.ui
	${options_UIS}
)

if( DEV_MODE )
	set( EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/${PROJECT_NAME}" )
	file(MAKE_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")
	file(MAKE_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}/translations")
	file(MAKE_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}/plugins")
	set_property( SOURCE ${qomp_TRS} PROPERTY OUTPUT_LOCATION "${EXECUTABLE_OUTPUT_PATH}/translations" )
	set( QOMP_DATADIR ${EXECUTABLE_OUTPUT_PATH} )
endif()

if( LINUX )
	set( DATADIR_PATH ${CMAKE_INSTALL_PREFIX}/share )
	set( APPDATA_PATH ${DATADIR_PATH}/${PROJECT_NAME} )
	if( NOT QOMP_DATADIR )
		set( QOMP_DATADIR ${APPDATA_PATH} )
	endif()
	add_definitions( -DHAVE_X11 -DQOMP_DATADIR="${QOMP_DATADIR}" )
endif()

find_package( Qt5 COMPONENTS Core Widgets Network Multimedia Concurrent REQUIRED )
if (Qt5Widgets_FOUND)
	message(STATUS "Qt5 version: ${Qt5Widgets_VERSION}")
endif()
find_package( Qt5LinguistTools )
qt5_add_resources( RSCS ${qomp_RSCS} )
if( Qt5LinguistTools_FOUND )
	qt5_add_translation( TRS ${qomp_TRS} )
else()
	message(WARNING "Qt5LinguistTools module not found. The translations files will not be created")
endif()
qt5_wrap_ui(main_UIS
	${qomp_UIS}
)
add_definitions(
	-DHAVE_QT5
	-DHAVE_QTMULTIMEDIA
)
set(QT_DEPLIBS
	Qt5::Widgets
	Qt5::Network
	Qt5::Multimedia
	Qt5::Concurrent
)
if( LINUX )
	find_package(Qt5X11Extras REQUIRED)
	list(APPEND QT_DEPLIBS
		Qt5::X11Extras
	)
endif()
if( WIN32 )
	find_package(Qt5WinExtras REQUIRED)
	list(APPEND QT_DEPLIBS
		Qt5::WinExtras
	)
endif()

if(USE_CCACHE)
	find_program(CCACHE_PATH ccache DOC "Path to ccache")
	if(CCACHE_PATH)
		message(STATUS "Found ccache at ${CCACHE_PATH}")
		set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_PATH})
		set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_PATH})
	endif()
endif()

add_executable(
	${PROJECT_NAME}
	WIN32
	${qomp_SRCS}
	${main_UIS}
	${RSCS}
	${TRS}
	${qomp_RC}
)

macro(create_themes _sources _output)
	qt5_add_binary_resources(build_theme_rcc "${_sources}" OPTIONS -binary DESTINATION "${_output}")
	add_custom_target(build_themes_rcc DEPENDS "${_output}" SOURCES "${_sources}")
	add_dependencies(${PROJECT_NAME} build_themes_rcc)
endmacro()

if(BUILD_THEMES)
	if(EXISTS "${PROJECT_SOURCE_DIR}/themes")
		set(THEMES_EXISTS ON)
		set( THEMES_QRC
			${PROJECT_SOURCE_DIR}/themes/themes.qrc
		)
		if(DEV_MODE)
			file(MAKE_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}/themes")
			set(THEMES_RCC "${EXECUTABLE_OUTPUT_PATH}/themes/themes.rcc")
		else()
			set(THEMES_RCC "${CMAKE_CURRENT_BINARY_DIR}/themes.rcc")
		endif()
		create_themes(${THEMES_QRC} ${THEMES_RCC})
	else()
		message(FATAL_ERROR "Qomp themes sources not found in ${PROJECT_SOURCE_DIR}/themes")
	endif()
endif()

set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )

if(USE_CPACK)
	include( ${PROJECT_SOURCE_DIR}/cmake/modules/qomp-packaging.cmake )
endif()

target_link_libraries(
	${PROJECT_NAME}
	${LIBQOMP_LIBRARY}
	${QT_DEPLIBS}
	${EXTRA_LIBS}
)

add_dependencies( ${PROJECT_NAME} ${LIBQOMP_LIBRARY} )

if( WIN32 )
	if( DEV_MODE )
		set( OPENSSL_ROOT_DIR "c:\\build\\openssl" CACHE STRING "OpenSSL root path" )
		set( ZLIB_ROOT "c:\\build\\zlib" CACHE STRING "ZLIB root path" )
		include( ${PROJECT_SOURCE_DIR}/cmake/modules/Win32Qt5Install_files.cmake )
	endif()
	INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} )
	set( TRS_PATH ${CMAKE_INSTALL_PREFIX}/translations )
	INSTALL( FILES ${TRS} DESTINATION ${TRS_PATH} )
	INSTALL( FILES LICENSE.txt DESTINATION ${CMAKE_INSTALL_PREFIX} )
	if(THEMES_EXISTS)
		INSTALL( FILES ${THEMES_RCC} DESTINATION ${CMAKE_INSTALL_PREFIX}/themes )
	endif()
endif()

if( LINUX )
	INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )

	set( DESKTOP_PATH ${DATADIR_PATH}/applications )
	set( DESKTOPFILES ${PROJECT_NAME}.desktop )
	set( ICONS_PREFIX ${DATADIR_PATH}/icons/hicolor )
	set( ICON_16 src/icons/qomp_16.png )
	set( ICON_24 src/icons/qomp_24.png )
	set( ICON_32 src/icons/qomp_32.png )
	set( ICON_48 src/icons/qomp_48.png )
	set( ICON_64 src/icons/qomp_64.png )
	set( ICON_128 src/icons/qomp.png )
	set( TRS_PATH ${APPDATA_PATH}/translations )

	INSTALL( FILES ${ICON_16} DESTINATION ${ICONS_PREFIX}/16x16/apps RENAME qomp.png )
	INSTALL( FILES ${ICON_128} DESTINATION ${ICONS_PREFIX}/128x128/apps RENAME qomp.png )
	INSTALL( FILES ${ICON_24} DESTINATION ${ICONS_PREFIX}/24x24/apps RENAME qomp.png )
	INSTALL( FILES ${ICON_32} DESTINATION ${ICONS_PREFIX}/32x32/apps RENAME qomp.png )
	INSTALL( FILES ${ICON_48} DESTINATION ${ICONS_PREFIX}/48x48/apps RENAME qomp.png )
	INSTALL( FILES ${ICON_64} DESTINATION ${ICONS_PREFIX}/64x64/apps RENAME qomp.png )
	INSTALL( FILES ${TRS} DESTINATION ${TRS_PATH} )
	INSTALL( FILES ${DESKTOPFILES} DESTINATION ${DESKTOP_PATH} )
	if(THEMES_EXISTS)
		INSTALL( FILES ${THEMES_RCC} DESTINATION ${APPDATA_PATH}/themes )
	endif()
	if(USE_CPACK)
		INSTALL( FILES ${PROJECT_BINARY_DIR}/copyright DESTINATION ${DATADIR_PATH}/doc/${PROJECT_NAME} )
	endif()
endif()

unset( BUILD_PLUGINS CACHE )
