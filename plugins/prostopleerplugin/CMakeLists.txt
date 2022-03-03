set( PLUGIN prostopleerplugin )
cmake_minimum_required( VERSION 3.7.0 )

include(pluginsCommon)

set( prostopleerplugin_HDRS
	${PLUGIN}.h
	prostopleerplugingettunesdialog.h
	prostopleerpluginsettings.h
	prostopleerplugindefines.h
	prostopleercontroller.h
)
set( prostopleerplugin_SRCS
	${PLUGIN}.cpp
	prostopleerplugingettunesdialog.cpp
	prostopleerpluginsettings.cpp
	prostopleercontroller.cpp
)
set( prostopleerplugin_UIS
	prostopleerpluginresultswidget.ui
	prostopleerpluginsettings.ui
)
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/include
)

find_package( Qt5 COMPONENTS Widgets Network REQUIRED )
qt5_wrap_ui( UIS ${prostopleerplugin_UIS} )
set( QT_DEPLIBS
	Qt5::Widgets
	Qt5::Network
)

add_library(
	${PLUGIN}
	MODULE
	${prostopleerplugin_SRCS}
	${UIS}
)
target_link_libraries(
	${PLUGIN}
	${QT_DEPLIBS}
	libqomp
)
add_dependencies( ${PLUGIN} libqomp qomp )
install(
	TARGETS
	${PLUGIN}
	LIBRARY DESTINATION ${PLUGINS_INSTALL_PATH}
	RUNTIME DESTINATION ${PLUGINS_INSTALL_PATH}
)
