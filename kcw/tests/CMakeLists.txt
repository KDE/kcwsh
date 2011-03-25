include_directories( .. )

add_library(commoninjector MODULE commoninjectiondll.cpp)
target_link_libraries(commoninjector kcw)

add_executable(debugtest debugtest.cpp)
target_link_libraries(debugtest kcw)
add_test(debugtest debugtest)

add_executable(sharedmemorytest sharedmemorytest.cpp)
target_link_libraries(sharedmemorytest kcw)
add_test(sharedmemorytest sharedmemorytest)

add_executable(injectortest injectortest.cpp)
target_link_libraries(injectortest kcw)
add_test(injectortest injectortest)
