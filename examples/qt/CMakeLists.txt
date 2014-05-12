# place for a Qt based example application
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../frontends/qt)

if(QT4_FOUND)
    include_directories(${QT_INCLUDES})

    add_definitions(${QT_DEFINITIONS})
endif()

set(qkcwsh_HDRS
    mainwindow.h
)
set(qkcwsh_SRCS
    mainwindow.cpp
    main.cpp
)

add_executable(qkcwsh WIN32 ${qkcwsh_SRCS})
if(Qt5_FOUND)
    target_link_libraries(qkcwsh Qt5::Core Qt5::Widgets)
else()
    target_link_libraries(qkcwsh ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTMAIN_LIBRARY})
endif()

target_link_libraries(qkcwsh kcwshqt)
install(TARGETS qkcwsh RUNTIME DESTINATION bin
                       LIBRARY DESTINATION lib
                       ARCHIVE DESTINATION lib)

install(FILES list-add.png DESTINATION share/qkcwsh/icons)
