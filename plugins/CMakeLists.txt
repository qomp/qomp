cmake_minimum_required( VERSION 3.1.0 )

set( CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}" )

set( plugins_list
	filesystemplugin
	urlplugin
	prostopleerplugin
	myzukaruplugin
	notificationsplugin
	yandexmusicplugin
	lastfmplugin
	tunetofileplugin
	mprisplugin
	cuteradioplugin
)

if( DEV_MODE )
	if( WIN32 )
		#Store old runtime output path
		set(OLD_RUNTIME_TARGETS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
		#Set runtime output path for plugins dll-files
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/qomp/plugins")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")
	else()
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/qomp/plugins")
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/qomp/plugins")
	endif()
endif()

if( UNIX AND NOT (APPLE OR CYGWIN) )
	set(LINUX ON)
endif()

if( "${BUILD_PLUGINS}" STREQUAL "ALL" )
	set( plugins ${plugins_list} )
else()
	set ( plugins "${BUILD_PLUGINS}" )
endif()

foreach( plugin ${plugins_list} )
	foreach( subdir ${plugins} )
		if( ${plugin} STREQUAL ${subdir} AND ( EXISTS "${CMAKE_CURRENT_LIST_DIR}/${plugin}" ) )
			message(STATUS "Parse subdirectory: ${plugin}" )
			add_subdirectory( "${plugin}" )
		endif()
	endforeach( subdir )
endforeach( plugin )

if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OLD_RUNTIME_TARGETS}")
endif()

