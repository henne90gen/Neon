
add_library(NeonStd STATIC stdlib.cpp)

add_custom_command(TARGET NeonStd POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:NeonStd> ${PROJECT_SOURCE_DIR}/neon-build)
