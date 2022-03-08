set( PLUGIN tunetofileplugin )
cmake_minimum_required( VERSION 3.7.0 )

include(pluginsCommon)

set( tunetofileplugin_HDRS
	tunetofileplugin.h
	tunetofilesettings.h
)
set( tunetofileplugin_SRCS
	tunetofileplugin.cpp
	tunetofilesettings.cpp
)
set( tunetofileplugin_UIS
	tunetofilesettings.ui
)
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/include
)

set(QT_LIB_LIST Widgets)
find_qt_package("${QT_LIB_LIST}" REQUIRED )
add_qt_library(QT_DEPLIBS "${QT_LIB_LIST}")
wrap_qt_ui( UIS "${tunetofileplugin_UIS}" )

add_library(
	${PLUGIN}
	MODULE
	${tunetofileplugin_SRCS}
	${UIS}
)
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
