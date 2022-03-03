set( PLUGIN filesystemplugin )
cmake_minimum_required( VERSION 3.7.0)

include(pluginsCommon)

if( WIN32 )
	set( LIBCUE_ROOT "C:\\build\\libcue" CACHE STRING "LibCue root path" )
endif()

set( filesystemplugin_HDRS
	${PLUGIN}.h
	cueparser.h
)
set( filesystemplugin_SRCS
	${PLUGIN}.cpp
	cueparser.cpp
)

find_package( LibCue REQUIRED )
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/include
)

find_package( Qt5 COMPONENTS Widgets Multimedia REQUIRED)
set( QT_DEPLIBS
	Qt5::Widgets
	Qt5::Multimedia
)
add_library(
	${PLUGIN}
	MODULE
	${filesystemplugin_SRCS}
)
target_link_libraries(
	${PLUGIN}
	${LIBCUE_LIBRARY}
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
