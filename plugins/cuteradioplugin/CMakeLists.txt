set( PLUGIN cuteradioplugin )
cmake_minimum_required( VERSION 3.1.0 )

if( POLICY CMP0071 )
        cmake_policy(SET CMP0071 OLD)
endif()

if( UNIX AND NOT (APPLE OR CYGWIN) )
	set(LINUX ON)
endif()
set( CMAKE_AUTOMOC TRUE )
add_definitions( -DQT_PLUGIN )
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
find_package( Qt5 COMPONENTS Widgets Network REQUIRED )
set( QT_DEPLIBS
	Qt5::Widgets
	Qt5::Network
)
qt5_wrap_ui( UIS ${cuteradioplugin_UIS} )
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
if( WIN32 )
	install(
		TARGETS
		${PLUGIN}
		LIBRARY
		DESTINATION
		${CMAKE_INSTALL_PREFIX}/plugins
		RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins
	)
endif()

if( LINUX )
	install(
		TARGETS
		${PLUGIN}
		LIBRARY
		DESTINATION
		${LIB_INSTALL_DIR}/qomp/plugins
		RUNTIME DESTINATION
		${LIB_INSTALL_DIR}/qomp/plugins
	)
endif()
