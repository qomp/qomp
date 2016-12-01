cmake_minimum_required( VERSION 2.8.12 )

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
)

if( DEV_MODE )
	set( LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/qomp/plugins" )
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
		if( ${plugin} STREQUAL ${subdir} )
			message( "Parse subdirectory: ${plugin}" )
			add_subdirectory( "${plugin}" )
		endif()
	endforeach( subdir )
endforeach( plugin )



