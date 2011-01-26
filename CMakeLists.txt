project(kcwsh)

cmake_minimum_required(VERSION 2.8)

set(kcwsh_SRCS
    clienthandler.cpp
    pipehandler.cpp
    main.cpp
)
add_executable(kcwsh ${kcwsh_SRCS})

set(kcwshhook_SRCS
    dllmain.cpp
)
add_library(kcwshhook MODULE ${kcwshhook_SRCS})

install(TARGETS kcwsh kcwshhook RUNTIME DESTINATION bin
                                LIBRARY DESTINATION lib
                                ARCHIVE DESTINATION lib)