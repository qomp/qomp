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

set(QT_LIB_LIST Widgets Multimedia)
find_qt_package("${QT_LIB_LIST}" REQUIRED )
add_qt_library(QT_DEPLIBS "${QT_LIB_LIST}")
if(USE_QT6)
	find_qt_package(Core5Compat REQUIRED)
	add_qt_library(QT_DEPLIBS Core5Compat)
endif()

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
