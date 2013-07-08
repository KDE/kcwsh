project(kcwsh)

cmake_minimum_required(VERSION 2.8)

option(BUILD_GUI_TEST "build a gui app for testing purposes" ON)
option(BUILD_TESTS "build unittests" ON)

if(BUILD_TESTS)
    enable_testing()
endif(BUILD_TESTS)

# required to check whether to build the kcwshtest
find_package(Qt4)

# this is my package I am allowed to do what I want!
add_definitions(-D_CRT_SECURE_NO_WARNINGS /DUNICODE)

#string(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
#string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

find_package(Kcw REQUIRED)

include_directories(${KCW_INCLUDE_DIRS})

set(kcwsh_SRCS
    dllmain.cpp
)

set(kcwsh_bin_SRCS
    main.cpp
)

add_library(kcwsh ${kcwsh_SRCS})
target_link_libraries(kcwsh ${KCW_LIBRARIES})

# as a test example use this little script
add_executable(kcwsh_bin ${kcwsh_bin_SRCS})
target_link_libraries(kcwsh_bin ${KCW_LIBRARIES})

install(TARGETS kcwsh kcwsh_bin RUNTIME DESTINATION bin
                                LIBRARY DESTINATION bin
                                ARCHIVE DESTINATION lib)

if(BUILD_TESTS)
#    add_subdirectory(tests)
endif(BUILD_TESTS)

if(BUILD_GUI_TEST AND QT4_FOUND)
    add_subdirectory(kcwshtest)
endif(BUILD_GUI_TEST AND QT4_FOUND)