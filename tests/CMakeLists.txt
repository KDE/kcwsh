include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(terminaltest terminaltest.cpp)
target_link_libraries(terminaltest kcwsh)
add_test(NAME simpletest COMMAND terminaltest echo)

