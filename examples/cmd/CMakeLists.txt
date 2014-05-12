set(kcwsh_bin_SRCS
    main.cpp
    consoleterminal.cpp
    consoleoutputwriter.cpp
    consoleinputreader.cpp
)

add_executable(kcwsh_bin ${kcwsh_bin_SRCS})
target_link_libraries(kcwsh_bin ${KCW_LIBRARIES} kcwsh)
set_target_properties(kcwsh_bin PROPERTIES RUNTIME_OUTPUT_NAME kcwsh AUTOMOC OFF)

install(TARGETS kcwsh_bin RUNTIME DESTINATION bin
                          ARCHIVE DESTINATION bin
			  LIBRARY DESTINATION lib)

