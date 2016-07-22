set( PLUGIN filesystemplugin )
cmake_minimum_required( VERSION 2.8.12)
if( CMAKE_VERSION VERSION_GREATER 3.0.0 )
	cmake_policy(SET CMP0028 OLD)
endif()
set( CMAKE_AUTOMOC ON )
if( WIN32 )
	set( TAGLIB_ROOT "C:\\build\\taglib" CACHE STRING "TagLib root path" )
	set( LIBCUE_ROOT "C:\\build\\libcue" CACHE STRING "LibCue root path" )
endif()
add_definitions( -Wall -DQT_PLUGIN )
set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}" )
if( UNIX AND NOT (APPLE OR CYGWIN) )
	set(LINUX ON)
endif()
set( filesystemplugin_HDRS
	${PLUGIN}.h
	cueparser.h
	filesystemcommon.h
)
set( filesystemplugin_SRCS
	${PLUGIN}.cpp
	cueparser.cpp
	filesystemcommon.cpp
)
find_package( TagLib REQUIRED )
find_package( LibCue REQUIRED )
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${TAGLIB_INCLUDE_DIR}
	${LIBCUE_INCLUDE_DIR}
	${PROJECT_BINARY_DIR}/libqomp/include
)

find_package( Qt5Widgets REQUIRED )
add_definitions(
	-DHAVE_QT5
)
set( QT_DEPLIBS
	Qt5::Widgets
)

add_library(
	${PLUGIN}
	SHARED
	${filesystemplugin_SRCS}
)
target_link_libraries(
	${PLUGIN}
	${TAGLIB_LIBRARY}
	${LIBCUE_LIBRARY}
	${QT_DEPLIBS}
	libqomp
)
add_dependencies( ${PLUGIN} libqomp qomp )
if( WIN32 )
	install(
		TARGETS
		${PLUGIN}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}/plugins
		RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins
	)
endif()

if( LINUX )
	install(
		TARGETS
		${PLUGIN}
		LIBRARY
		DESTINATION
		${LIB_INSTALL_DIR}/qomp/plugins
		RUNTIME DESTINATION
		${LIB_INSTALL_DIR}/qomp/plugins
	)
endif()
