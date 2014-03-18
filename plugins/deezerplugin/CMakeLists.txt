project ( deezerplugin )
cmake_minimum_required( VERSION 2.8.6 )
set ( CMAKE_BUILD_TYPE Release )
set ( CMAKE_AUTOMOC TRUE )
add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTGUI_INCLUDE_DIR}
	${QT_QTXML_INCLUDE_DIR}
	../../libqomp/include
)
set ( deezerplugin_HDRS
	deezerplugin.h
	deezergettunesdlg.h
	deezerplugindefines.h
	deezerauth.h
)

set ( deezerplugin_SRCS
	deezerplugin.cpp
	deezergettunesdlg.cpp
	deezerauth.cpp
)

add_library ( ${PROJECT_NAME} SHARED ${deezerplugin_SRCS} ${deezerplugin_HDRS} )
target_link_libraries ( ${PROJECT_NAME}  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} )
