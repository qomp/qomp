cmake_minimum_required( VERSION 2.8.12 )
if( CMAKE_VERSION VERSION_GREATER 3.0.0 )
	cmake_policy(SET CMP0028 OLD)
endif()

if( ${USE_QT5} )
	#install taglib
	find_package( TagLib )
	if( TAGLIB_FOUND )
		find_file( _libtag libtag.dll PATHES ${TAGLIB_ROOT}/bin ${TAGLIB_INCLUDE_DIR}/../bin )
		if( NOT "${_libtag}" STREQUAL "_libtag-NOTFOUND" )
			INSTALL( FILES ${_libtag} DESTINATION ${CMAKE_INSTALL_PREFIX} )
		endif()
	endif()
	#install libcue
	find_package( LibCue )
	if( LIBCUE_FOUND )
		find_file( _libcue libcue.dll PATHES ${LIBCUE_ROOT}/bin ${LIBCUE_INCLUDE_DIR}/../bin )
		if( NOT "${_libcue}" STREQUAL "_libcue-NOTFOUND" )
			INSTALL( FILES ${_libcue} DESTINATION ${CMAKE_INSTALL_PREFIX} )
		endif()
	endif()
	#install OpenSSL
	set( OPENSSL_ROOT_DIR "c:\\build\\psibuild\\psideps\\Qt5\\openssl" CACHE STRING "OpenSSL root path" )
	set( ZLIB_ROOT "c:\\build\\psibuild\\psideps\\Qt5\\zlib" CACHE STRING "ZLIB root path" )
	find_package( OpenSSL QUIET )
	if( OPENSSL_FOUND )
		find_file( _libssl ssleay32.dll PATHES ${OPENSSL_ROOT_DIR}/bin ${OPENSSL_ROOT_DIR}/lib )
		if( NOT "${_libssl}" STREQUAL "_libssl-NOTFOUND" )
			INSTALL( FILES ${_libssl} DESTINATION ${CMAKE_INSTALL_PREFIX} )
		endif()
		find_file( _libeay libeay32.dll PATHES ${OPENSSL_ROOT_DIR}/bin ${OPENSSL_ROOT_DIR}/lib )
		if( NOT "${_libeay}" STREQUAL "_libeay-NOTFOUND" )
			INSTALL( FILES ${_libeay} DESTINATION ${CMAKE_INSTALL_PREFIX} )
		endif()
	endif()
	#install ZLIB
	find_package( ZLIB QUIET )
	if( ZLIB_FOUND )
		find_file( _libz zlib1.dll PATHES ${ZLIB_ROOT}/bin ${ZLIB_ROOT}/lib )
		if( NOT "${_libz}" STREQUAL "_libz-NOTFOUND" )
			INSTALL( FILES ${_libz} DESTINATION ${CMAKE_INSTALL_PREFIX} )
		endif()
	endif()
	#install Qt5 libs and plugins
	set( qt5_components
		Core
		Gui
		Network
		Multimedia
		Xml
		Widgets
		Concurrent
		WinExtras
	)
	find_package( Qt5 COMPONENTS ${qt5_components} REQUIRED )
	foreach( liba ${qt5_components} )
		get_target_property( _libloc Qt5::${liba} LOCATION )
		if( _libloc )
			INSTALL( FILES ${_libloc} DESTINATION ${CMAKE_INSTALL_PREFIX} )
		endif()
	endforeach()
	set( INST_PLATFORM_PLUGS
		QMinimalIntegrationPlugin
		QWindowsIntegrationPlugin
	)
	foreach( plug ${INST_PLATFORM_PLUGS} )
		get_target_property( _libloc Qt5::${plug} LOCATION )
		if( _libloc )
			INSTALL( FILES ${_libloc} DESTINATION ${CMAKE_INSTALL_PREFIX}/platforms )
		endif()
	endforeach()
	set( INST_NETWORK_PLUGS
		QNativeWifiEnginePlugin
		QGenericEnginePlugin
	)
	foreach( plug ${INST_NETWORK_PLUGS} )
		get_target_property( _libloc Qt5::${plug} LOCATION )
		if( _libloc )
			INSTALL( FILES ${_libloc} DESTINATION ${CMAKE_INSTALL_PREFIX}/bearer )
		endif()
	endforeach()
	get_target_property( _audio Qt5::QWindowsAudioPlugin LOCATION )
	if( _audio )
		INSTALL( FILES ${_audio} DESTINATION ${CMAKE_INSTALL_PREFIX}/audio )
	endif()
	get_target_property( _plformats Qt5::QM3uPlaylistPlugin LOCATION )
	if( _plformats )
		INSTALL( FILES ${_plformats} DESTINATION ${CMAKE_INSTALL_PREFIX}/playlistformats )
	endif()
	set( INST_MEDIA_PLUGS
		AudioCaptureServicePlugin
		DSServicePlugin
	)
	foreach( plug ${INST_MEDIA_PLUGS} )
		get_target_property( _libloc Qt5::${plug} LOCATION )
		if( _libloc )
			INSTALL( FILES ${_libloc} DESTINATION ${CMAKE_INSTALL_PREFIX}/mediaservice )
		endif()
	endforeach()
	#install mingw and other needed libs
	set( ICU_LIBS_PREFIXES
		icudt5
		icuin5
		icuuc5
	)
	set( ICU_LIBS "" )
	foreach( icu_prefix ${ICU_LIBS_PREFIXES} )
		foreach( icu_counter RANGE 9 )
			find_file( ${icu_prefix}${icu_counter} "${icu_prefix}${icu_counter}.dll" )
			if( NOT "${${icu_prefix}${icu_counter}}" STREQUAL "${icu_prefix}${icu_counter}-NOTFOUND" )
				set( ICU_LIBS ${ICU_LIBS} ${${icu_prefix}${icu_counter}} )
			endif()
		endforeach()
	endforeach()
	if( ICU_LIBS )
		INSTALL( FILES ${ICU_LIBS} DESTINATION ${CMAKE_INSTALL_PREFIX} )
	endif()

	set( FILE_LIST
		libgcc_s_sjlj-1.dll
		libgcc_s_dw2-1.dll
		libstdc++-6.dll
		libwinpthread-1.dll
	)
	set( inc 1 )
	foreach( liba ${FILE_LIST} )
		find_file( ${liba}${inc} ${liba} )
		if( NOT "${${liba}${inc}}" STREQUAL "${liba}${inc}-NOTFOUND" )
			INSTALL( FILES ${${liba}${inc}} DESTINATION ${CMAKE_INSTALL_PREFIX} )
		endif()
		math( EXPR inc ${inc}+1)
	endforeach()
endif( ${USE_QT5} ) 
