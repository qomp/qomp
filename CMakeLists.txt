project( qomp )
cmake_minimum_required( VERSION 2.8.11 )

#common part
set ( CMAKE_AUTOMOC ON )
set ( CMAKE_INCLUDE_CURRENT_DIR ON )
set ( BUILD_PLUGINS "ALL" CACHE STRING "List of plugins to build" )
set ( USE_QT5 OFF CACHE BOOL "Use Qt5" )
if ( ${WIN32} )
	set ( TAGLIB_ROOT "C:\\build\\taglib" CACHE STRING "TagLib root path" )
endif ( ${WIN32} )
set ( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" )

set ( LIBQOMP_LIBRARY libqomp )
add_subdirectory ( ${LIBQOMP_LIBRARY} )
add_subdirectory ( plugins )

set ( qomp_UIS
	src/qompmainwin.ui
	src/aboutdlg.ui
)
set ( qomp_RSCS
	src/qomp.qrc
	src/themes.qrc
)
set ( qomp_TRS
	${PROJECT_NAME}.translations/translations/${PROJECT_NAME}_ru.ts
)
set ( options_SRCS
	src/options/qompoptionsdlg.cpp
	src/options/qompoptionsmain.cpp
	src/options/qompoptionsplugins.cpp
)
set ( options_UIS
	src/options/qompoptionsdlg.ui
	src/options/qompoptionsmain.ui
	src/options/qompoptionsplugins.ui
)
set ( qomp_SRCS
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
	src/thememanager.cpp
	${options_SRCS}
)

if ( ${USE_QT5} )
	find_package ( Qt5Widgets REQUIRED )
	find_package ( Qt5Network REQUIRED )
	find_package ( Qt5Multimedia REQUIRED )
	find_package ( Qt5LinguistTools )
	include_directories (
		${CMAKE_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		${Qt5Widgets_INCLUDES}
		${Qt5Network_INCLUDES}
		${Qt5Multimedia_INCLUDES}
		libqomp/include
		libqomp/src
		libqomp/src/options
		src/options
		src
	)
	qt5_add_resources( RSCS ${qomp_RSCS} )
	qt5_add_translation( TRS ${qomp_TRS} )
	qt5_wrap_ui(main_UIS 
		${qomp_UIS}
		${options_UIS}
	)
	set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
	add_definitions(
		-DHAVE_QT5
		-DHAVE_QTMULTIMEDIA
		${Qt5Widgets_DEFINITIONS}
		${Qt5Network_DEFINITIONS}
		${Qt5Multimedia_DEFINITIONS}
	)
	set (QT_DEPLIBS
		Qt5::Widgets
		Qt5::Network
		Qt5::Multimedia
	)
	set (qomp_MMEDIA
		src/qompqtmultimediaplayer.cpp
	)
else ( ${USE_QT5} )
	find_package ( Qt4 REQUIRED )
	include ( ${QT_USE_FILE} )
	include_directories (
		${CMAKE_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		${QT_QTNETWORK_INCLUDE_DIR}
		libqomp/include
		libqomp/src
		libqomp/src/options
		src/options
		src
	)
	QT4_ADD_RESOURCES( RSCS ${qomp_RSCS} )
	QT4_ADD_TRANSLATION( TRS ${qomp_TRS} )
	QT4_WRAP_UI(main_UIS 
		${qomp_UIS}
		${options_UIS}
	)
	set (QT_DEPLIBS
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTMULTIMEDIA_LIBRARY}
		${QT_QTNETWORK_LIBRARY}
	)
endif ( ${USE_QT5} )


if ( ${WIN32} )
	set ( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
	set ( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" )
	add_executable (
		${PROJECT_NAME}
		WIN32
		${qomp_SRCS}
		${qomp_MMEDIA}
		${main_UIS}
		${RSCS}
		${TRS}
		qomp.rc
	)
	target_link_libraries (
			${PROJECT_NAME}
			${LIBQOMP_LIBRARY}
			${QT_DEPLIBS}
	)
	add_dependencies( ${PROJECT_NAME} ${LIBQOMP_LIBRARY} )
	INSTALL ( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} )
	set ( TRS_PATH ${CMAKE_INSTALL_PREFIX}/translations )
	INSTALL ( FILES ${TRS} DESTINATION ${TRS_PATH} )
	INSTALL ( FILES LICENSE.txt DESTINATION ${CMAKE_INSTALL_PREFIX} )
endif ( ${WIN32} )

if ( ${UNIX} )
	set ( DATADIR_PATH ${CMAKE_INSTALL_PREFIX}/share )
	set ( APPDATA_PATH ${DATADIR_PATH}/${PROJECT_NAME} )
	add_definitions ( -DHAVE_X11 -DQOMP_DATADIR="${APPDATA_PATH}" )
	if( NOT ${USE_QT5} )
		find_package ( Phonon QUIET )
			set ( qomp_phonon_SRCS
				src/qompphononplayer.cpp
				#src/qompphononmetadataresolver.cpp -- disabled in qomp
			)
		if ( ${Phonon_FOUND} )
			message ( "-- Looking for Phonon - found" )
			add_definitions(-DHAVE_PHONON)
			include_directories ( ${PHONON_INCLUDES} )
		else ()
			message ( "-- Looking for Phonon - not found" )
			set ( QT_USE_PHONON TRUE )
			include_directories ( ${QT_PHONON_INCLUDE_DIR} )
		endif( ${Phonon_FOUND} )
	endif( NOT ${USE_QT5} )
	set ( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
	set ( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" )
	add_executable (
		${PROJECT_NAME}
		${qomp_SRCS}
		${qomp_phonon_SRCS}
		${main_UIS}
		${RSCS}
		${TRS}
	)
	if(${USE_QT5})
		target_link_libraries (
			${PROJECT_NAME}
			${LIBQOMP_LIBRARY}
			${QT_DEPLIBS}
		)
	else(${USE_QT5})
		if ( ${Phonon_FOUND} )
			target_link_libraries (
				${PROJECT_NAME}
				${LIBQOMP_LIBRARY}
				${PHONON_LIBRARY}
				${QT_DEPLIBS}
			)
		else ( ${Phonon_FOUND} )
			target_link_libraries (
				${PROJECT_NAME}
				${LIBQOMP_LIBRARY}
				${QT_PHONON_LIBRARY}
				${QT_DEPLIBS}
			)
		endif ( ${Phonon_FOUND} )
	endif(${USE_QT5})
	add_dependencies( ${PROJECT_NAME} ${LIBQOMP_LIBRARY} )
	INSTALL ( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
	
	set ( DESKTOP_PATH ${DATADIR_PATH}/applications )
	set ( DESKTOPFILES ${PROJECT_NAME}.desktop )
	set ( ICONS_PREFIX ${DATADIR_PATH}/icons/hicolor )
	set ( ICON_16 src/icons/qomp_16.png )
	set ( ICON_24 src/icons/qomp_24.png )
	set ( ICON_32 src/icons/qomp_32.png )
	set ( ICON_48 src/icons/qomp_48.png )
	set ( ICON_64 src/icons/qomp_64.png )
	set ( ICON_128 src/icons/qomp.png )
	set ( TRS_PATH ${APPDATA_PATH}/translations )

	INSTALL ( FILES ${ICON_16} DESTINATION ${ICONS_PREFIX}/16x16/apps RENAME qomp.png )
	INSTALL ( FILES ${ICON_128} DESTINATION ${ICONS_PREFIX}/128x128/apps RENAME qomp.png )
	INSTALL ( FILES ${ICON_24} DESTINATION ${ICONS_PREFIX}/24x24/apps RENAME qomp.png )
	INSTALL ( FILES ${ICON_32} DESTINATION ${ICONS_PREFIX}/32x32/apps RENAME qomp.png )
	INSTALL ( FILES ${ICON_48} DESTINATION ${ICONS_PREFIX}/48x48/apps RENAME qomp.png )
	INSTALL ( FILES ${ICON_64} DESTINATION ${ICONS_PREFIX}/64x64/apps RENAME qomp.png )
	INSTALL ( FILES ${TRS} DESTINATION ${TRS_PATH} )
	INSTALL ( FILES ${DESKTOPFILES} DESTINATION ${DESKTOP_PATH} )
	
endif ( ${UNIX} )
unset( BUILD_PLUGINS CACHE )
