if (LIBCUE_INCLUDE_DIR AND LIBCUE_LIBRARY)
	# in cache already
	set(LIBCUE_FIND_QUIETLY TRUE)
endif ()

if ( UNIX AND NOT( APPLE OR CYGWIN ) )
	find_package( PkgConfig QUIET )
	pkg_check_modules( PC_LIBCUE QUIET libcue )
	set ( LIBCUE_DEFINITIONS ${PC_LIBCUE_CFLAGS_OTHER} )
endif ()

set ( LIBINCS 
	libcue.h
)

find_path(
	LIBCUE_INCLUDE_DIR ${LIBINCS}
	HINTS
	${LIBCUE_ROOT}/include
	${PC_LIBCUE_INCLUDEDIR}
	${PC_LIBCUE_INCLUDE_DIRS}
	PATH_SUFFIXES
	""
	if ( NOT WIN32 )
	libcue
	libcue-1.4/libcue
	endif ()
)

find_library(
	LIBCUE_LIBRARY
	NAMES cue
	HINTS 
	${PC_LIBCUE_LIBDIR}
	${PC_LIBCUE_LIBRARY_DIRS}
	${LIBCUE_ROOT}/lib
	${LIBCUE_ROOT}/bin
)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
				LibCue
				DEFAULT_MSG
				LIBCUE_LIBRARY
				LIBCUE_INCLUDE_DIR
)
if ( LIBCUE_FOUND )
	set ( LIBCUE_LIBRARIES ${LIBCUE_LIBRARY} )
	set ( LIBCUE_INCLUDE_DIRS ${LIBCUE_INCLUDE_DIR} )
endif ()

mark_as_advanced( LIBCUE_INCLUDE_DIR LIBCUE_LIBRARY )
