# Carburetta generate command:
file(GLOB CARB_EXE ${CARB_DIR}/build/*/Release/carburetta.exe)
add_custom_target(parser_generate
	COMMAND ${CARB_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/carb.cbrt --c ${CMAKE_CURRENT_SOURCE_DIR}/parse.cpp --h)

# Compiler sources and executables:
add_library(compiler_core STATIC
	syntaxTree/base.cpp
	syntaxTree/expressions.cpp
	syntaxTree/globals.cpp
	syntaxTree/literals.cpp
	syntaxTree/operators.cpp
	syntaxTree/statements.cpp
	parse.cpp
	utilities.cpp
)
add_executable(compiler main.cpp)
target_link_libraries(compiler compiler_core)
add_executable(dump_lex tools/dumpLex.cpp)
target_link_libraries(dump_lex compiler_core)
add_executable(dump_ast tools/dumpAST.cpp)
target_link_libraries(dump_ast compiler_core)
