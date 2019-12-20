
add_library(NeonLib SHARED stdlib.cpp)

add_custom_command(TARGET NeonLib POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:NeonLib> ${PROJECT_SOURCE_DIR}/)
