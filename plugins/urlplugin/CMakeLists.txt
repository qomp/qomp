set( PLUGIN urlplugin )
cmake_minimum_required( VERSION 2.8.12 )
#if( CMAKE_VERSION VERSION_GREATER 3.0.0 )
#	cmake_policy(SET CMP0028 OLD)
#endif()
add_definitions( -DQT_PLUGIN )
if( UNIX AND NOT (APPLE OR CYGWIN) )
	set(LINUX ON)
endif()
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
	${urlplugin_SRCS}
)
target_link_libraries(
	${PLUGIN}
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
