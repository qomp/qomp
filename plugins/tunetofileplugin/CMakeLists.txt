set( PLUGIN tunetofileplugin )
cmake_minimum_required( VERSION 2.8.12 )
if( CMAKE_VERSION VERSION_GREATER 3.0.0 )
	cmake_policy(SET CMP0028 OLD)
endif()
set( CMAKE_AUTOMOC ON )
add_definitions( -Wall -DQT_PLUGIN )
set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}" )
if( UNIX AND NOT (APPLE OR CYGWIN) )
	set(LINUX ON)
endif()
set( tunetofileplugin_HDRS
	tunetofileplugin.h
	tunetofilesettings.h
)
set( tunetofileplugin_SRCS
	tunetofileplugin.cpp
	tunetofilesettings.cpp
)
set( tunetofileplugin_UIS
	tunetofilesettings.ui
)
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/libqomp/include
)

find_package( Qt5Widgets REQUIRED )
add_definitions(
	-DHAVE_QT5
)
qt5_wrap_ui( UIS ${tunetofileplugin_UIS} )
set( QT_DEPLIBS
	Qt5::Widgets
)

add_library(
	${PLUGIN}
	SHARED
	${tunetofileplugin_SRCS}
	${UIS}
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
