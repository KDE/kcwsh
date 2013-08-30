# place for a Qt based example application
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../frontends/qt
${QT_INCLUDES})

add_definitions(${QT_DEFINITIONS})

set(qkcwsh_HDRS
    mainwindow.h
)
set(qkcwsh_SRCS
    mainwindow.cpp
    main.cpp
)

qt4_wrap_cpp(qkcwsh_MOCS ${qkcwsh_HDRS})

add_executable(qkcwsh WIN32 ${qkcwsh_SRCS} ${qkcwsh_MOCS})
target_link_libraries(qkcwsh ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTMAIN_LIBRARY} kcwshqt)
install(TARGETS qkcwsh RUNTIME DESTINATION bin
                       LIBRARY DESTINATION lib
                       ARCHIVE DESTINATION lib)
