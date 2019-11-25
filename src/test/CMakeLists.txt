include(${PROJECT_SOURCE_DIR}/vendor/catch/contrib/Catch.cmake)
set(CATCH_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/catch/single_include)

add_executable(LexerTests LexerTest.cpp)
target_include_directories(LexerTests
        PRIVATE ${CATCH_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/src/main)
target_link_libraries(LexerTests NeonParser NeonAstGenerator)
catch_discover_tests(LexerTests)

set(PARSER_TEST_SOURCES
        parser/ParserTest.cpp
        parser/FunctionTest.cpp
        parser/OperationTest.cpp)
add_executable(ParserTests ${PARSER_TEST_SOURCES})
target_include_directories(ParserTests
        PRIVATE ${CATCH_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/src/main)
target_link_libraries(ParserTests NeonParser NeonAstGenerator)
catch_discover_tests(ParserTests)

set(AST_GENERATOR_TEST_SOURCES
        ast/AstGeneratorTest.cpp
        ast/AstTestHelper.cpp)
add_executable(AstGeneratorTests ${AST_GENERATOR_TEST_SOURCES})
target_include_directories(AstGeneratorTests
        PRIVATE ${CATCH_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/src/main)
target_link_libraries(AstGeneratorTests NeonParser NeonAstGenerator)
catch_discover_tests(AstGeneratorTests)
