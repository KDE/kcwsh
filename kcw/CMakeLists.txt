project(kcw)

cmake_minimum_required(VERSION 2.8)

option(BUILD_TESTS "build unittests" ON)

if(BUILD_TESTS)
    enable_testing()
endif(BUILD_TESTS)

set(kcw_HDRS
    kcwapp.h
    kcwdebug.h
    kcweventloop.h
    kcwinjector.h
    kcwprocess.h
    kcwthread.h
)

set(kcw_SRCS
    kcwapp.cpp
    kcwdebug.cpp
    kcweventloop.cpp
    kcwinjector.cpp
    kcwprocess.cpp
    kcwthread.cpp
)

add_library(kcw STATIC ${kcw_SRCS})

install(TARGETS kcw RUNTIME DESTINATION bin
                    LIBRARY DESTINATION lib
                    ARCHIVE DESTINATION lib)

install(FILES ${kcw_HDRS} DESTINATION include/kcw)

add_subdirectory(tests)