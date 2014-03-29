project ( libqomp )
cmake_minimum_required( VERSION 2.8.6 )
if ( UNIX )
	set ( CMAKE_BUILD_TYPE Release )
	set ( CMAKE_AUTOMOC TRUE )
	find_package ( Qt4 REQUIRED )
	include ( ${QT_USE_FILE} )
	include_directories (
		${CMAKE_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		${QT_QTNETWORK_INCLUDE_DIR}
		include
		src
		src/plugins
		src/options
	)
	set ( plugins_HDRS
		src/plugins/qompplugingettunesdlg.h
		src/plugins/qompplugintreeview.h
		src/plugins/qompplugintreemodel.h
		src/plugins/qompplugincontroller.h
	)
	set ( plugins_SRCS
		src/plugins/qompplugingettunesdlg.cpp
		src/plugins/qompplugintreeview.cpp
		src/plugins/qompplugintreemodel.cpp
		src/plugins/qompplugintypes.cpp
		src/plugins/qompplugincontroller.cpp
	)
	set ( options_HDRS
		src/options/qompoptionspage.h
	)
	set ( libqomp_HDRS
		src/options.h
    src/pluginhost.h
		src/pluginmanager.h
		src/qompbusylabel.h
		src/qompmetadataresolver.h
		src/qompnetworkingfactory.h
		src/qompplayer.h
		src/tune.h
		${plugins_HDRS}
		${options_HDRS}
	)
	set ( libqomp_SRCS
		src/common.cpp
		src/options.cpp
		src/qompbusylabel.cpp
		src/qompmetadataresolver.cpp
		src/qompnetworkingfactory.cpp
		src/qompplayer.cpp
		src/tune.cpp
    src/pluginhost.cpp
		src/pluginmanager.cpp
		${plugins_SRCS}
	)
	set ( libqomp_UIFILES
		src/plugins/qompplugingettunesdlg.ui
	)
	QT4_WRAP_UI(libqomp_UIS ${libqomp_UIFILES})
	set (LIBQOMP_DATADIR ${CMAKE_INSTALL_PREFIX}/share/qomp)
	add_definitions ( -Wall -DQOMP_DATADIR="${LIBQOMP_DATADIR}" -LIBQOMP_LIBRARY )

	add_library (
		${PROJECT_NAME}
		SHARED
		${libqomp_SRCS}
		${libqomp_UIS}
		${options_HDRS}
	)
	target_link_libraries (
		${PROJECT_NAME}
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTNETWORK_LIBRARY}
	)
	set_target_properties (
		${PROJECT_NAME}
		PROPERTIES 
		OUTPUT_NAME qomp
		CLEAN_DIRECT_OUTPUT 1
	)
	install (
		TARGETS
		${PROJECT_NAME}
		LIBRARY DESTINATION
		${CMAKE_INSTALL_PREFIX}/lib
	)
endif( UNIX )