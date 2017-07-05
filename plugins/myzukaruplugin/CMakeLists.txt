set( PLUGIN myzukaruplugin )
cmake_minimum_required( VERSION 3.1.0 )

set( CMAKE_AUTOMOC ON )
add_definitions( -DQT_PLUGIN )
if( UNIX AND NOT (APPLE OR CYGWIN) )
	set(LINUX ON)
endif()
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
add_definitions(
	-DHAVE_QT5
)
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
if( WIN32 )
	install(
		TARGETS
		${PLUGIN}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}/plugins
		RUNTIME DESTINATION
		${CMAKE_INSTALL_PREFIX}/plugins
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
