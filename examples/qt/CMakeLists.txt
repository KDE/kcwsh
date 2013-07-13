# place for a Qt based example application
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../frontends/qt
${QT_INCLUDES})

add_definitions(${QT_DEFINITIONS})

set(qkcwsh_SRCS
    main.cpp
)

add_executable(qkcwsh WIN32 ${qkcwsh_SRCS})
target_link_libraries(qkcwsh ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTMAIN_LIBRARY})
install(TARGETS qkcwsh RUNTIME DESTINATION bin
                       LIBRARY DESTINATION lib
                       ARCHIVE DESTINATION lib)
