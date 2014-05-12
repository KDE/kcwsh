project(kcwshtest)

cmake_minimum_required(VERSION 2.8)

option(GUIAPP "produce a gui app" ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 CONFIG Core Widgets)
if(NOT Qt5_FOUND)
	find_package(Qt4 REQUIRED)

	include_directories(${QT_INCLUDES})

	add_definitions(${QT_DEFINITIONS})
endif()

set(kcwshtest_SRCS
    main.cpp
    mywidget.cpp
)

if(GUIAPP)
    set(_GUI WIN32)
else(GUIAPP)
    set(_GUI)
endif(GUIAPP)

add_executable(kcwshtest ${_GUI} ${kcwshtest_SRCS})
if(Qt5_FOUND)
    target_link_libraries(kcwshtest Qt5::Core Qt5::Widgets)
else()
    target_link_libraries(kcwshtest ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
    if(GUIAPP)
        target_link_libraries(kcwshtest ${QT_QTMAIN_LIBRARY})
    endif()
endif()

install(TARGETS kcwshtest RUNTIME DESTINATION bin
                          LIBRARY DESTINATION lib
                          ARCHIVE DESTINATION lib)
