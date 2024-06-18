cmake_minimum_required(VERSION 3.8)
project(language LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Carburetta directory and build command:
set(CARB_DIR ${CMAKE_SOURCE_DIR}/tools/carburetta)
add_custom_target(
	carburetta_build
	COMMAND msbuild ${CARB_DIR}/carburetta.sln -nologo -p:Configuration=Release
)

add_subdirectory(src)

# Testing:
include(FetchContent)
FetchContent_Declare(
	CppUTest
	GIT_REPOSITORY https://github.com/cpputest/cpputest
	GIT_TAG master
)
FetchContent_MakeAvailable(CppUTest)

add_subdirectory(test)
