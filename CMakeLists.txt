project(kcwsh)

cmake_minimum_required(VERSION 2.8)

option(BUILD_GUI_TEST "build a gui app for testing purposes" ON)

# required to check whether to build the kcwshtest
find_package(Qt4)

# this is my package I am allowed to do what I want!
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set(kcwsh_SRCS
    clienthandler.cpp
    pipehandler.cpp
    kcwapp.cpp
    main.cpp
)
add_executable(kcwsh ${kcwsh_SRCS})

set(kcwshhook_SRCS
    serverhandler.cpp
    dllmain.cpp
)
add_library(kcwshhook MODULE ${kcwshhook_SRCS})

install(TARGETS kcwsh kcwshhook RUNTIME DESTINATION bin
                                LIBRARY DESTINATION lib
                                ARCHIVE DESTINATION lib)
if(BUILD_GUI_TEST AND QT4_FOUND)
    add_subdirectory(kcwshtest)
endif(BUILD_GUI_TEST AND QT4_FOUND)