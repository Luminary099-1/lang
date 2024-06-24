#include "gtest/gtest.h"

#include "parser/parser.h"
#include "parser/parserTools.hpp"
#include "syntaxTree/base.hpp"
#include "syntaxTree/globals.hpp"
#include "utilities.hpp"

#include <memory>


// Evaluates the parsing of an empty function.
TEST(Parser, Function_Empty)
{
	std::unique_ptr<TU> tu;
	carb_stack stack;
	carb_stack_init(&stack);

	tu = std::make_unique<TU>("@TESTDATADIR@/testParser/fn_empty.lang");
	carb_set_input(&stack, tu->GetBuf(), tu->GetSize(), tu->IsFinal());
	AST prog {};
	ASSERT_TRUE(Parser::getAST(stack, *tu, prog))
		<< "An error occured constructing the AST.";
	ASSERT_EQ(1, prog.size())
		<< "Incorrect number of top-level AST nodes.";
	Function* fn {dynamic_cast<Function*>(prog[0].get())};
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

	carb_stack_cleanup(&stack);
}
