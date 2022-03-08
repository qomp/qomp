set( PLUGIN cuteradioplugin )
cmake_minimum_required( VERSION 3.7.0 )

include(pluginsCommon)

set( cuteradioplugin_HDRS
	cuteradioplugingettunesdialog.h
	cuteradioplugindefines.h
	cuteradiocontroller.h
	cuteradiomodel.h
	${PLUGIN}.h
)
set( cuteradioplugin_SRCS
	cuteradiocontroller.cpp
	cuteradiomodel.cpp
	cuteradioplugingettunesdialog.cpp
	${PLUGIN}.cpp
)
set(cuteradioplugin_UIS
	cuteradioplugingettunesdialog.ui
)
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/include
)
set(QT_LIB_LIST Widgets Network )
find_qt_package("${QT_LIB_LIST}" REQUIRED )
add_qt_library(QT_DEPLIBS "${QT_LIB_LIST}")
wrap_qt_ui( UIS "${cuteradioplugin_UIS}" )

add_library(
	${PLUGIN}
	MODULE
	${cuteradioplugin_SRCS}
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
