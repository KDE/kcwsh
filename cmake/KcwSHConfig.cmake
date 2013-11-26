# Config file for kcwsh
# It defines the following variables
# KCWSH_INCLUDE_DIRS  - include directories for kcwsh
# KCWSH_LIBRARIES     - libraries to link against

get_filename_component(KCWSH_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(NOT TARGET kcwsh)
    include(${KCWSH_CMAKE_DIR}/KcwSHTargets.cmake)
endif()

set(KCWSH_INCLUDE_DIRS ${KCWSH_CMAKE_DIR}/../../../include/kcwsh)
set(KCWSH_LIBRARIES kcwsh)
