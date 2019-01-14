cmake_minimum_required( VERSION 3.0.2 )
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(D "d")
	add_definitions(-DALLOW_QT_PLUGINS_DIR)
endif()

string(REGEX REPLACE "([^ ]+)[/\\].*" "\\1" QT_BIN_DIR_TMP "${QT_MOC_EXECUTABLE}")
string(REGEX REPLACE "\\\\" "/" QT_BIN_DIR "${QT_BIN_DIR_TMP}")
unset(QT_BIN_DIR_TMP)
get_filename_component(QT_DIR "${QT_BIN_DIR}" DIRECTORY)
set(QT_PLUGINS_DIR ${QT_DIR}/plugins)
set(QT_TRANSLATIONS_DIR ${QT_DIR}/translations)

function(copy SOURCE DEST TARGET)
	if(EXISTS ${SOURCE})
		set(OUT_TARGET_FILE "${CMAKE_BINARY_DIR}/${TARGET}.cmake")

		string(REGEX REPLACE "\\\\+" "/" DEST "${DEST}")
		string(REGEX REPLACE "\\\\+" "/" SOURCE "${SOURCE}")

		if(NOT TARGET ${TARGET})
			file(REMOVE "${OUT_TARGET_FILE}")
			add_custom_target(
				${TARGET}
				COMMAND
				${CMAKE_COMMAND} -P "${OUT_TARGET_FILE}"
			)
		endif()

		if(IS_DIRECTORY ${SOURCE})
			# copy directory
			file(GLOB_RECURSE FILES "${SOURCE}/*")
			get_filename_component(SOURCE_DIR_NAME ${SOURCE} NAME)

			foreach(FILE ${FILES})
				file(RELATIVE_PATH REL_PATH ${SOURCE} ${FILE})
				set(REL_PATH "${SOURCE_DIR_NAME}/${REL_PATH}")
				get_filename_component(REL_PATH ${REL_PATH} DIRECTORY)
				set(DESTIN "${DEST}/${REL_PATH}")

				string(REGEX REPLACE "/+" "/" DESTIN ${DESTIN})
				string(REGEX REPLACE "/+" "/" FILE ${FILE})

				file(APPEND
					"${OUT_TARGET_FILE}"
					"file(INSTALL \"${FILE}\" DESTINATION \"${DESTIN}\" USE_SOURCE_PERMISSIONS)\n")
			endforeach()
		else()
			string(REPLACE "//" "/" DEST ${DEST})
			if(DEST MATCHES "/$")
				set(DIR "${DEST}")
				string(REGEX REPLACE "^(.+)/$" "\\1" DIR ${DIR})
			else()
				# need to copy and rename
				get_filename_component(DIR ${DEST} DIRECTORY)
				get_filename_component(FILENAME ${DEST} NAME)
				get_filename_component(SOURCE_FILENAME ${SOURCE} NAME)
			endif()
			file(APPEND
				"${OUT_TARGET_FILE}"
				"file(INSTALL \"${SOURCE}\" DESTINATION \"${DIR}\" USE_SOURCE_PERMISSIONS)\n")
			if(DEFINED FILENAME)
				file(APPEND
					"${OUT_TARGET_FILE}"
					"file(RENAME \"${DIR}/${SOURCE_FILENAME}\" \"${DIR}/${FILENAME}\")\n")
			endif()
		endif()
	endif()
endfunction()

function(find_lib LIBLIST PATHES OUTPUT_PATH)
		set(_LIBS ${LIBLIST})
		set(_PATHES ${PATHES})
		set(_OUTPUT_PATH ${OUTPUT_PATH})
		set(FIXED_PATHES "")
		foreach(_path ${_PATHES})
			string(REGEX REPLACE "//bin" "/bin" tmp_path "${_path}")
			list(APPEND FIXED_PATHES ${tmp_path})
		endforeach()
		if(NOT USE_MXE)
			set(ADDITTIONAL_FLAG "NO_DEFAULT_PATH")
		else()
			set(ADDITTIONAL_FLAG "")
		endif()
		foreach(_liba ${_LIBS})
			set(_library _library-NOTFOUND)
			find_file( _library ${_liba} PATHS ${FIXED_PATHES} ${ADDITTIONAL_FLAG})
			if( NOT "${_library}" STREQUAL "_library-NOTFOUND" )
				message("library found ${_library}")
				copy("${_library}" "${_OUTPUT_PATH}" prepare-bin)
			endif()
		endforeach()
		set(_LIBS "")
		set(_PATHES "")
		set(_OUTPUT_PATH "")
endfunction()

find_program(WINDEPLOYQTBIN windeployqt ${QT_BIN_DIR})
if(NOT "${WINDEPLOYQTBIN}" STREQUAL "WINDEPLOYQTBIN-NOTFOUND")
	message(STATUS "WinDeployQt utility - FOUND")
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		list(APPEND WDARGS --debug)
	else()
		list(APPEND WDARGS --release)
	endif()
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${WINDEPLOYQTBIN}
		ARGS
		${WDARGS}
		$<TARGET_FILE:${PROJECT_NAME}>
		WORKING_DIRECTORY
		${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
		COMMENT
		"Preparing Qt runtime dependencies"
	)
