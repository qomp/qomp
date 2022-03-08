set( PLUGIN myzukaruplugin )
cmake_minimum_required( VERSION 3.7.0 )

include(pluginsCommon)

set( myzukaruplugin_HDRS
	${PLUGIN}.h
	myzukarudefines.h
	myzukarugettunesdlg.h
	myzukarucontroller.h
	myzukaruresolvestrategy.h
)
set( myzukaruplugin_SRCS
	${PLUGIN}.cpp
	myzukarugettunesdlg.cpp
	myzukarucontroller.cpp
	myzukaruresolvestrategy.cpp
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

add_library(
	${PLUGIN}
	MODULE
	${myzukaruplugin_SRCS}
)
target_link_libraries(
	${PLUGIN}
	${QT_DEPLIBS}
	libqomp
)
add_dependencies(${PLUGIN} libqomp qomp )
install(
	TARGETS
	${PLUGIN}
	LIBRARY DESTINATION ${PLUGINS_INSTALL_PATH}
	RUNTIME DESTINATION ${PLUGINS_INSTALL_PATH}
)
