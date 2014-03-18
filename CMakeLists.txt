project( qomp )
cmake_minimum_required( VERSION 2.8.6 )

if ( UNIX )
	set ( CMAKE_BUILD_TYPE Release )
	set ( CMAKE_AUTOMOC TRUE )
	set ( DATADIR_PATH ${CMAKE_INSTALL_PREFIX}/share )
	set ( APPDATA_PATH ${DATADIR_PATH}/${PROJECT_NAME} )
	add_definitions ( -Wall -DHAVE_X11 -DQOMP_DATADIR="${APPDATA_PATH}" -DHAVE_PHONON ) 
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
	find_package ( Phonon QUIET )
	set ( qomp_phonon_SRCS
		src/qompphononplayer.cpp
		src/qompphononmetadataresolver.cpp
	)
	if ( ${Phonon_FOUND} )
		message ( "-- Looking for Phonon - found" )
		include_directories ( ${PHONON_INCLUDES} )
	else ()
		message ( "-- Looking for Phonon - not found" )
		set ( QT_USE_PHONON TRUE )
		include_directories ( ${QT_PHONON_INCLUDE_DIR} )
	endif( ${Phonon_FOUND} )
	set ( qomp_HDRS )

	set ( qomp_UIS
		src/qompmainwin.ui
		src/aboutdlg.ui
	)
	set ( qomp_RSCS
		src/qomp.qrc
	)
	QT4_ADD_RESOURCES( RSCS ${qomp_RSCS} )
	set ( qomp_TRS
		${PROJECT_NAME}.translations/translations/${PROJECT_NAME}_ru.ts
	)
	QT4_ADD_TRANSLATION( TRS ${qomp_TRS} )
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
	QT4_WRAP_UI(main_UIS 
		${qomp_UIS}
		${options_UIS}
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
		${options_SRCS}
	)
	set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
	set( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" )
	subdirs(libqomp plugins)
	add_executable (
		${PROJECT_NAME}
		${qomp_SRCS}
		${qomp_phonon_SRCS}
		${main_UIS}
		${RSCS}
		${TRS}
	)
	if ( ${Phonon_FOUND} )
		target_link_libraries (
			${PROJECT_NAME}
			libqomp
			${PHONON_LIBRARY}
			${QT_QTCORE_LIBRARY}
			${QT_QTGUI_LIBRARY}
			${QT_QTMULTIMEDIA_LIBRARY}
			${QT_QTNETWORK_LIBRARY} 
		)
	else ()
		target_link_libraries (
			${PROJECT_NAME}
			libqomp
			${QT_PHONON_LIBRARY}
			${QT_QTCORE_LIBRARY}
			${QT_QTGUI_LIBRARY}
			${QT_QTMULTIMEDIA_LIBRARY}
			${QT_QTNETWORK_LIBRARY} 
		)
	endif ( ${Phonon_FOUND} )

	INSTALL ( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin )
	
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
endif ( UNIX )