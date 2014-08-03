if (TAGLIB_INCLUDE_DIR AND TAGLIB_LIBRARY)
	# in cache already
	set(TAGLIB_FIND_QUIETLY TRUE)
endif ()

if ( UNIX AND NOT( APPLE OR CYGWIN ) )
	find_package( PkgConfig QUIET )
	pkg_check_modules( PC_TAGLIB QUIET taglib )
	set ( TAGLIB_DEFINITIONS ${PC_TAGLIB_CFLAGS_OTHER} )
endif ( UNIX AND NOT( APPLE OR CYGWIN ) )

find_path(
	TAGLIB_INCLUDE_DIR taglib.h
	HINTS
	${TAGLIB_ROOT}/include
	${PC_TAGLIB_INCLUDEDIR}
	${PC_TAGLIB_INCLUDE_DIRS}
	PATH_SUFFIXES
	""
	taglib
)

find_library(
	TAGLIB_LIBRARY
	NAMES taglib tag
	HINTS 
	${PC_TAGLIB_LIBDIR}
	${PC_TAGLIB_LIBRARY_DIRS}
	${TAGLIB_ROOT}/lib
	${TAGLIB_ROOT}/bin
)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
				TagLib
				DEFAULT_MSG
				TAGLIB_LIBRARY
				TAGLIB_INCLUDE_DIR
)
if ( TAGLIB_FOUND )
	set ( TAGLIB_LIBRARIES ${TAGLIB_LIBRARY} )
	set ( TAGLIB_INCLUDE_DIRS ${TAGLIB_INCLUDE_DIR} )
endif ( TAGLIB_FOUND )

mark_as_advanced( TAGLIB_INCLUDE_DIR TAGLIB_LIBRARY )
