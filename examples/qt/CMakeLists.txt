# place for a Qt based example application
set(qkcwsh_SRCS
    main.cpp
)

add_executable(qkcwsh WIN32 ${qkcwsh_SRCS})
target_link_libraries(qkcwsh ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
install(TARGETS qkcwsh RUNTIME DESTINATION bin
                       LIBRARY DESTINATION lib
                       ARCHIVE DESTINATION lib)
