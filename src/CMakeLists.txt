set(SOURCES
    main.cpp
    Lexer.cpp
    Parser.cpp
    Token.cpp
    Grammar.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${llvm_libs})
