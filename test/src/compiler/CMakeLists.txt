# Parser test:
set(TESTDATADIR ${CMAKE_CURRENT_SOURCE_DIR}/testData)
configure_file(in_testParser.cpp testParser.cpp)
add_executable(test_parser testParser.cpp)
target_include_directories(test_parser PRIVATE ${CMAKE_SOURCE_DIR}/src/compiler)
target_link_libraries(
	test_parser PRIVATE
	compiler_core
	CppUTest::CppUTest
    CppUTest::CppUTestExt
)
