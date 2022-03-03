if(UNIX AND NOT (APPLE OR CYGWIN))
	find_program(QTCHOOSER_PATH qtchooser DOC "Path to qtchooser")
	if(NOT "${QTCHOOSER_PATH}" STREQUAL "QTCHOOSER_PATH-NOTFOUND")
		message(STATUS "Found qtchooser: ${QTCHOOSER_PATH}")
		execute_process(COMMAND "${QTCHOOSER_PATH}" -print-env
				OUTPUT_VARIABLE Qt5_ENV)
		string(REGEX MATCHALL "QTLIBDIR=\"?([^\"]+)\"?" QT5LIBDIR ${Qt5_ENV})
		if(CMAKE_MATCH_1)
			if(EXISTS ${CMAKE_MATCH_1})
				message(STATUS "QTLIBDIR=${CMAKE_MATCH_1}")
				set(CMAKE_PREFIX_PATH "${CMAKE_MATCH_1}/cmake")
				set(Qt5_DIR "${CMAKE_MATCH_1}")
			endif()
		endif()
		string(REGEX MATCHALL "QTTOOLDIR=\"?([^\"]+)\"?" QT5TOOLDIR ${Qt5_ENV})
		if(CMAKE_MATCH_1)
			if(EXISTS ${CMAKE_MATCH_1})
				message(STATUS "QTTOOLDIR=${CMAKE_MATCH_1}")
				set(QT_QMAKE_EXECUTABLE "${CMAKE_MATCH_1}/qmake")
			endif()
		endif()
	endif()
endif()
