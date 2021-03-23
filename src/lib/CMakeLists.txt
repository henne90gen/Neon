
add_library(NeonStd STATIC stdlib.cpp)

add_custom_command(TARGET NeonStd
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${NEON_BUILD_DIR})
add_custom_command(TARGET NeonStd
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:NeonStd> ${NEON_BUILD_DIR})
