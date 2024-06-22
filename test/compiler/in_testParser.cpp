#include "gtest/gtest.h"

#include "parse.h"
#include "syntaxTree/base.hpp"
#include "syntaxTree/globals.hpp"
#include "utilities.hpp"

#include <memory>


AST getAST(carb_stack& stack, TU& tu)
{
	AST out;
	while (true)
	{
		switch (carb_scan(&stack, out))
		{
			case _CARB_FINISH:
				break;
			case _CARB_FEED_ME:
				tu.ReadNext();
				carb_set_input(&stack, tu._buf, tu._size, 1);
				continue;
			case _CARB_END_OF_INPUT:
				break;
			case _CARB_LEXICAL_ERROR:
				// TODO: Fail
				continue;
			case _CARB_SYNTAX_ERROR:
				// TODO: Fail
				continue;
			// default:
			// 	TODO: Fail
		}
		break;
	}

	return out;
}


TEST(Parser, Function_Empty)
{
	std::unique_ptr<TU> tu;
	carb_stack stack;
	carb_stack_init(&stack);

	tu = std::make_unique<TU>("@TESTDATADIR@/testParser/fn_empty.lang");
	carb_set_input(&stack, tu->_buf, tu->_size, 1);
	AST prog {getAST(stack, *tu)};
	ASSERT_EQ(1, prog.size());
	Function* fn {dynamic_cast<Function*>(prog[0].get())};
	ASSERT_NE(fn, nullptr);
	EXPECT_STREQ("empty", fn->_name->_id.c_str());
	EXPECT_TRUE(fn->_type->IsVoid());
	EXPECT_EQ(0, fn->_params.size());
	EXPECT_EQ(0, fn->_body.size());

	carb_stack_cleanup(&stack);
}
