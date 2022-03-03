cmake_minimum_required( VERSION 3.7.0 )

#CPack section start
set(CPACK_PACKAGE_VERSION ${QOMP_VERSION_STRING})
set(CPACK_PACKAGE_VERSION_MAJOR "${QOMP_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${QOMP_VERSION_MINOR}")
if(QOMP_VERSION_PATCH)
    set(CPACK_PACKAGE_VERSION_PATCH "${QOMP_VERSION_PATCH}")
endif()
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_CONTACT "Vitaly Tonkacheyev")
set(CPACK_MAINTAINER_EMAIL "thetvg@gmail.com")
set(PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT} <${CPACK_MAINTAINER_EMAIL}>")
set(CPACK_PACKAGE_VENDOR "Qomp Team")
set(CPACK_PACKAGE_DESCRIPTION "Quick(Qt) Online Music Player")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Quick(Qt) Online Music Player - one player for different online music hostings.
 main features:
 * search and play music from several online music hostings (Yande.Music, myzuka.ru, pleer.com);
 * play music from local filesystem;
 * Last.fm scrobbling;
 * MPRIS support(Linux only);
 * System tray integration;
 * proxy-server support;
 * playlists support;
 * crossplatform (Windows, OS X, Linux, Android)")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(PACKAGE_URL "http://qomp.sourceforge.net/")
if(WIN32 AND NOT UNIX)
    set(CPACK_GENERATOR NSIS)
    #set(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "${PROJECT_NAME}.exe")
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} Qomp")
    set(CPACK_NSIS_HELP_LINK "https:\\\\\\\\github.com\\\\qomp")
    set(CPACK_NSIS_URL_INFO_ABOUT "Qomp Sources")
    set(CPACK_NSIS_CONTACT "${CPACK_MAINTAINER_EMAIL}")
    set(CPACK_NSIS_MODIFY_PATH ON)
else()
    find_program(RPMB_PATH rpmbuild DOC "Path to rpmbuild")
    find_program(DPKG_PATH dpkg DOC "Path to dpkg")
    set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
    if(RPMB_PATH)
        set(CPACK_GENERATOR "RPM")
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
        set(CPACK_RPM_PACKAGE_LICENSE "GPL-2")
        set(CPACK_RPM_PACKAGE_GROUP "Applications/Sound")
        set(CPACK_RPM_SPEC_CHANGELOG "${PROJECT_SOURCE_DIR}/CHANGELOG.md")
        set(CPACK_RESOURCE_FILE_LICENSE "${CPACK_RESOURCE_FILE_LICENSE}")
        set(CPACK_RPM_PACKAGE_URL "${PACKAGE_URL}")
    endif()
    if(DPKG_PATH)
        set(CPACK_GENERATOR "DEB")
        exec_program("LANG=en date +'%a, %d %b %Y %T %z'"
            OUTPUT_VARIABLE BUILD_DATE
        )
        exec_program("date +'%Y'"
            OUTPUT_VARIABLE BUILD_YEAR
        )
        message(STATUS "Build date: ${BUILD_DATE}")
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${PACKAGE_MAINTAINER}")
        set(CPACK_DEBIAN_PACKAGE_SECTION "sound")
        exec_program("${DPKG_PATH} --print-architecture"
            OUTPUT_VARIABLE DEB_PKG_ARCH
        )
        if(NOT "${DEB_PKG_ARCH}" STREQUAL "")
            set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${DEB_PKG_ARCH}")
        endif()
        find_program(LSB_APP lsb_release DOC "Path to lsb_release")
        if(LSB_APP)
            exec_program("${LSB_APP} -is"
                OUTPUT_VARIABLE OSNAME
            )
            if(NOT "${OSNAME}" STREQUAL "")
                message(STATUS "Current system: ${OSNAME}")
                if("${OSNAME}" STREQUAL "Ubuntu")
                    set(PKG_OS_SUFFIX "-0ubuntu1~0ppa${CPACK_PACKAGE_RELEASE}~")
                endif()
            endif()
            exec_program("${LSB_APP} -cs"
                OUTPUT_VARIABLE OSCODENAME
            )
            if(NOT "${OSCODENAME}" STREQUAL "")
                message(STATUS "Debian codename: ${OSCODENAME}")
            endif()
        endif()
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5multimedia5-plugins, gstreamer1.0-x, gstreamer1.0-libav, gstreamer1.0-plugins-good, libssl1.0.0")
        set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
        if(NOT CPACK_DEBIAN_PACKAGE_VERSION)
            set(CPACK_DEBIAN_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}${PKG_OS_SUFFIX}${OSCODENAME}")
        endif()
        if(NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
            set(CPACK_PACKAGE_ARCHITECTURE "${CMAKE_SYSTEM_PROCESSOR}")
        else()
            set(CPACK_PACKAGE_ARCHITECTURE "${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
        endif()
        if(NOT CPACK_PACKAGE_FILE_NAME)
            set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_DEBIAN_PACKAGE_VERSION}_${CPACK_PACKAGE_ARCHITECTURE}")
        endif()
        configure_file(copyright-deb.in copyright @ONLY)
        set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_BINARY_DIR}/copyright")
    endif()
endif()
include(CPack)
#CPack section end
