# place for a qt based frontend

if(QT4_FOUND)
include_directories(${QT_INCLUDES})

add_definitions(${QT_DEFINITIONS})
endif()

set(kcwshqt_HDRS
    qterminalwidget.h
)

set(kcwshqt_SRCS
    qoutputwriter.cpp
    qinputreader.cpp
    qterminalwidget.cpp
)

add_library(kcwshqt SHARED ${kcwshqt_SRCS} ${kcwshqt_MOCS})
if(Qt5_FOUND)
    target_link_libraries(kcwshqt kcwsh Qt5::Widgets Qt5::Core)
else()
    target_link_libraries(kcwshqt kcwsh ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
endif()
install(TARGETS kcwshqt RUNTIME DESTINATION bin
                        ARCHIVE DESTINATION lib
                        LIBRARY DESTINATION lib)
