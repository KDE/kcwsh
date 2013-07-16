# place for a qt based frontend

include_directories(${QT_INCLUDES})

add_definitions(${QT_DEFINITIONS})

set(kcwshqt_HDRS
    qterminalwidget.h
)

set(kcwshqt_SRCS
    qoutputwriter.cpp
    qinputreader.cpp
    qterminalwidget.cpp
)

qt4_wrap_cpp(kcwshqt_MOCS qterminalwidget_p.h qterminalwidget.h)

add_library(kcwshqt SHARED ${kcwshqt_SRCS} ${kcwshqt_MOCS})
target_link_libraries(kcwshqt kcwsh ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
install(TARGETS kcwshqt RUNTIME DESTINATION bin
                        ARCHIVE DESTINATION lib
                        LIBRARY DESTINATION lib)