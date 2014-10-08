set ( QOMP_LIB libqomp )
cmake_minimum_required( VERSION 2.8.11 )
cmake_policy(SET CMP0028 OLD)
set ( CMAKE_AUTOMOC ON )
add_definitions ( -Wall -DLIBQOMP_LIBRARY )
set ( plugins_HDRS
	src/plugins/qompplugingettunesdlg.h
	src/plugins/qompplugintreeview.h
	src/plugins/qompplugintreemodel.h
	src/plugins/qompplugincontroller.h
	src/plugins/qomppluginaction.h
)
set ( plugins_SRCS
	src/plugins/qompplugingettunesdlg.cpp
	src/plugins/qompplugintreeview.cpp
	src/plugins/qompplugintreemodel.cpp
	src/plugins/qompplugintypes.cpp
	src/plugins/qompplugincontroller.cpp
	src/plugins/qomppluginaction.cpp
)
set ( options_HDRS
	src/options/qompoptionspage.h
)
set ( libqomp_HDRS
	src/options.h
	src/pluginhost.h
	src/pluginmanager.h
	src/qompactionslist.h
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
	src/qompactionslist.cpp
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
find_package ( TagLib REQUIRED )

if ( ${UNIX} )
	set (LIBQOMP_DATADIR ${CMAKE_INSTALL_PREFIX}/share/qomp)
	add_definitions ( -DQOMP_DATADIR="${LIBQOMP_DATADIR}" )
endif ( ${UNIX} )
include_directories (
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${TAGLIB_INCLUDE_DIR}
	include
	src
	src/plugins
	src/options
)
if ( ${USE_QT5} )
	find_package ( Qt5Widgets REQUIRED )
	find_package ( Qt5Network REQUIRED )
	include_directories (
		${Qt5Widgets_INCLUDES}
		${Qt5Network_INCLUDES}
	)
	qt5_wrap_ui(libqomp_UIS ${libqomp_UIFILES})
	set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
	add_definitions(
		${Qt5Widgets_DEFINITIONS}
		${Qt5Network_DEFINITIONS}
		-DHAVE_QT5
	)
	set (QT_DEPLIBS
		Qt5::Widgets
		Qt5::Network
	)
else ( ${USE_QT5} )
	find_package ( Qt4 REQUIRED )
	include ( ${QT_USE_FILE} )
	include_directories (
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		${QT_QTNETWORK_INCLUDE_DIR}
	)
	QT4_WRAP_UI(libqomp_UIS ${libqomp_UIFILES})
	set (QT_DEPLIBS
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTNETWORK_LIBRARY}
	)
endif ( ${USE_QT5} )

add_library (
	${QOMP_LIB}
	SHARED
	${libqomp_SRCS}
	${libqomp_UIS}
	${options_HDRS}
)
set_target_properties (
	${QOMP_LIB}
	PROPERTIES 
	OUTPUT_NAME qomp
	CLEAN_DIRECT_OUTPUT 1
)
target_link_libraries (
	${QOMP_LIB}
	${QT_DEPLIBS}
	${TAGLIB_LIBRARY}
)

if ( ${UNIX} )
	install (
		TARGETS
		${QOMP_LIB}
		LIBRARY DESTINATION
		${CMAKE_INSTALL_PREFIX}/lib
	)
endif( ${UNIX} )

if ( ${WIN32} )
	install (
		TARGETS
		${QOMP_LIB}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}
		RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
		#ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}
	)
endif ( ${WIN32} )
