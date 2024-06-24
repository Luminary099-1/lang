add_subdirectory(parser)

# Compiler sources and executables:
add_library(compiler_core STATIC
	syntaxTree/base.cpp
	syntaxTree/expressions.cpp
	syntaxTree/globals.cpp
	syntaxTree/literals.cpp
	syntaxTree/operators.cpp
	syntaxTree/statements.cpp
	parser/parser.cpp
	parser/parserTools.cpp
	utilities.cpp
)
add_executable(compiler main.cpp)
target_link_libraries(compiler compiler_core)
add_executable(dump_lex tools/dumpLex.cpp)
target_link_libraries(dump_lex compiler_core)
add_executable(dump_ast tools/dumpAST.cpp)
target_link_libraries(dump_ast compiler_core)
