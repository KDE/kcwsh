project(kcwsh)

cmake_minimum_required(VERSION 2.8)

option(BUILD_GUI_TEST "build a gui app for testing purposes" ON)
option(BUILD_TESTS "build unittests" ON)

# required to check whether to build the kcwshtest
find_package(Qt4)

# this is my package I am allowed to do what I want!
add_definitions(-D_CRT_SECURE_NO_WARNINGS /DUNICODE)

string(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

add_subdirectory(kcw)

include_directories(kcw)

set(kcwsh_SRCS
    clienthandler.cpp
    pipehandler.cpp
    main.cpp
)
add_executable(kcwsh ${kcwsh_SRCS})
target_link_libraries(kcwsh kcw)

set(kcwshhook_SRCS
    serverhandler.cpp
    remoteExec.cpp
    dllmain.cpp
)
add_library(kcwshhook SHARED ${kcwshhook_SRCS})
target_link_libraries(kcwshhook kcw)

install(TARGETS kcwsh kcwshhook RUNTIME DESTINATION bin
                                LIBRARY DESTINATION lib
                                ARCHIVE DESTINATION lib)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif(BUILD_TESTS)

if(BUILD_GUI_TEST AND QT4_FOUND)
    add_subdirectory(kcwshtest)
endif(BUILD_GUI_TEST AND QT4_FOUND)