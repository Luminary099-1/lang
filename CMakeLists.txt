cmake_minimum_required(VERSION 3.28)
project(language)

add_custom_target(
	build_carburetta
	COMMAND msbuild ${PROJECT_SOURCE_DIR}/tools/carburetta/carburetta.sln
)

add_custom_target(
	run_carburetta
	COMMAND ${PROJECT_SOURCE_DIR}/tools/carburetta/build/Win_amd64/Release/carburetta.exe ${PROJECT_SOURCE_DIR}/src/compiler/carb.cbrt --c ${PROJECT_SOURCE_DIR}/src/compiler/parse.cpp --h
)

add_executable(compiler src/compiler/main.cpp)
target_include_directories(src/compiler/syntaxTree/)
set_property(TARGET compiler PROPERTY CXX_STANDARD 17)