else()
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
				list(APPEND ICU_LIBS
					"${icu_prefix}${icu_counter}.dll"
				)
			endif()
		endforeach()
	endforeach()
	find_lib("${ICU_LIBS}" "${QT_BIN_DIR}" "${EXECUTABLE_OUTPUT_PATH}/")
	set(QT_LIBS
		Qt5Core${D}.dll
		Qt5Gui${D}.dll
		Qt5Widgets${D}.dll
		Qt5Network${D}.dll
		Qt5Xml${D}.dll
		Qt5XmlPatterns${D}.dll
		Qt5Multimedia${D}.dll
		Qt5MultimediaWidgets${D}.dll
		Qt5Concurrent${D}.dll
		Qt5OpenGL${D}.dll
		Qt5WinExtras${D}.dll
	)
	find_lib("${QT_LIBS}" "${QT_BIN_DIR}" "${EXECUTABLE_OUTPUT_PATH}/")
	find_lib("qtaudio_windows${D}.dll" "${QT_PLUGINS_DIR}/audio/" "${EXECUTABLE_OUTPUT_PATH}/audio/")
	set(PLATFORM_PLUGS
		qminimal${D}.dll
		qoffscreen${D}.dll
		qwindows${D}.dll
	)
	find_lib("${PLATFORM_PLUGS}" "${QT_PLUGINS_DIR}/platforms/" "${EXECUTABLE_OUTPUT_PATH}/platforms/")
	set(BEARER_PLUGS
		qgenericbearer${D}.dll
		qnativewifibearer${D}.dll
	)
	find_lib("${BEARER_PLUGS}" "${QT_PLUGINS_DIR}/bearer/" "${EXECUTABLE_OUTPUT_PATH}/bearer/")
	find_lib("qtmultimedia_m3u${D}.dll" "${QT_PLUGINS_DIR}/playlistformats/" "${EXECUTABLE_OUTPUT_PATH}/playlistformats/")
	set(MEDIA_PLUGS
		dsengine${D}.dll
		qtmedia_audioengine${D}.dll
	)
	find_lib("${MEDIA_PLUGS}" "${QT_PLUGINS_DIR}/mediaservice/" "${EXECUTABLE_OUTPUT_PATH}/mediaservice/")
	set(IMAGE_PLUGS
		qgif${D}.dll
		qjpeg${D}.dll
	)
	find_lib("${IMAGE_PLUGS}" "${QT_PLUGINS_DIR}/imageformats/" "${EXECUTABLE_OUTPUT_PATH}/imageformats/")
	set(GENERIC_PLUGS
		qtuiotouchplugin${D}.dll
	)
	find_lib("${GENERIC_PLUGS}" "${QT_PLUGINS_DIR}/generic/" "${EXECUTABLE_OUTPUT_PATH}/generic/")
endif()
#install mingw and other needed libs
set( FILE_LIST
	libgcc_s_sjlj-1.dll
	libgcc_s_dw2-1.dll
	libgcc_s_seh-1.dll
	libstdc++-6.dll
	libwinpthread-1.dll
)

if(USE_MXE)
	list(APPEND FILE_LIST
		libbz2.dll
		libfreetype-6.dll
		libglib-2.0-0.dll
		libharfbuzz-0.dll
		libharfbuzz-icu-0.dll
		libiconv-2.dll
		libintl-8.dll
		libjasper-1.dll
		libjpeg-9.dll
		liblcms2-2.dll
		liblzma-5.dll
		liblzo2-2.dll
		libmng-2.dll
		libpcre16-0.dll
		libpcre2-16-0.dll
		libpcre-1.dll
		libpng16-16.dll
		libtiff-5.dll
		libssl-1_1-x64.dll
		libcrypto-1_1-x64.dll
		libssl-1_1.dll
		libcrypto-1_1.dll
	)
endif()

list(APPEND FILE_LIST
	libzlib.dll
	zlib1.dll
	libeay32.dll
	ssleay32.dll
	libtag.dll
	libcue.dll
)

set(ZLIB_ROOT "c:/build/zlib/" CACHE PATH "Path to zlib root dir")
set(OPENSSL_ROOT "c:/build/openssl/" CACHE PATH "Path to zlib root dir")

set(LIB_PATHS
	"${TAGLIB_ROOT}/bin"
	"${LIBCUE_ROOT}/bin"
	"${ZLIB_ROOT}/bin"
	"${OPENSSL_ROOT}/bin"
)
if(USE_MXE)
	list(APPEND LIB_PATHS
		${CMAKE_PREFIX_PATH}/bin
		${CMAKE_PREFIX_PATH}/lib
	)
endif()

find_lib("${FILE_LIST}" "${LIB_PATHS}" "${EXECUTABLE_OUTPUT_PATH}/")
