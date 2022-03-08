set( PLUGIN poiskmplugin )
cmake_minimum_required( VERSION 3.7.0 )

include(pluginsCommon)

set( poiskmplugin_HDRS
	${PLUGIN}.h
	poiskmplugingettunesdialog.h
	poiskmplugindefines.h
	poiskmcontroller.h
	poiskmurlresolvestrategy.h
)
set( poiskmplugin_SRCS
	${PLUGIN}.cpp
	poiskmplugingettunesdialog.cpp
	poiskmcontroller.cpp
	poiskmurlresolvestrategy.cpp
)
set( poiskmplugin_UIS
	poiskmpluginresultswidget.ui
)
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/include
)

set(QT_LIB_LIST Widgets Network)
find_qt_package("${QT_LIB_LIST}" REQUIRED )
add_qt_library(QT_DEPLIBS "${QT_LIB_LIST}")
if(USE_QT6)
	find_qt_package(Core5Compat REQUIRED)
	add_qt_library(QT_DEPLIBS Core5Compat)
endif()
wrap_qt_ui( UIS "${poiskmplugin_UIS}" )

add_library(
	${PLUGIN}
	MODULE
	${poiskmplugin_SRCS}
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
