cmake_minimum_required(VERSION 3.28)
project(language)

add_executable(lang src/Main.cpp)
set_property(TARGET lang PROPERTY CXX_STANDARD 17)
