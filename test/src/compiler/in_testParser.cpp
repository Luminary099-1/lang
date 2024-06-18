#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"

#include "parse.h"
#include "syntaxTree/base.hpp"
#include "syntaxTree/globals.hpp"
#include "utilities.hpp"

#include <memory>


// Evaluates the lexer and parser for each AST node.
TEST_GROUP(Parser) {};


/**
 * Parses the source contained in the passed file. Will fail running tests if an
 * error is encountered while scanning.
 * @param src Path to the source file.
 * @return AST from the parsed source.
 */
std::vector<SyntaxTreeNode*> getAST(const char* src)
{
	std::unique_ptr<TU> tu {std::make_unique<TU>(src)};
	carb_stack stack;
	carb_stack_init(&stack);
	carb_set_input(&stack, tu->_buf, tu->_size, 1);

	std::vector<SyntaxTreeNode*> out;
	while (true)
	{
		switch (carb_scan(&stack, out))
		{
			case _CARB_FINISH:
				break;
			case _CARB_FEED_ME:
				tu->ReadNext();
				carb_set_input(&stack, tu->_buf, tu->_size, 1);
				continue;
			case _CARB_END_OF_INPUT:
				break;
			case _CARB_LEXICAL_ERROR:
				FAIL("Unexpected token.");
				continue;
			case _CARB_SYNTAX_ERROR:
				FAIL("Syntax error.");
				continue;
			default:
				FAIL("Encountered an unrecoverable error, aborting.\n");
		}
		break;
	}

	carb_stack_cleanup(&stack);
	return out;
}


// Empty function without parameters.
TEST(Parser, Empty)
{
	std::vector<SyntaxTreeNode*> prog {getAST("@TESTDATADIR@/testParser/fn_empty.lang")};
	CHECK_TEXT(prog.size() == 1, "Unexpected number of global definitions.");
	Function* fn {dynamic_cast<Function*>(prog[0])};
	CHECK_TEXT(fn != nullptr, "Node is not a function.");
	STRCMP_EQUAL_TEXT(
		"empty", fn->_name->_id.c_str(), "Incorrect function name.");
	CHECK_TEXT(fn->_type->IsVoid(), "Return type is not void.");
	CHECK_TEXT(fn->_params.size() == 0, "Unexpected parameter(s).");
	CHECK_TEXT(fn->_body.size() == 0, "Function body is non-empty.");
	delete fn; // FIXME: Temporary to satisfy the memory-leak detector.
}


/**
 * Sample parser test runner.
 * @param argc Number of command line arguments.
 * @param argv Command line arguments,
 * @return Program exit status code.
 */
int main(int argc, char** argv)
{
	return CommandLineTestRunner::RunAllTests(argc, argv);
}
