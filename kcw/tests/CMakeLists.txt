include_directories( .. )

add_executable(debugtest debugtest.cpp)
target_link_libraries(debugtest kcw)
add_test(debugtest debugtest)
