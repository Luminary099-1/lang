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
	googletest
	GIT_REPOSITORY https://github.com/google/googletest
	GIT_TAG v1.14.0
)
if(WIN32)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(googletest)

add_subdirectory(test)
