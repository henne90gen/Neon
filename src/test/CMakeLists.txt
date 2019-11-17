include(${PROJECT_SOURCE_DIR}/vendor/catch/contrib/Catch.cmake)
set(CATCH_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/catch/single_include)

set(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LexerTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/ParserTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/FunctionTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/OperationTest.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/AstGeneratorTest.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/AstTestHelper.cpp)

add_executable(Tests ${TEST_SOURCES})
target_include_directories(Tests
                           PRIVATE ${CATCH_INCLUDE_DIR}
                                   ${PROJECT_SOURCE_DIR}/src/main)
target_link_libraries(Tests NeonParser NeonAstGenerator)
catch_discover_tests(Tests)
