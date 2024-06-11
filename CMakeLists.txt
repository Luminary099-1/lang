cmake_minimum_required(VERSION 3.28)
project(language)

add_custom_target(
	carb_build
	COMMAND msbuild ${PROJECT_SOURCE_DIR}/tools/carburetta/carburetta.sln
)

add_custom_target(
	carb_run
	COMMAND ${PROJECT_SOURCE_DIR}/tools/carburetta/build/Win_amd64/Release/carburetta.exe ${PROJECT_SOURCE_DIR}/src/compiler/carb.cbrt --c ${PROJECT_SOURCE_DIR}/src/compiler/parse.cpp --h
)

add_executable(lex_dump
	src/compiler/tools/dumpLex.cpp
	src/compiler/utilities.cpp
	src/compiler/parse.cpp
	src/compiler/syntaxTree/base.cpp
	src/compiler/syntaxTree/expressions.cpp
	src/compiler/syntaxTree/globals.cpp
	src/compiler/syntaxTree/literals.cpp
	src/compiler/syntaxTree/operators.cpp
	src/compiler/syntaxTree/statements.cpp
)
set_property(TARGET lex_dump PROPERTY CXX_STANDARD 20)

add_executable(compiler
	src/compiler/main.cpp
	src/compiler/utilities.cpp
	src/compiler/parse.cpp
	src/compiler/syntaxTree/base.cpp
	src/compiler/syntaxTree/expressions.cpp
	src/compiler/syntaxTree/globals.cpp
	src/compiler/syntaxTree/literals.cpp
	src/compiler/syntaxTree/operators.cpp
	src/compiler/syntaxTree/statements.cpp
)
set_property(TARGET compiler PROPERTY CXX_STANDARD 20)
