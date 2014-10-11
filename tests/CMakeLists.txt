include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# simple applications that provoke a specific behaviour
add_executable(scrolltester scrolltester.cpp)
add_executable(titletester titletester.cpp)

add_executable(terminaltest terminaltest.cpp)
target_link_libraries(terminaltest kcwsh)
add_test(NAME simpletest COMMAND terminaltest simple echo)
add_test(NAME titletest COMMAND terminaltest title $<TARGET_FILE:titletester> )
add_test(NAME resizetest COMMAND terminaltest resize echo)
