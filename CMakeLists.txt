project(kcwsh)

cmake_minimum_required(VERSION 2.8.12)

option(BUILD_GUI_TEST "build a gui app for testing purposes" ON)
option(BUILD_TESTS "build unittests" ON)

set(CMAKE_AUTOMOC ON)

if(BUILD_TESTS)
    enable_testing()
endif(BUILD_TESTS)

set(KCWSH_MAJOR_VERSION 0)
set(KCWSH_MINOR_VERSION 1)
set(KCWSH_PATCH_VERSION 0)

if(MSVC)
    add_definitions(/Zc:wchar_t-)
endif()

set(KCWSH_VERSION "${KCWSH_MAJOR_VERSION}.${KCWSH_MINOR_VERSION}.${KCWSH_PATCH_VERSION}")

configure_file(cmake/KcwSHConfigVersion.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/KcwSHConfigVersion.cmake" @ONLY)

# Install the KcwSHConfig.cmake and KcwSHConfigVersion.cmake
install(FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/KcwSHConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/KcwSHConfigVersion.cmake"
  DESTINATION lib/cmake/kcwsh COMPONENT dev)


# required to check whether to build the kcwshtest
find_package(Qt5 COMPONENTS Core Widgets CONFIG)
if(NOT Qt5_FOUND)
find_package(Qt4)
endif()

# this is my package I am allowed to do what I want!
add_definitions(-D_CRT_SECURE_NO_WARNINGS /DUNICODE)
if(MSVC)
    add_definitions(-wd4275 -wd4251)
endif(MSVC)

#string(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
#string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

find_package(Kcw REQUIRED)

include_directories(${KCW_INCLUDE_DIRS})

set(kcwsh_HDRS
    terminal.h
    outputwriter.h
    inputreader.h
    kcwsh_export.h
)

set(kcwsh_SRCS
    dllmain.cpp
    terminal.cpp
    inputwriter.cpp
    inputreader.cpp
    outputreader.cpp
    outputwriter.cpp
    hooks.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# kcwsh library always needs to be shared
add_library(kcwsh SHARED ${kcwsh_SRCS})
target_link_libraries(kcwsh ${KCW_LIBRARIES})
set_target_properties(kcwsh PROPERTIES PREFIX "" AUTOMOC OFF)

install(TARGETS kcwsh EXPORT KcwSHTargets
                      RUNTIME DESTINATION bin
                      LIBRARY DESTINATION bin
                      ARCHIVE DESTINATION lib)
install(FILES ${kcwsh_HDRS} DESTINATION include/kcwsh)

export(TARGETS kcwsh FILE "${CMAKE_CURRENT_BINARY_DIR}/KcwSHTargets.cmake")
export(PACKAGE kcwsh)

install(EXPORT KcwSHTargets DESTINATION lib/cmake/kcwsh COMPONENT devel)

add_subdirectory(frontends)
add_subdirectory(examples)

if(BUILD_TESTS)
   add_subdirectory(tests)
endif()

if(BUILD_GUI_TEST AND QT4_FOUND)
#    add_subdirectory(kcwshtest)
endif()
