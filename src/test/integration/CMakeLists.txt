
add_executable(NeonTester main.cpp)
target_link_libraries(NeonTester PRIVATE NeonCompiler)

add_test(NAME IntegrationTests COMMAND NeonTester WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
