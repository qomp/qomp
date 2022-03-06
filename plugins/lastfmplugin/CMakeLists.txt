set( PLUGIN lastfmplugin )
cmake_minimum_required( VERSION 3.7.0 )

include(pluginsCommon)

set( lastfmplugin_HDRS
	${PLUGIN}.h
	lastfmsettings.h
	lastfmdefines.h
	lastfmauthdlg.h
)
set( lastfmplugin_SRCS
	${PLUGIN}.cpp
	lastfmsettings.cpp
	lastfmauthdlg.cpp
)
set( lastfmplugin_UIS
	lastfmsettings.ui
	lastfmauthdlg.ui
)
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/include
)

set(QT_LIB_LIST Widgets Network Xml)
find_qt_package("${QT_LIB_LIST}" REQUIRED )
add_qt_library(QT_DEPLIBS "${QT_LIB_LIST}")
wrap_qt_ui(UIS "${lastfmplugin_UIS}")

add_library( ${PLUGIN} MODULE ${lastfmplugin_SRCS} ${UIS} )
target_link_libraries(
	${PLUGIN}
	${QT_DEPLIBS}
	libqomp
)
add_dependencies( ${PLUGIN} libqomp qomp )
install(
	TARGETS
	${PLUGIN}
	LIBRARY DESTINATION ${PLUGINS_INSTALL_PATH}
	RUNTIME DESTINATION ${PLUGINS_INSTALL_PATH}
)
