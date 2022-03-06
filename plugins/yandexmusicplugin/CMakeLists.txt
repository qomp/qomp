set( PLUGIN yandexmusicplugin )
cmake_minimum_required( VERSION 3.7.0 )

include(pluginsCommon)

set( yandexmusicplugin_HDRS
	${PLUGIN}.h
	yandexmusicgettunsdlg.h
	yandexmusicurlresolvestrategy.h
	yandexmusiccontroller.h
	yandexmusiccaptcha.h
	yandexmusicoauth.h
	yandexmusicsettings.h
)
set( yandexmusicplugin_SRCS
	${PLUGIN}.cpp
	yandexmusicgettunsdlg.cpp
	yandexmusicurlresolvestrategy.cpp
	yandexmusiccontroller.cpp
	yandexmusiccaptcha.cpp
	yandexmusicoauth.cpp
	yandexmusicsettings.cpp
)

set( yandexmusicplugin_UIS
	yandexmusicsettings.ui
)

include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/include
)

set(QT_LIB_LIST Widgets Network Xml)
find_qt_package("${QT_LIB_LIST}" REQUIRED )
add_qt_library(QT_DEPLIBS "${QT_LIB_LIST}")
wrap_qt_ui( UIS "${yandexmusicplugin_UIS}" )
if(USE_QT6)
	find_qt_package(Core5Compat REQUIRED)
	add_qt_library(QT_DEPLIBS Core5Compat)
endif()

add_library( ${PLUGIN} MODULE ${yandexmusicplugin_SRCS} ${UIS} )
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
