project(kcwshtest)

cmake_minimum_required(VERSION 2.8)

option(GUIAPP "produce a gui app" ON)

find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES})

add_definitions(${QT_DEFINITIONS})

set(kcwshtest_SRCS
    main.cpp
    mywidget.cpp
)


qt4_wrap_cpp(kcwshtest_SRCS mywidget.h)

if(GUIAPP)
    set(_GUI WIN32)
else(GUIAPP)
    set(_GUI)
endif(GUIAPP)

add_executable(kcwshtest ${_GUI} ${kcwshtest_SRCS})

target_link_libraries(kcwshtest ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
if(GUIAPP)
    target_link_libraries(kcwshtest ${QT_QTMAIN_LIBRARY})
endif(GUIAPP)

install(TARGETS kcwshtest RUNTIME DESTINATION bin
                          LIBRARY DESTINATION lib
                          ARCHIVE DESTINATION lib)