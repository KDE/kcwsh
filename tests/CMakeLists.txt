include_directories(${CMAKE_SOURCE_DIR})

add_library(testinjector MODULE testinjectiondll.cpp)
target_link_libraries(testinjector kcw)

add_executable(readoutputbuffertest readoutputbuffertest.cpp)
target_link_libraries(readoutputbuffertest kcw)
#add_test(readoutputbuffertest readoutputbuffertest)

add_executable(threadtest threadtest.cpp)
target_link_libraries(threadtest kcw)