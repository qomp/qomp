set( PLUGIN myzukaruplugin )
cmake_minimum_required( VERSION 3.7.0 )

include(pluginsCommon)

set( myzukaruplugin_HDRS
	${PLUGIN}.h
	myzukarudefines.h
	myzukarugettunesdlg.h
	myzukarucontroller.h
	myzukaruresolvestrategy.h
)
set( myzukaruplugin_SRCS
	${PLUGIN}.cpp
	myzukarugettunesdlg.cpp
	myzukarucontroller.cpp
	myzukaruresolvestrategy.cpp
)
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/include
)

find_package( Qt5 COMPONENTS Widgets Network REQUIRED )
set( QT_DEPLIBS
	Qt5::Widgets
	Qt5::Network
)

add_library(
	${PLUGIN}
	MODULE
	${myzukaruplugin_SRCS}
)
target_link_libraries(
	${PLUGIN}
	${QT_DEPLIBS}
	libqomp
)
add_dependencies(${PLUGIN} libqomp qomp )
install(
	TARGETS
	${PLUGIN}
	LIBRARY DESTINATION ${PLUGINS_INSTALL_PATH}
	RUNTIME DESTINATION ${PLUGINS_INSTALL_PATH}
)
