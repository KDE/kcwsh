add_subdirectory(cmd)

if(Qt5_FOUND OR QT4_FOUND)
    add_subdirectory(qt)
endif()
