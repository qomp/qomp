cmake_minimum_required( VERSION 3.0.2 )
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND WIN32)
	set(D "d")
	add_definitions(-DALLOW_QT_PLUGINS_DIR)
endif()

option(USE_MXE "Use MXE toolkit" OFF)

string(REGEX REPLACE "([^ ]+)[/\\].*" "\\1" QT_BIN_DIR_TMP "${QT_MOC_EXECUTABLE}")
string(REGEX REPLACE "\\\\" "/" QT_BIN_DIR "${QT_BIN_DIR_TMP}")
unset(QT_BIN_DIR_TMP)
get_filename_component(QT_DIR ${QT_BIN_DIR} DIRECTORY)
set(QT_PLUGINS_DIR ${QT_DIR}/plugins)

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


#install Qt5 libs and plugins
set(LIBS_LIST "")
copy("${QT_BIN_DIR}/Qt5Core${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/" prepare-bin)
copy("${QT_BIN_DIR}/Qt5Gui${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/" prepare-bin)
copy("${QT_BIN_DIR}/Qt5Widgets${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/" prepare-bin)
copy("${QT_BIN_DIR}/Qt5Network${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/" prepare-bin)
copy("${QT_BIN_DIR}/Qt5Xml${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/" prepare-bin)
copy("${QT_BIN_DIR}/Qt5XmlPatterns${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/" prepare-bin)
copy("${QT_BIN_DIR}/Qt5Multimedia${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/" prepare-bin)
copy("${QT_BIN_DIR}/Qt5MultimediaWidgets${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/" prepare-bin)
copy("${QT_BIN_DIR}/Qt5Concurrent${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/" prepare-bin)
copy("${QT_BIN_DIR}/Qt5OpenGL${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/" prepare-bin)
copy("${QT_BIN_DIR}/Qt5WinExtras${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/" prepare-bin)
copy("${QT_PLUGINS_DIR}/audio/qtaudio_windows${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/audio/" prepare-bin)
copy("${QT_PLUGINS_DIR}/platforms/qminimal${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/platforms/" prepare-bin)
copy("${QT_PLUGINS_DIR}/platforms/qoffscreen${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/platforms/" prepare-bin)
copy("${QT_PLUGINS_DIR}/platforms/qwindows${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/platforms/" prepare-bin)
copy("${QT_PLUGINS_DIR}/bearer/qgenericbearer${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/bearer/" prepare-bin)
copy("${QT_PLUGINS_DIR}/bearer/qnativewifibearer${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/bearer/" prepare-bin)
copy("${QT_PLUGINS_DIR}/playlistformats/qtmultimedia_m3u${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/playlistformats/" prepare-bin)
copy("${QT_PLUGINS_DIR}/mediaservice/dsengine${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/mediaservice/" prepare-bin)
copy("${QT_PLUGINS_DIR}/mediaservice/qtmedia_audioengine${D}.dll" "${EXECUTABLE_OUTPUT_PATH}/mediaservice/" prepare-bin)

#install mingw and other needed libs
set( FILE_LIST
	libgcc_s_sjlj-1.dll
	libgcc_s_dw2-1.dll
	libgcc_s_seh-1.dll
	libstdc++-6.dll
	libwinpthread-1.dll
)
set( ICU_LIBS_PREFIXES
	icudt5
	icuin5
	icuuc5
)

foreach( icu_prefix ${ICU_LIBS_PREFIXES} )
	foreach( icu_counter RANGE 9 )
		find_file( ${icu_prefix}${icu_counter} "${icu_prefix}${icu_counter}.dll" )
		if( EXISTS "${${icu_prefix}${icu_counter}}" )
			copy("${${icu_prefix}${icu_counter}}" "${EXECUTABLE_OUTPUT_PATH}/" prepare-bin)
		endif()
	endforeach()
endforeach()

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
		libpcre-1.dll
		libpng16-16.dll
		libtiff-5.dll
	)
endif()

list(APPEND FILE_LIST
	zlib1.dll
	libeay32.dll
	ssleay32.dll
	libtag.dll
	libcue.dll
)

set(ZLIB_ROOT "c:/build/zlib/" CACHE PATH "Path to zlib root dir")
set(OPENSSL_ROOT "c:/build/openssl/" CACHE PATH "Path to zlib root dir")

set(LIB_PATHES
	"${TAGLIB_ROOT}/bin"
	"${LIBCUE_ROOT}/bin"
	"${ZLIB_ROOT}/bin"
	"${OPENSSL_ROOT}/bin"
)

function(find_lib LIBLIST PATHES)
	set(FIXED_PATHES "")
	foreach(_path ${PATHES})
		string(REGEX REPLACE "[\\]" "/" tmp_path "${_path}")
		list(APPEND FIXED_PATHES ${tmp_path})
	endforeach()
	set( inc 1 )
	foreach(_liba ${LIBLIST})
		find_file( ${_liba}${inc} ${_liba} PATHS ${FIXED_PATHES})
		if( NOT "${${_liba}${inc}}" STREQUAL "${_liba}${inc}-NOTFOUND" )
			message("Library found at ${${_liba}${inc}}")
			copy("${${_liba}${inc}}" "${EXECUTABLE_OUTPUT_PATH}/" prepare-bin)
		endif()
		math( EXPR inc ${inc}+1)
	endforeach()
endfunction()

find_lib(${FILE_LIST})

