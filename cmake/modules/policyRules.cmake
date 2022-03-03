cmake_minimum_required( VERSION 3.7.0 )
if(POLICY CMP0071)
    if(${CMAKE_VERSION} VERSION_LESS "3.10.0")
        cmake_policy(SET CMP0071 OLD)
        message(STATUS "CMP0071 policy set to OLD")
    else()
        cmake_policy(SET CMP0071 NEW)
    endif()
endif()
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif() 
