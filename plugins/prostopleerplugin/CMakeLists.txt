set ( PLUGIN prostopleerplugin )
cmake_minimum_required( VERSION 2.8.6 )
if ( ${UNIX} )
	set ( CMAKE_AUTOMOC TRUE )
	add_definitions ( -Wall )
	find_package ( Qt4 REQUIRED )
	include ( ${QT_USE_FILE} )
	include_directories (
		${CMAKE_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		${QT_QTNETWORK_INCLUDE_DIR}
		../../libqomp/include
	)
	set ( prostopleerplugin_HDRS
		${PLUGIN}.h
		prostopleerplugingettunesdialog.h
		prostopleerpluginsettings.h
		prostopleerplugindefines.h
		prostopleercontroller.h
	)
	set ( prostopleerplugin_SRCS
		${PLUGIN}.cpp
		prostopleerplugingettunesdialog.cpp
		prostopleerpluginsettings.cpp
		prostopleercontroller.cpp
	)
	set ( prostopleerplugin_UIS
		prostopleerpluginresultswidget.ui
		prostopleerpluginsettings.ui
	)
	QT4_WRAP_UI(UIS ${prostopleerplugin_UIS})
	add_library (
		${PLUGIN}
		SHARED
		${prostopleerplugin_SRCS}
		${UIS}
	)
	target_link_libraries (
		${PLUGIN}
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTNETWORK_LIBRARY}
	)
	install (
		TARGETS
		${PLUGIN}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}/share/qomp/plugins
	)
endif( ${UNIX} )