include(${PROJECT_SOURCE_DIR}/vendor/catch/contrib/Catch.cmake)
set(CATCH_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/catch/single_include)

add_executable(Tests
        main.cpp
        LexerTest.cpp
        parser/ParserTest.cpp
        parser/FunctionTest.cpp
        parser/OperationTest.cpp
        ast/AstGeneratorTest.cpp
        ast/AstTestHelper.cpp)
target_include_directories(Tests
        PRIVATE ${CATCH_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/src/main)
target_link_libraries(Tests NeonParser NeonAstGenerator)
catch_discover_tests(Tests)
