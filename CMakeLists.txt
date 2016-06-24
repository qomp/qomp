project( qomp )
cmake_minimum_required( VERSION 2.8.12 )
if( CMAKE_VERSION VERSION_GREATER 3.0.0 )
	cmake_policy(SET CMP0028 OLD)
endif()
#common part
set( CMAKE_AUTOMOC ON )
set( CMAKE_INCLUDE_CURRENT_DIR ON )

option( USE_QT5 "Use Qt5" ON )
option( INSTALL_DEPS "Install win32 qomp dependencies" OFF )
option( DEV_MODE "Enable development mode" OFF )
option( BUILD_THEMES "Build qomp themes" ON )
set( BUILD_PLUGINS "ALL" CACHE STRING "List of plugins to build" )
set( LIB_SUFFIX "" CACHE STRING "Define suffix of lib directory name (32/64)" )
set( LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE PATH "Directory where libs will be installed" )

set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" )

if( NOT CMAKE_BUILD_TYPE )
	set(CMAKE_BUILD_TYPE Release)
endif()
if( NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
	add_definitions( -DQT_NO_DEBUG )
endif()

set( LIBQOMP_LIBRARY libqomp )
add_subdirectory( ${LIBQOMP_LIBRARY} )
add_subdirectory( plugins )

set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}" )
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_BINARY_DIR}/libqomp/include
	src/options
	src
	)

set( qomp_RSCS
	src/${PROJECT_NAME}.qrc
	src/themes.qrc
	)
set( qomp_TRS
	translations/translations/${PROJECT_NAME}_ru.ts
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
set( qomp_SRCS
	src/main.cpp
	src/qompmainwin.cpp
	src/qompcon.cpp
	src/qompplaylistmodel.cpp
	src/qomptrayicon.cpp
	src/aboutdlg.cpp
	src/qomptunedownloader.cpp
	src/qompplaylistdelegate.cpp
	src/translator.cpp
	src/updateschecker.cpp
	src/qompplaylistview.cpp
	src/qompmenu.cpp
	${options_SRCS}
	)
set( qomp_UIS
	src/qompmainwin.ui
	src/aboutdlg.ui
	${options_UIS}
	)

if( WIN32 )
	set( TAGLIB_ROOT "C:\\build\\taglib" CACHE STRING "TagLib root path" )
	set( LIBCUE_ROOT "C:\\build\\libcue" CACHE STRING "LibCue root path" )
	if( INSTALL_DEPS )
		set( OPENSSL_ROOT_DIR "c:\\build\\psibuild\\psideps\\Qt5\\openssl" CACHE STRING "OpenSSL root path" )
		set( ZLIB_ROOT "c:\\build\\psibuild\\psideps\\Qt5\\zlib" CACHE STRING "ZLIB root path" )
	endif()
	set( qomp_RC ${PROJECT_NAME}.rc )
endif()

if( UNIX AND NOT (APPLE OR CYGWIN) )
	set(LINUX ON)
endif()

if( DEV_MODE )
	set( EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/${PROJECT_NAME}" )
	file(MAKE_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")
	file(MAKE_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}/translations")
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

if( USE_QT5 )
	find_package( Qt5 REQUIRED Widgets Network Multimedia Concurrent )
	find_package( Qt5LinguistTools )
	qt5_add_resources( RSCS ${qomp_RSCS} )
	if( Qt5LinguistTools_FOUND )
		qt5_add_translation( TRS ${qomp_TRS} )
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
	set(qomp_MMEDIA
		src/qompqtmultimediaplayer.cpp
		)
else()
	message(WARNING " Qt4 not longer supported! Last version of qomp with Qt4 support is 1.0")
	find_package( Qt4 REQUIRED )
	include( ${QT_USE_FILE} )
	include_directories(
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		${QT_QTNETWORK_INCLUDE_DIR}
		)
	QT4_ADD_RESOURCES( RSCS ${qomp_RSCS} )
	QT4_ADD_TRANSLATION( TRS ${qomp_TRS} )
	QT4_WRAP_UI(main_UIS
		${qomp_UIS}
		)
	if( LINUX )
		find_package( Phonon QUIET )
		set( qomp_phonon_SRCS
			src/qompphononplayer.cpp
			)
		add_definitions(-DHAVE_PHONON)
		if( ${Phonon_FOUND} )
			message( "-- Looking for Phonon - found" )
			foreach( inc_path ${PHONON_INCLUDES};/usr/include/phonon )
				if( EXISTS "${inc_path}/Phonon/MediaObject" )
					include_directories(${inc_path})
				endif()
			endforeach( inc_path )
			set( phonon_LIB ${PHONON_LIBRARY} )
		else()
			message( "-- Looking for Phonon - not found" )
			set( QT_USE_PHONON TRUE )
			include_directories( ${QT_PHONON_INCLUDE_DIR} )
			set( phonon_LIB ${QT_PHONON_LIBRARY} )
		endif()
	endif()
	set(QT_DEPLIBS
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTMULTIMEDIA_LIBRARY}
		${QT_QTNETWORK_LIBRARY}
		)
endif()

if(BUILD_THEMES)
	if(EXISTS "${PROJECT_SOURCE_DIR}/themes")
		set(THEMES_EXISTS ON)
		if( USE_QT5 )
			set( THEMES_QRC
				${PROJECT_SOURCE_DIR}/themes/themes.qrc
				)
			if(DEV_MODE)
				file(MAKE_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}/themes")
				set(THEMES_RCC "${EXECUTABLE_OUTPUT_PATH}/themes/themes.rcc")
			else()
				set(THEMES_RCC "${CMAKE_CURRENT_BINARY_DIR}/themes.rcc")
			endif()
			qt5_add_binary_resources(build_themes_rcc "${THEMES_QRC}" OPTIONS -binary DESTINATION "${THEMES_RCC}")
		endif()
	else()
		message(FATAL_ERROR "Qomp themes sources not found in ${PROJECT_SOURCE_DIR}/themes")
	endif()
endif()

set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
if( LINUX )
	set( CMAKE_INSTALL_RPATH "${LIB_INSTALL_DIR}/qomp" )
endif()

add_executable(
	${PROJECT_NAME}
	WIN32
	${qomp_SRCS}
	${qomp_phonon_SRCS}
	${qomp_MMEDIA}
	${main_UIS}
	${RSCS}
	${TRS}
	${qomp_RC}
	)

target_link_libraries(
	${PROJECT_NAME}
	${LIBQOMP_LIBRARY}
	${phonon_LIB}
	${QT_DEPLIBS}
	)

add_dependencies( ${PROJECT_NAME} ${LIBQOMP_LIBRARY} )

if( WIN32 )
	INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} )
	if( INSTALL_DEPS AND USE_QT5 )
		include( Win32Qt5Install_files )
	endif()
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
endif()

unset( BUILD_PLUGINS CACHE )
