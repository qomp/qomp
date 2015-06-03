cmake_minimum_required( VERSION 2.8.11 )
if( CMAKE_VERSION VERSION_GREATER 3.0.0 )
	cmake_policy(SET CMP0028 OLD)
endif( CMAKE_VERSION VERSION_GREATER 3.0.0 )
set( USE_MXE OFF CACHE BOOL "Use MXE (M cross environment) tools")
if( ${USE_QT5} )
	#install taglib
	find_package( TagLib )
	if( TAGLIB_FOUND )
		INSTALL( FILES ${TAGLIB_LIBRARY} DESTINATION ${CMAKE_INSTALL_PREFIX} )
	endif( TAGLIB_FOUND )
	#install Qt5 libs and plugins
	set( qt5_components
		Core
		Gui
		Network
		Multimedia
		Xml
		Widgets
		Concurrent
	)
	find_package( Qt5 COMPONENTS ${qt5_components} REQUIRED )
	set( qt_ver ${Qt5_VERSION_MAJOR}${Qt5_VERSION_MINOR} )
	foreach( liba ${qt5_components} )
		get_target_property( _libloc Qt5::${liba} LOCATION )
		if( _libloc )
			INSTALL( FILES ${_libloc} DESTINATION ${CMAKE_INSTALL_PREFIX} )
		endif( _libloc )
	endforeach( liba )
	set( INST_PLATFORM_PLUGS
		QMinimalIntegrationPlugin
		QWindowsIntegrationPlugin
	)
	foreach( plug ${INST_PLATFORM_PLUGS} )
		get_target_property( _libloc Qt5::${plug} LOCATION )
		if( _libloc )
			INSTALL( FILES ${_libloc} DESTINATION ${CMAKE_INSTALL_PREFIX}/platforms )
		endif( _libloc )
	endforeach( plug )
	set( INST_NETWORK_PLUGS
		QNativeWifiEnginePlugin
		QGenericEnginePlugin
	)
	foreach( plug ${INST_NETWORK_PLUGS} )
		get_target_property( _libloc Qt5::${plug} LOCATION )
		if( _libloc )
			INSTALL( FILES ${_libloc} DESTINATION ${CMAKE_INSTALL_PREFIX}/bearer )
		endif( _libloc )
	endforeach( plug )
	get_target_property( _audio Qt5::QWindowsAudioPlugin LOCATION )
	if( _audio )
		INSTALL( FILES ${_audio} DESTINATION ${CMAKE_INSTALL_PREFIX}/audio )
	endif( _audio )
	get_target_property( _plformats Qt5::QM3uPlaylistPlugin LOCATION )
	if( _plformats )
		INSTALL( FILES ${_plformats} DESTINATION ${CMAKE_INSTALL_PREFIX}/playlistformats )
	endif( _plformats )
	set( INST_MEDIA_PLUGS
		AudioCaptureServicePlugin
		DSServicePlugin
	)
	foreach( plug ${INST_MEDIA_PLUGS} )
		get_target_property( _libloc Qt5::${plug} LOCATION )
		if( _libloc )
			INSTALL( FILES ${_libloc} DESTINATION ${CMAKE_INSTALL_PREFIX}/mediaservice )
		endif( _libloc )
	endforeach( plug )
	#install mingw and other needed libs
	#version of icu libs sets automatically from Qt5 version
	set( FILE_LIST
		icudt${qt_ver}.dll
		icuin${qt_ver}.dll
		icuuc${qt_ver}.dll
		libeay32.dll
		libgcc_s_sjlj-1.dll
		libgcc_s_dw2-1.dll
		libstdc++-6.dll
		libwinpthread-1.dll
		zlib1.dll
		ssleay32.dll
	)
	if( ${USE_MXE} )
		set( MXE_NEEDED_LIBS
			libbz2.dll
			libfreetype-6.dll
			libglib-2.0-0.dll
			libharfbuzz-0.dll
			libiconv-2.dll
			libintl-8.dll
			libpcre16-0.dll
			libpcre-1.dll
			libpng16-16.dll
		)
		set( FILE_LIST
			${FILE_LIST}
			${MXE_NEEDED_LIBS}
		)
	endif( ${USE_MXE} )
	set( inc 1 )
	foreach( liba ${FILE_LIST} )
		find_file( _file${inc} ${liba} )
		if( _file${inc} )
			INSTALL( FILES ${_file${inc}} DESTINATION ${CMAKE_INSTALL_PREFIX} )
		endif( _file${inc} )
		math( EXPR inc ${inc}+1)
	endforeach( liba )
endif( ${USE_QT5} ) 
