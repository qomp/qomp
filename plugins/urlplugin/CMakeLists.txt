set( PLUGIN urlplugin )
cmake_minimum_required( VERSION 3.7.0 )

include(pluginsCommon)

set( urlplugin_HDRS
	${PLUGIN}.h
)
set( urlplugin_SRCS
	${PLUGIN}.cpp
)
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/include
)

find_package( Qt5 COMPONENTS Widgets REQUIRED )
set( QT_DEPLIBS
	Qt5::Widgets
)

add_library(
	${PLUGIN}
	MODULE
	${urlplugin_SRCS}
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
