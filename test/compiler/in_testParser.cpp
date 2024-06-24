#include "gtest/gtest.h"

#include "parser/parser.h"
#include "parser/parserTools.hpp"
#include "syntaxTree/base.hpp"
#include "syntaxTree/globals.hpp"
#include "utilities.hpp"

#include <memory>


// Test fixture for testing parser output.
class ParserTest
	: public testing::Test
{
protected:
	carb_stack _stack;	// Parser structure.
	AST _ast;			// AST output.
	bool _success;		// Indicates the parsing was successful.

	ParserTest()
	{
		carb_stack_init(&_stack);
	}

	~ParserTest()
	{
		carb_stack_cleanup(&_stack);
	}

	void Load(const char* src_path)
	{
		TU tu (src_path);
		carb_set_input(&_stack, tu.GetBuf(), tu.GetSize(), tu.IsFinal());
		_success = Parser::getAST(_stack, tu, _ast);
	}
};


// Empty void function with no parameters.
TEST_F(ParserTest, Function_Empty)
{
	Load("@TESTDATADIR@/testParser/fn_empty.lang");
	ASSERT_TRUE(_success)
		<< "An error occured constructing the AST.";
	ASSERT_EQ(1, _ast.size())
		<< "Incorrect number of top-level AST nodes.";
	Function* fn {dynamic_cast<Function*>(_ast[0].get())};
	ASSERT_NE(fn, nullptr)
		<< "Expected a function node.";
	EXPECT_STREQ("empty", fn->_name->_id.c_str())
		<< "Expected a function called 'empty'.";
	EXPECT_TRUE(fn->_type->IsVoid())
		<< "Expected a function of type void.";
	EXPECT_EQ(0, fn->_params.size())
		<< "Unexpected function parameters.";
	EXPECT_EQ(0, fn->_body.size())
		<< "Unexpected function body statements.";
}
